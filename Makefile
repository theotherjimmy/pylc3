
GTEST_DIR := ~/gtest
GTEST_INC := $(GTEST_DIR)/include

#makefile debugging
debug ?= @

#choose based on system
GTEST_LIB := -lgtest

PY_INC := /usr/include/python2.7/

SRC_DIRS += tests
SRC_DIRS += src

INC_DIRS += inc

CXX := g++
CXXFLAGS += -g
CXXFLAGS += -I $(GTEST_INC)
CXXFLAGS += -I $(PY_INC)
CXXFLAGS += $(addprefix -I ,${INC_DIRS})
CXXFLAGS += $(addprefix -I ,${SRC_DIRS})
CXXFLAGS += -std=c++11
CXXFLAGS += -Wall
CXXFLAGS += -Wno-sign-compare
CXXFLAGS += -Werror
CXXFLAGS += -fPIC
DEFS ?= -DPHASE_A -DPHASE_B -DPHASE_C
PYDEFS ?= -DPHASE_A -DPHASE_B -DPHASE_C

LDFLAGS += -lboost_python
LDFLAGS += -lboost_system
LDFLAGS += -lpython2.7

SRCS := $(wildcard $(addsuffix /*.cpp, ${SRC_DIRS}))
OBJS := ${SRCS:.cpp=.o}
DEPS := ${SRCS:.cpp=.d}
TEST := lc3_unittest
PYLC3 := pylc3.so
python-tests := $(wildcard tests/*.py)

all: $(PYLC3)

test: cpp-test python-test

cpp-test: $(TEST)
	${debug}./$(TEST)

python-test: ${PYLC3} ${python-tests:.py=.py-test-out}

%.py-test-out : %.py ${PYLC3}
	-${debug}PYTHONPATH=$(pwd) python2 $< > $@ || cat $@

$(PYLC3): $(filter-out tests/%, ${OBJS})
	${debug}echo "LD  $^"
	${debug}$(CXX) $^ $(PYDEFS) $(CXXFLAGS) -shared ${LDFLAGS} -o $@

$(TEST):  $(filter-out %/pyInterface.cpp, ${OBJS})
	${debug}echo "LD  $^"
	${debug}$(CXX) $^ $(EXTRA_TESTS) $(GTEST_LIB) $(DEFS) $(CXXFLAGS) ${LDFLAGS} -pthread -o $@

-include $(DEPS)

%.o: %.cpp
	${debug}echo "C++ $^"
	${debug}$(CXX) -c ${CXXFLAGS} -MD -o $@ $< -MT ${@:.o=.d}

clean:
	-${debug}rm -rf ${OBJS} ${DEPS} ${TEST}
