#include "GraphLoader.hpp"
#include <filesystem>
#include <boost/json.hpp>

namespace p1 {
using namespace boost;

auto
read(std::string const & filename) {
  std::string res;
  res.resize(std::filesystem::file_size(filename));
  auto f = fopen(filename.c_str(), "rb");
  if (f) {
    std::fread(res.data(), 1, res.size(), f);
  }
  fclose(f);
  return res;
}

json::value
read_file_json(std::string const & filename) {
  return json::parse(read(filename));
}

std::vector<Graph>
create_graphs(json::value const & file_json) {
  auto               levels_ary = file_json.at("levels").as_array();
  std::vector<Graph> graphs;
  graphs.reserve(levels_ary.size());
  for (auto const & level_val : levels_ary) {
    auto const & level_obj = level_val.as_object();
    graphs.emplace_back(level_obj);
  }
  return graphs;
}

std::vector<Graph>
create_graphs(std::string const & filename) {
  auto file_json = read_file_json(filename);
  return create_graphs(file_json);
}

} // namespace p1
