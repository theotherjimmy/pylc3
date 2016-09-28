cmake_minimum_required(VERSION 3.2)
project(pylc3)

set( CMAKE_VERBOSE_MAKEFILE on )
# Locate Python stuff
find_package(PythonLibs 2.7 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
find_package(PythonInterp)

find_package(Boost)
find_package(Boost COMPONENTS python)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/inc)
PYTHON_ADD_MODULE(pylc3 src/pyInterface.cpp src/simulator.cpp)
add_library(libpylc3 SHARED src/pyInterface.cpp src/simulator.cpp)

#set(CMAKE_SHARED_LIBRARY_PREFIX "")
target_link_libraries(pylc3 
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARIES})

target_link_libraries(libpylc3 
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARIES})
set(CMAKE_SHARED_LIBRARY_PREFIX "")
target_compile_definitions(pylc3 PRIVATE PREFIX="${DESTDIR}")
target_compile_features(pylc3 PRIVATE cxx_range_for)
target_compile_features(libpylc3 PRIVATE cxx_range_for)
target_compile_definitions(libpylc3 PRIVATE PREFIX="${DESTDIR}")
install(TARGETS pylc3 DESTINATION /usr/lib/python2.7/dist-packages)
install(TARGETS libpylc3 DESTINATION /usr/lib)

file(GLOB INCLUDES inc/*.hpp)
install(FILE ${INCLUDES} DESTINATION /usr/lib/pylc3)
