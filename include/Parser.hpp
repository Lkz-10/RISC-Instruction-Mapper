#pragma once

#include "Models.hpp"
#include <string>

class Parser
{
    public:
        static EncodingSpecs_t parseRequirements(const std::string& filename);
};
