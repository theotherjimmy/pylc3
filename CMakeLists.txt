cmake_minimum_required(VERSION 3.5)
project(pylc3)
find_package(PythonLibs 2.7 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
find_package(Boost)
find_package(Boost COMPONENTS python REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/inc)
add_library(pylc3 SHARED src/pyInterface.cpp src/simulator.cpp)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
target_link_libraries(pylc3 
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARIES})
target_compile_definitions(pylc3 PRIVATE PREFIX="${DESTDIR}")
target_compile_features(pylc3 PRIVATE cxx_range_for)
target_include_directories(pylc3 INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<INSTALL_INTERFACE:include/pylc3>)