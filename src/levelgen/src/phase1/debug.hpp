#pragma once

#if defined DEBUG
#include <iostream>

#define DEBUGTRACE                                                             \
  std::cout << "*** TRACE:  " << __FILE__ << ":" << __LINE__ << std::endl
#else
#define DEBUGTRACE
#endif
