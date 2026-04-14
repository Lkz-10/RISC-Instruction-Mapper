#include "../include/Encoder.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

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
        if (field.name == name) return field.is_flexible;
    }
    return false;
}

std::vector<EncodedInsn_t> Encoder::generateLayout()
{
    std::vector<EncodedInsn_t> result{};

    int current_msb = specs_.totalWidth - 1;
    int f_width = calcWidth(specs_.formats.size());
    int f_msb = current_msb;
    int f_lsb = current_msb - f_width + 1;
    current_msb = f_lsb - 1;

    int max_opcode_width = 0;
    for (const auto& format : specs_.formats) {
        max_opcode_width = std::max(max_opcode_width, calcWidth(format.insns.size()));
    }
    int opcode_msb = current_msb;
    int opcode_lsb = current_msb - max_opcode_width + 1;
    current_msb = opcode_lsb - 1;

    std::map<std::string, std::pair<int, int>> fields_map{};
    for (const auto& field : specs_.fields) {
        int width = field.width;
        if (current_msb - width + 1 < 0) {
            throw std::runtime_error("Error: total field width exceeds required total instruction width");
        }
        fields_map[field.name] = {current_msb, current_msb - width + 1};
        current_msb -= width;
    }

    for (size_t f_idx = 0; f_idx < specs_.formats.size(); ++f_idx) {
        const auto& format{specs_.formats[f_idx]};

        for (size_t i_idx = 0; i_idx < format.insns.size(); ++i_idx) {
            EncodedInsn_t insn{};
            insn.name = format.insns[i_idx];

            std::vector<EncodedField_t> active_fields{};

            active_fields.push_back({f_msb, f_lsb, "F", toBinary(static_cast<int>(f_idx), f_width)});

            std::string op_name{(format.formatName == "branch") ? "CODE" : "OPCODE"}; // TO README
            active_fields.push_back({opcode_msb, opcode_lsb, op_name, toBinary(static_cast<int>(i_idx), max_opcode_width)});

            for (const auto& op : format.operands) {
                auto it = fields_map.find(op);
                if (it != fields_map.end()) {
                    active_fields.push_back({it->second.first, it->second.second, op, "+"});
                } else {
                    throw std::runtime_error("Error: Missing field definition for " + op);
                }
            }

            std::sort(active_fields.begin(), active_fields.end(),
                      [](const EncodedField_t& a, const EncodedField_t& b) {return a.msb > b.msb;});

            int current_bit = specs_.totalWidth - 1;
            int res_cnt = 0;
            std::vector<EncodedField_t> final_fields{};

            for (size_t i = 0; i < active_fields.size(); ++i) {
                auto& field = active_fields[i];

                if (current_bit > field.msb) {
                    int res_msb = current_bit;
                    int res_lsb = field.msb + 1;
                    std::string res_name{"RES" + std::to_string(res_cnt++)};
                    final_fields.push_back({res_msb, res_lsb, res_name, toBinary(0, res_msb - res_lsb + 1)});
                }

                if (isFieldFlexible(field.name)) {
                    if (i + 1 < active_fields.size()) {
                        field.lsb = active_fields[i+1].msb + 1;
                    } else {
                        field.lsb = 0;
                    }
                }

                final_fields.push_back(field);
                current_bit = field.lsb - 1;

                if (field.lsb == 0) break;
            }

            if (current_bit >= 0) {
                std::string res_name{"RES" + std::to_string(res_cnt++)};
                final_fields.push_back({current_bit, 0, res_name, toBinary(0, current_bit + 1)});
            }

            insn.fields = final_fields;
            result.push_back(insn);
        }
    }

    return result;
}
