#include "simulator.hpp"
#include <chrono>
#include <complex>
#include <cstdint>
#include <future>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include "gtest/gtest.h"
#include "LC3Helper.h"

simulator sim3;

TEST(CheckTRAP, All) {

        sim3.setPC(0x3000 - 1);
        sim3.doInst(TRAP | SETDR(1) | 0x2);
        EXPECT_EQ(sim3.getReg(7), 0x3000);
        EXPECT_EQ(sim3.getPC(), sim3.memory[0x0002]);

        sim3.setPC(0x4000 - 1);
        sim3.doInst(TRAP | SETDR(1) | 0x20);
        EXPECT_EQ(sim3.getReg(7), 0x4000);
        EXPECT_EQ(sim3.getPC(), sim3.memory[0x0020]);

}
