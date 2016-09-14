#pragma once

#include <boost/python.hpp>
#include "simulator.hpp"
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

namespace py = boost::python;
using MEM_type = vector<uint16_t>;

extern "C" void initpylc3();
extern "C" void init_module_pylc3();

