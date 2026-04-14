#pragma once

#include "Models.hpp"
#include <vector>
#include <string>

struct EncodedField_t
{
    int msb;
    int lsb;
    std::string name;
    std::string value;
};

struct EncodedInsn_t
{
    std::string name;
    std::vector<EncodedField_t> fields;
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
        bool isFieldFlexible(const std::string& name) const;
};
