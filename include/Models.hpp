#pragma once

#include <string>
#include <vector>

struct Field_t
{
    std::string name;
    int width;
    bool isFlexible;
};

struct InstructionsFormat_t
{
    std::vector<std::string> insns;
    std::vector<std::string> operands;
    std::string formatName;
    std::string comment;
};

struct EncodingSpecs_t
{
    int totalWidth;
    std::vector<Field_t> fields;
    std::vector<InstructionsFormat_t> formats;
};
