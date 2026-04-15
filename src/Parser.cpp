#include "../include/Parser.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

EncodingSpecs_t Parser::parseRequirements(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {throw std::runtime_error("Could not open file: " + filename);}

    nlohmann::json input_json;
    file >> input_json;

    EncodingSpecs_t data{};

    data.totalWidth = std::stoi(input_json.at("length").get<std::string>());

    for (const auto& field_item: input_json.at("fields")) {
        auto it = field_item.begin();
        Field_t field{};
        field.name = it.key();

        std::string val = it.value();
        if (val.find(">=") == 0) {              //FIND
            field.width = std::stoi(val.substr(2));
            field.isFlexible = true;
        } else {
            field.width = std::stoi(val);
            field.isFlexible = false;
        }
        data.fields.push_back(field);
    }

    for (const auto& insn_item : input_json.at("instructions")) {
        InstructionsFormat_t format{};
        format.formatName = insn_item.at("format").get<std::string>();
        format.comment    = insn_item.at("comment").get<std::string>();
        format.insns      = insn_item.at("insns").get<std::vector<std::string>>();
        format.operands   = insn_item.at("operands").get<std::vector<std::string>>();

        data.formats.push_back(format);
    }

    return data;
}
