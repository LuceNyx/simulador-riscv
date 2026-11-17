#pragma once
#include <cstdint>

struct DecodedInstruction {
    uint32_t raw{};
    uint32_t opcode{};
    uint32_t rd{};
    uint32_t funct3{};
    uint32_t rs1{};
    uint32_t rs2{};
    uint32_t funct7{};
    int32_t immediate{};
    char format{'?'};
};

class Decoder {
public:
    static DecodedInstruction decode(uint32_t instruction);
    static const char* formatName(char format);
};
