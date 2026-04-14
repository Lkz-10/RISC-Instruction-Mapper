#include "../include/Encoder.hpp"
#include <cmath>
#include <algorithm>

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
    if (count <= 1) return 1;
    return static_cast<int>(std::ceil(std::log2(number)));
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

    std::map<std::string, std::pair<int, int>> fields_positions{};
    for (const auto& field : specs_.fields) {
        int width = field.width;
        if (current_msb - width + 1 < 0) {
            throw std::string{"Error: total field width exceeds required total instruction width"};
        }
        fields_positions[field.name] = {current_msb, current_msb - width + 1};
        current_msb -= width;
    }

}
