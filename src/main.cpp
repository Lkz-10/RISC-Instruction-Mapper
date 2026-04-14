#include <iostream>
#include "Models.hpp"
#include "Parser.hpp"

namespace{constexpr int kRequiredArgsCnt = 2;}

int main(const int argc, const char* argv[])
{
    if (argc < kRequiredArgsCnt) {
        std::cerr << "Usage: " << argv[0] << " <config_file.json>" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        std::cout << "Parsing started..." << std::endl;
        const EncodingSpecs_t specs = Parser::parseRequirements(argv[1]);
        std::cout << "Parsing completed" << std::endl;
    }
    catch (std::string err_msg)
    {
        std::cerr << err_msg << std::endl;
    }

    return 0;
}
