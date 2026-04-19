#pragma once

#include "Encoder.hpp"
#include <vector>
#include <string>

class Exporter
{
    public:
        static void exportJson(const std::vector<EncodedInsn_t>& instructions, const std::string& filename);
};
