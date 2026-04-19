#include <iostream>
#include "../include/Models.hpp"
#include "../include/Parser.hpp"
#include "../include/Encoder.hpp"
#include "../include/Exporter.hpp"

namespace{constexpr int kRequiredArgsCnt = 3;}

int main(const int argc, const char* argv[])
{
    if (argc < kRequiredArgsCnt) {
        std::cerr << "Usage: " << argv[0] << " <config_file.json> <output_file.json>" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        std::cout << "Parsing started..." << std::endl;
        const EncodingSpecs_t specs = Parser::parseRequirements(argv[1]);
        std::cout << "Parsing completed" << std::endl;

        std::cout << "Generating layout..." << std::endl;
        Encoder encoder(specs);
        std::vector<EncodedInsn_t> insns = encoder.generateLayout();
        std::cout << "Generating completed" << std::endl;

        std::cout << "Exporting to " << argv[2] << "..." << std::endl;
        Exporter::exportJson(insns, argv[2]);
        std::cout << "Done" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Unknown error" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
