#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <Types.hpp>
#include <utils.hpp>
#include <nlohmann/json.hpp>

struct PrimitiveInfo {
    etugl::mat4f matrix;
    etugl::vec4f color;
    int type;
    int id_node;

    static etugl::mat4f jsonArrayToMat4(const nlohmann::json& json_matrix) {
        etugl::mat4f mat(1.0f);
        if (json_matrix.is_array() && json_matrix.size() == 4) {
            for (int i = 0; i < 4; ++i) {
                if (json_matrix[i].is_array() && json_matrix[i].size() == 4) {
                    for (int j = 0; j < 4; ++j) {
                        mat[i][j] = json_matrix[i][j].get<float>();
                    }
                }
            }
        }
        return mat;
    }

    static etugl::vec4f jsonArrayToVec4(const nlohmann::json& json_vector) {
        if (json_vector.is_array() && json_vector.size() == 4) {
            return etugl::vec4f(
                json_vector[0].get<float>(),
                json_vector[1].get<float>(),
                json_vector[2].get<float>(),
                json_vector[3].get<float>()
            );
        }
        return etugl::vec4f(0.0f); 
    }
};

static inline bool formatJsonFileToVectors(
    const std::filesystem::path& file_path,
    std::vector<PrimitiveInfo>& primitives,
    std::vector<int>& nodes,
    std::vector<int>& parents,
    std::vector<std::vector<int>>& children
) {
    const std::string file_path_str = file_path.string();

    primitives.clear();
    nodes.clear();
    parents.clear();
    children.clear();

    std::ifstream file_stream(file_path);
    if (!file_stream.is_open()) {
        LOG_ERROR("Error in file opening JSON at {}", file_path_str);
        return false;
    }

    nlohmann::json parsed_json;
    try {
        file_stream >> parsed_json;
    } catch (const nlohmann::json::parse_error& e) {
        std::string error = e.what();
        LOG_ERROR("JSON parsing error in file {}: {}", file_path_str, error);
        return false;
    }

    if (parsed_json.count("primitives") && parsed_json["primitives"].is_array()) {
        for (const auto& p_json : parsed_json["primitives"]) {
            PrimitiveInfo p_info;
            p_info.matrix = PrimitiveInfo::jsonArrayToMat4(p_json["matrix"]);
            p_info.color = PrimitiveInfo::jsonArrayToVec4(p_json["color"]);
            p_info.type = p_json["type"].get<int>();
            p_info.id_node = p_json["id_node"].get<int>();
            primitives.push_back(p_info);
        }
    }

    if (parsed_json.count("nodes") && parsed_json["nodes"].is_array()) {
        for (const auto& node_idx_json : parsed_json["nodes"]) {
            nodes.push_back(node_idx_json.get<int>());
        }
    }

    if (parsed_json.count("parent") && parsed_json["parent"].is_array()) {
        for (const auto& parent_idx_json : parsed_json["parent"]) {
            parents.push_back(parent_idx_json.get<int>());
        }
    }

    if (parsed_json.count("children") && parsed_json["children"].is_array()) {
        for (const auto& children_arr_json : parsed_json["children"]) {
            std::vector<int> current_children;
            if (children_arr_json.is_array()) {
                for (const auto& child_idx_json : children_arr_json) {
                    current_children.push_back(child_idx_json.get<int>());
                }
            }
            children.push_back(current_children);
        }
    }

    LOG_INFO("Successfully parsed JSON file at {}", file_path_str);
    return true; 
}