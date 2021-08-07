#include "GraphLoader.hpp"
#include "GraphCreator.hpp"
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
  auto levels_ary = file_json.at("levels").as_array();

  std::vector<Graph> graphs;
  graphs.reserve(levels_ary.size());
  int outer_count = 0;
  for (auto const & level_val : levels_ary) {
    Graph cur_graph = GraphCreator(level_val.as_object())
                          .compress_vertices()
                          .group_by_colors()
                          .create();
    std::cout << "creating: " << cur_graph.level_name() << std::endl;
    int inner_count = 0;
    for (Graph const & graph : graphs) {
      if (cur_graph.check_isomorphism(graph)) {
        std::cout << "Warning: Levels are isomorphisms: " << graph.level_name()
                  << "(" << inner_count << ") and " << cur_graph.level_name()
                  << "(" << outer_count << ")\n";
      }
      ++inner_count;
    }
    graphs.push_back(std::move(cur_graph));
    ++outer_count;
  }
  return graphs;
}

std::vector<Graph>
create_graphs(std::string const & filename) {
  auto file_json = read_file_json(filename);
  return create_graphs(file_json);
}

} // namespace p1
