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

simulator sim4;

TEST(CheckRTI, All) {

        sim4.setReg(6, 0x3000);
        sim4.memory[0x3000] = 0xDEAD;
        sim4.memory[0x3001] = (1<<15) | (1<<2) | (1<<1) | (1<<0);
        sim4.setPC(0x4000);
        sim4.setPcsrBit('n',false);
        sim4.setPcsrBit('z',false);
        sim4.setPcsrBit('p',false);
        sim4.setPcsrBit('s',false);
        EXPECT_EQ(sim4.doInst(RTI), true);
        EXPECT_EQ(sim4.getPC(), 0xDEAD);
        EXPECT_EQ(sim4.getPcsrBit('n'), true);
        EXPECT_EQ(sim4.getPcsrBit('z'), true);
        EXPECT_EQ(sim4.getPcsrBit('p'), true);
        EXPECT_EQ(sim4.getPcsrBit('s'), true);

        sim4.setReg(6, 0x3000);
        sim4.memory[0x3000] = 0xDEAD;
        sim4.memory[0x3001] = (1<<15) | (1<<2) | (1<<1) | (1<<0);
        sim4.setPC(0x4000);
        sim4.setPcsrBit('n',false);
        sim4.setPcsrBit('z',false);
        sim4.setPcsrBit('p',false);
        sim4.setPcsrBit('s',true);
        EXPECT_EQ(sim4.doInst(RTI), false);
        EXPECT_EQ(sim4.getPC(), 0x4001);
        EXPECT_EQ(sim4.getPcsrBit('n'), false);
        EXPECT_EQ(sim4.getPcsrBit('z'), false);
        EXPECT_EQ(sim4.getPcsrBit('p'), false);
        EXPECT_EQ(sim4.getPcsrBit('s'), true);

}
