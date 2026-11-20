#include "Decoder.hpp"

static int32_t signExtend(uint32_t value, unsigned bits) {
    const uint32_t sign = 1u << (bits - 1);
    return static_cast<int32_t>((value ^ sign) - sign);
}

DecodedInstruction Decoder::decode(uint32_t ins) {
    DecodedInstruction d{};
    d.raw = ins;
    d.opcode = ins & 0x7f;
    d.rd = (ins >> 7) & 0x1f;
    d.funct3 = (ins >> 12) & 0x7;
    d.rs1 = (ins >> 15) & 0x1f;
    d.rs2 = (ins >> 20) & 0x1f;
    d.funct7 = (ins >> 25) & 0x7f;

    switch (d.opcode) {
        case 0x33: d.format = 'R'; break;
        case 0x13:
        case 0x03:
        case 0x67: d.format = 'I'; break;
        case 0x23: {
            d.format = 'S';
            const uint32_t imm = ((ins >> 25) << 5) | ((ins >> 7) & 0x1f);
            d.immediate = signExtend(imm, 12);
            return d;
        }
        case 0x63: {
            d.format = 'B';
            const uint32_t imm = (((ins >> 31) & 1) << 12) |
                                 (((ins >> 7) & 1) << 11) |
                                 (((ins >> 25) & 0x3f) << 5) |
                                 (((ins >> 8) & 0xf) << 1);
            d.immediate = signExtend(imm, 13);
            return d;
        }
        case 0x37:
        case 0x17:
            d.format = 'U';
            d.immediate = static_cast<int32_t>(ins & 0xfffff000u);
            return d;
        case 0x6f: {
            d.format = 'J';
            const uint32_t imm = (((ins >> 31) & 1) << 20) |
                                 (((ins >> 12) & 0xff) << 12) |
                                 (((ins >> 20) & 1) << 11) |
                                 (((ins >> 21) & 0x3ff) << 1);
            d.immediate = signExtend(imm, 21);
            return d;
        }
        default: d.format = '?'; return d;
    }

    if (d.format == 'I') {
        if (d.opcode == 0x13 && (d.funct3 == 1 || d.funct3 == 5))
            d.immediate = static_cast<int32_t>((ins >> 20) & 0x1f);
        else
            d.immediate = signExtend(ins >> 20, 12);
    }
    return d;
}

const char* Decoder::formatName(char f) {
    switch (f) {
        case 'R': return "R";
        case 'I': return "I";
        case 'S': return "S";
        case 'B': return "B";
        case 'U': return "U";
        case 'J': return "J";
        default: return "desconocido";
    }
}
