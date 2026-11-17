#include "Memory.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

Memory::Memory(std::size_t size) : bytes(size, 0) {}

void Memory::checkRange(uint32_t address, std::size_t count) const {
    const uint64_t end = static_cast<uint64_t>(address) + count;
    if (end > bytes.size()) {
        throw std::out_of_range("Acceso fuera de la memoria simulada: address=" +
                                std::to_string(address));
    }
}

bool Memory::loadBinary(const std::string& path, uint32_t startAddress) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;

    const std::streamsize length = file.tellg();
    if (length < 0 || static_cast<uint64_t>(startAddress) +
                          static_cast<uint64_t>(length) > bytes.size()) {
        throw std::out_of_range("El binario no cabe en la memoria simulada.");
    }

    file.seekg(0, std::ios::beg);
    if (length > 0 &&
        !file.read(reinterpret_cast<char*>(bytes.data() + startAddress), length)) {
        throw std::runtime_error("No se pudo leer completamente el archivo binario.");
    }
    return true;
}

uint8_t Memory::read8(uint32_t a) const {
    checkRange(a, 1);
    return bytes[a];
}

uint16_t Memory::read16(uint32_t a) const {
    checkRange(a, 2);
    return static_cast<uint16_t>(bytes[a]) |
           (static_cast<uint16_t>(bytes[a + 1]) << 8);
}

uint32_t Memory::read32(uint32_t a) const {
    checkRange(a, 4);
    return static_cast<uint32_t>(bytes[a]) |
           (static_cast<uint32_t>(bytes[a + 1]) << 8) |
           (static_cast<uint32_t>(bytes[a + 2]) << 16) |
           (static_cast<uint32_t>(bytes[a + 3]) << 24);
}

void Memory::write8(uint32_t a, uint8_t v) {
    checkRange(a, 1);
    bytes[a] = v;
}

void Memory::write16(uint32_t a, uint16_t v) {
    checkRange(a, 2);
    bytes[a] = static_cast<uint8_t>(v & 0xff);
    bytes[a + 1] = static_cast<uint8_t>((v >> 8) & 0xff);
}

void Memory::write32(uint32_t a, uint32_t v) {
    checkRange(a, 4);
    bytes[a] = static_cast<uint8_t>(v & 0xff);
    bytes[a + 1] = static_cast<uint8_t>((v >> 8) & 0xff);
    bytes[a + 2] = static_cast<uint8_t>((v >> 16) & 0xff);
    bytes[a + 3] = static_cast<uint8_t>((v >> 24) & 0xff);
}

void Memory::dump(uint32_t start, uint32_t end) const {
    if (end < start) throw std::invalid_argument("El final debe ser >= al inicio.");
    const uint64_t count = static_cast<uint64_t>(end) - start + 1;
    checkRange(start, static_cast<std::size_t>(count));

    std::cout << "Memoria 0x" << std::hex << std::setw(8) << std::setfill('0')
              << start << "-0x" << std::setw(8) << end << ":\n";
    for (uint64_t a = start; a <= end; ++a) {
        if ((a - start) % 16 == 0) {
            if (a != start) std::cout << '\n';
            std::cout << "0x" << std::setw(8) << a << ": ";
        }
        std::cout << std::setw(2) << static_cast<unsigned>(bytes[a]) << ' ';
    }
    std::cout << std::dec << std::setfill(' ') << '\n';
}

std::size_t Memory::size() const noexcept { return bytes.size(); }
