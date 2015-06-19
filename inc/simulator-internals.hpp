#pragma once

enum opcode {
        BR  = 0,
        ADD = 1,
        LD  = 2,
        ST  = 3,
        JSR = 4,
        AND = 5,
        LDR = 6,
        STR = 7,
        RTI = 8,
        NOT = 9,
        LDI = 10,
        STI = 11,
        JMP = 12,
        RESERVED = 13,
        LEA = 14,
        TRAP = 15,
};

static inline enum opcode inst2opcode( uint16_t inst ) {
        return (enum opcode) ((inst & (0xF << 12)) >> 12);
}

static inline uint16_t inst2sr1( uint16_t inst ) {
        return (inst & (0x7 << 6)) >> 6;
}

static inline uint16_t inst2sr2( uint16_t inst ) {
        return (inst & (0x7 << 0)) >> 0;
}

static inline uint16_t inst2baser( uint16_t inst ) {
        return (inst & (0x7 << 6)) >> 6;
}

static inline uint16_t inst2dr( uint16_t inst ) {
        return (inst & (0x7 << 9)) >> 9;
}

static inline bool inst2n( uint16_t inst ) {
        return (inst & (1 << 11)) >> 11;
}

static inline bool inst2z( uint16_t inst ) {
        return (inst & (1 << 10)) >> 10;
}

static inline bool inst2p( uint16_t inst ) {
        return (inst & (1 << 9)) >> 9;
}

static inline bool inst2steering( uint16_t inst ) {
        return (inst & (1 << 5)) >> 5;
}

static inline uint16_t inst2imm5( uint16_t inst ) {
        if ((inst & (1 << 4)) >> 4) {
                return inst | (0x7FF << 5);
        } else {
                return inst & 0x1F;
        }
}

static inline uint16_t inst2imm6( uint16_t inst ) {
        if ((inst & (1 << 5)) >> 5) {
                return inst | (0x3FF << 6);
        } else {
                return inst & 0x3F;
        }
}

static inline uint16_t inst2imm9( uint16_t inst ) {
        if ((inst & (1 << 8)) >> 8) {
                return inst | (0x7F << 9);
        } else {
                return inst & 0x1FF;
        }
}

static inline uint16_t inst2imm11( uint16_t inst ) {
        if ((inst & (1 << 10)) >> 10) {
                return inst | (0x1F << 11);
        } else {
                return inst & 0x7FF;
        }
}

static inline uint16_t inst2trapvec8( uint16_t inst ) {
        return inst & 0xFF;
}

