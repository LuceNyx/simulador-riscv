#include "Simulator.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

class TemporaryProgram {
public:
    explicit TemporaryProgram(const std::string& name)
        : path_(std::filesystem::temp_directory_path() / name) {}

    ~TemporaryProgram() {
        std::error_code ignored;
        std::filesystem::remove(path_, ignored);
    }

    void write(const std::array<uint32_t, 2>& instructions, std::size_t count) const {
        std::ofstream file(path_, std::ios::binary | std::ios::trunc);
        if (!file) throw std::runtime_error("No se pudo crear el programa de prueba.");
        for (std::size_t i = 0; i < count; ++i) {
            const uint32_t word = instructions[i];
            for (unsigned byte = 0; byte < 4; ++byte)
                file.put(static_cast<char>((word >> (byte * 8)) & 0xff));
        }
        if (!file) throw std::runtime_error("No se pudo escribir el programa de prueba.");
    }

    const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void ebreakStopsOnItsAddress() {
    TemporaryProgram program("riscv-simulator-ebreak-test.bin");
    program.write({0x00100073u, 0u}, 1);
    Simulator simulator;
    require(simulator.loadProgram(program.path().string()), "No se pudo cargar EBREAK.");
    require(simulator.step(), "EBREAK debe contar como una instruccion ejecutada.");
    require(simulator.isHalted(), "EBREAK debe detener el simulador.");
    require(simulator.getPC() == 0, "El PC debe permanecer en la direccion de EBREAK.");
    require(!simulator.step(), "El simulador detenido no debe avanzar otra instruccion.");
}

void ebreakStopsAfterEarlierInstructions() {
    TemporaryProgram program("riscv-simulator-ebreak-after-addi-test.bin");
    program.write({0x02a00093u, 0x00100073u}, 2); // addi x1, x0, 42; ebreak
    Simulator simulator;
    require(simulator.loadProgram(program.path().string()), "No se pudo cargar el programa.");
    require(simulator.step(), "ADDI debe ejecutarse.");
    require(simulator.getPC() == 4, "El PC debe avanzar tras ADDI.");
    require(simulator.step(), "EBREAK debe ejecutarse.");
    require(simulator.isHalted(), "EBREAK debe detener el simulador.");
    require(simulator.getPC() == 4, "El PC debe apuntar a EBREAK.");
}

void otherSystemInstructionsRemainUnsupported() {
    TemporaryProgram program("riscv-simulator-system-test.bin");
    program.write({0x00000073u, 0u}, 1); // ECALL is not implemented.
    Simulator simulator;
    require(simulator.loadProgram(program.path().string()), "No se pudo cargar ECALL.");
    bool rejected = false;
    try {
        simulator.step();
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    require(rejected, "ECALL debe seguir reportandose como no soportado.");
    require(!simulator.isHalted(), "Una instruccion SYSTEM invalida no debe simular EBREAK.");
    require(simulator.getPC() == 0, "El PC no debe avanzar tras una instruccion invalida.");
}

} // namespace

int main() {
    try {
        ebreakStopsOnItsAddress();
        ebreakStopsAfterEarlierInstructions();
        otherSystemInstructionsRemainUnsupported();
        std::cout << "Todas las pruebas del simulador pasaron.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Fallo de prueba: " << error.what() << '\n';
        return 1;
    }
}
