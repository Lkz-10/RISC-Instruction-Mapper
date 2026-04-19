#include "../include/Exporter.hpp"
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

void Exporter::exportJson(const std::vector<EncodedInsn_t>& instructions, const std::string& filename)
{
    json j_array = json::array();

    for (const auto& insn : instructions) {
        json j_insn{};
        j_insn["insn"] = insn.name;

        json j_fields = json::array();
        for (const auto& field : insn.fields) {
            json field_content{};
            field_content["msb"] = field.msb;
            field_content["lsb"] = field.lsb;
            field_content["value"] = field.value;

            json field_wrapper{};
            field_wrapper[field.name] = field_content;

            j_fields.push_back(field_wrapper);
        }
        j_insn["fields"] = j_fields;
        j_array.push_back(j_insn);
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }

    file << j_array.dump(4);
    file.close();
}
