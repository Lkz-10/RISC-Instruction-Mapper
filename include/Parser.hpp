#pragma once

#include "Models.hpp"
#include <string>

class Parser
{
    public:
        static EncodingSpecs parseRequirements(const std::string& filename);
};
