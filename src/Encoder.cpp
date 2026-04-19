#include "../include/Encoder.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <numeric>

Encoder::Encoder(const EncodingSpecs_t& specs) : specs_(specs) {}

std::string Encoder::toBinary(int value, int width) const
{
    std::string result = "";
    for (int i = width - 1; i >= 0; --i) {
        result += ((value >> i) & 1) ? '1' : '0';
    }
    return result;
}

int Encoder::calcWidth(int number) const
{
    if (number <= 1) return 1;
    return static_cast<int>(std::ceil(std::log2(number)));
}

bool Encoder::isFieldFlexible(const std::string& name) const
{
    for (const auto& field : specs_.fields) {
        if (field.name == name) return field.isFlexible;
    }
    return false;
}

int Encoder::getFieldMinWidth(const std::string& name) const
{
    for (const auto& field : specs_.fields) {
        if (field.name == name) return field.width;
    }
    throw std::runtime_error("Unknown field: " + name);
}

void Encoder::setBasicFields() // set "F" and "OPCODE"(if necessary) fields
{
    // Set "F"
    int f_width = calcWidth(specs_.formats.size());
    global_placement_["F"] = {specs_.totalWidth - 1, specs_.totalWidth - f_width};

    // Set "OPCODE"
    int max_op_width = 0;
    for (const auto& fmt : specs_.formats) {
        if (fmt.insns.size() > 1)
            max_op_width = std::max(max_op_width, calcWidth(fmt.insns.size()));
    }

    if (max_op_width > 0) {
        int start_bit = global_placement_["F"].lsb - 1;
        global_placement_["OPCODE"] = {start_bit, start_bit - max_op_width + 1};
    }
}

std::vector<Block_t> Encoder::findFreeBlocks(const InstructionsFormat_t& fmt) const
{
    std::vector<bool> used(specs_.totalWidth, false);

    auto mark = [&](const std::string& name) {
        auto it = global_placement_.find(name);

        if (it != global_placement_.end()) {
            auto range = it->second;
            for (int i = range.lsb; i <= range.msb; ++i) used[i] = true;
        }
    };

    // Mark all used fields
    mark("F");
    if (fmt.insns.size() > 1) mark("OPCODE");
    for (const auto& field : fmt.operands) mark(field);

    // Find all free blocks
    std::vector<Block_t> blocks{};
    int current_msb = -1;
    for (int i = specs_.totalWidth - 1; i >= 0; --i) {
        if (!used[i]) {
            if (current_msb == -1) current_msb = i;
            if (i == 0 || used[i - 1]) {
                blocks.push_back({current_msb, i});
                current_msb = -1;
            }
        } else current_msb = -1;
    }
    return blocks;
}

bool Encoder::solvePlacement(std::vector<std::string> unassigned, std::vector<Block_t> blocks,
                             std::map<std::string, Placement_t>& local_results)
{
    if (unassigned.empty()) return true;

    std::string field = unassigned[0];
    unassigned.erase(unassigned.begin());
    int min_width = getFieldMinWidth(field);
    bool flex = isFieldFlexible(field);

    for (size_t i = 0; i < blocks.size(); ++i) {
        Block_t block = blocks[i];
        if (block.size() >= min_width) {
            Placement_t range{};
            std::vector<Block_t> next_blocks = blocks;

            if (flex) {
                // Take all the block
                range = {block.msb, block.lsb};
                next_blocks.erase(next_blocks.begin() + i);
            } else {
                // Take the highest bits
                range = {block.msb, block.msb - min_width + 1};
                next_blocks[i].msb = range.lsb - 1;
                if (next_blocks[i].size() == 0) next_blocks.erase(next_blocks.begin() + i);
            }

            local_results[field] = range;
            if (solvePlacement(unassigned, next_blocks, local_results)) return true;
            local_results.erase(field);
        }
    }
    return false;
}

void Encoder::mapFormatFields()
{
    std::vector<int> indices(specs_.formats.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Firstly less flexible formats
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        auto count_flex = [&](int idx) {
            int flex_cnt = 0;
            for (const auto& op : specs_.formats[idx].operands)
                if (isFieldFlexible(op)) flex_cnt++;
            return flex_cnt;
        };
        return count_flex(a) < count_flex(b);
    });

    for (int idx : indices) {
        const auto& fmt = specs_.formats[idx];
        std::vector<std::string> unassigned;
        for (const auto& op : fmt.operands) {
            if (global_placement_.find(op) == global_placement_.end()) unassigned.push_back(op);
        }

        if (unassigned.empty()) continue;

        std::map<std::string, Placement_t> local_res{};
        if (solvePlacement(unassigned, findFreeBlocks(fmt), local_res)) {
            global_placement_.insert(local_res.begin(), local_res.end());
        } else {
            throw std::runtime_error("Failed to encode format: " + fmt.formatName);
        }
    }
}

std::vector<EncodedInsn_t> Encoder::generateLayout()
{
    setBasicFields();
    mapFormatFields();

    std::vector<EncodedInsn_t> result{};
    for (size_t f_idx = 0; f_idx < specs_.formats.size(); ++f_idx) {
        const auto& fmt = specs_.formats[f_idx];

        for (size_t i_idx = 0; i_idx < fmt.insns.size(); ++i_idx) {
            EncodedInsn_t insn{};
            insn.name = fmt.insns[i_idx];

            std::vector<EncodedField_t> fields{};

            auto fRange = global_placement_["F"];
            fields.push_back({fRange.msb, fRange.lsb, "F", toBinary(f_idx, fRange.msb - fRange.lsb + 1)});

            if (fmt.insns.size() > 1) {
                auto opcodeRange = global_placement_.at("OPCODE");
                fields.push_back({opcodeRange.msb, opcodeRange.lsb, "OPCODE",
                                  toBinary(i_idx, opcodeRange.msb - opcodeRange.lsb + 1)});
            }

            for (const auto& op : fmt.operands) {
                auto opRange = global_placement_.at(op);
                fields.push_back({opRange.msb, opRange.lsb, op, "+"});
            }

            std::sort(fields.begin(), fields.end(), [](auto& a, auto& b) { return a.msb > b.msb; });

            std::vector<EncodedField_t> final_fields{};
            int current_bit = specs_.totalWidth - 1;
            int res_idx = 0;

            // completing final fields array
            for (const auto& field : fields) {
                // Add RES if necessary
                if (current_bit > field.msb) {
                    int next_stop = field.msb + 1;
                    final_fields.push_back({current_bit, next_stop, "RES" + std::to_string(res_idx++),
                                            toBinary(0, current_bit - next_stop + 1)});
                    current_bit = next_stop - 1;
                }
                final_fields.push_back(field);
                current_bit = field.lsb - 1;
            }

            // Add RES if necessary
            if (current_bit >= 0) {
                final_fields.push_back({current_bit, 0, "RES" + std::to_string(res_idx++),
                                        toBinary(0, current_bit + 1)});
            }

            insn.fields = final_fields;
            result.push_back(insn);
        }
    }
    return result;
}
