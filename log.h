#pragma once

#include <iostream>

#ifndef NDEBUG
#define DEBUG_LOG std::cout
#else
#define DEBUG_LOG if (false) std::cout
#endif
