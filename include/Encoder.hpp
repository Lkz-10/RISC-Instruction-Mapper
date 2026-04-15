#pragma once

#include "Models.hpp"
#include <vector>
#include <string>
#include <map>

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

struct Placement_t
{
    int msb;
    int lsb;
};

struct Block_t
{
    int msb;
    int lsb;
    int size() const {return msb - lsb + 1;}
};

class Encoder
{
    public:
        explicit Encoder(const EncodingSpecs_t& specs);
        std::vector<EncodedInsn_t> generateLayout();

    private:
        EncodingSpecs_t specs_;
        std::map<std::string, Placement_t> global_placement_;

        std::string toBinary(int value, int width) const;
        int calcWidth(int number) const;
        bool isFieldFlexible(const std::string& name) const;
        int getFieldMinWidth(const std::string& name) const;

        std::vector<Block_t> findFreeBlocks(const InstructionsFormat_t& fmt) const;
        bool solvePlacement(std::vector<std::string> unassigned,
                            std::vector<Block_t> blocks,
                            std::map<std::string, Placement_t>& local_results);

        void setBasicFields();
        void mapFormatFields();
};
