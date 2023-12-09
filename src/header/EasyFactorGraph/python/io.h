#pragma once

#include <EasyFactorGraph/io/TrainSetImport.h>
#include <EasyFactorGraph/io/json/Exporter.h>
#include <EasyFactorGraph/io/json/Importer.h>
#include <EasyFactorGraph/io/xml/Exporter.h>
#include <EasyFactorGraph/io/xml/Importer.h>

#include <EasyFactorGraph/python/model.h>

#include <filesystem>
#include <nlohmann/json.hpp>

namespace EFG::python {
// TrainSet import_train_set(const std::string &file_name);

struct xml {
  template <typename Model>
  static void from_file(Model &model, const std::string &file_path) {
    io::xml::Importer::importFromFile<typename Model::ModelT_value>(
        model.getModelMut(), file_path);
  }

  template <typename Model>
  static void to_file(const Model &model, const std::string &file_path) {
    io::xml::Exporter::exportToFile<typename Model::ModelT_value>(
        model.getModel(), io::xml::ExportInfo{file_path});
  }

  template <typename Model> static std::string to_string(const Model &model) {
    return io::xml::Exporter::exportToString<typename Model::ModelT_value>(
        model.getModel(), std::string{"Model"});
  }
};

struct json {
  template <typename Model>
  static void from_file(Model &model, const std::string &file_path) {
    io::json::Importer::importFromFile<typename Model::ModelT_value>(
        model.getModelMut(), file_path);
  }

  template <typename Model>
  static void from_string(Model &model, const std::string &json) {
    nlohmann::json temp = nlohmann::json::parse(json);
    io::json::Importer::importFromJson<typename Model::ModelT_value>(
        model.getModelMut(), temp);
  }

  template <typename Model>
  static void to_file(const Model &model, const std::string &file_path) {
    io::json::Exporter::exportToFile<typename Model::ModelT_value>(
        model.getModel(), file_path);
  }

  template <typename Model> static std::string to_string(const Model &model) {
    return io::json::Exporter::exportToJson<typename Model::ModelT_value>(
               model.getModel())
        .dump();
  }
};
} // namespace EFG::python
