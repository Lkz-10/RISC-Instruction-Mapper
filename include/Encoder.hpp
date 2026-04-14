#pragma once

#include "Models.hpp"
#include <vector>
#include <string>
#include <map>

struct EncodedField_t
{
    std::string value;
    int lsb;
    int msb;
};

struct EncodedInsn_t
{
    std::string name;
    std::map<std::string, EncodedField_t> fields;
};

class Encoder
{
    public:
        explicit Encoder(const EncodingSpecs_t& specs);

        std::vector<EncodedInsn_t> generateLayout();

    private:
        EncodingSpecs_t specs_;

        std::string toBinary(int value, int width) const;
        int calcWidth(int number) const;
}
