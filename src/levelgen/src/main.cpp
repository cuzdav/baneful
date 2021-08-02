#include "phase1/Graph.hpp"
#include "phase1/GraphLoader.hpp"

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>

int
main(int argc, char * argv[]) {
  std::cout << "hello levelgen\n";
  if (argc == 2) {
    try {
      std::vector<Graph> graphs = p1::create_graphs(argv[1]);
    }
    catch (std::runtime_error const & e) {
      std::cout << "Caught exception: " << e.what() << std::endl;
    }
  }
  else {
    std::cout << "needs 1 parameter - filename" << std::endl;
  }
}
