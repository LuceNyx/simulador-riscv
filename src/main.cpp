#include "Simulator.hpp"
#include <exception>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

static uint32_t parseAddress(const std::string& s) {
    std::size_t used = 0;
    const auto value = std::stoull(s, &used, 0);
    if (used != s.size() || value > 0xffffffffull)
        throw std::invalid_argument("Direccion invalida: " + s);
    return static_cast<uint32_t>(value);
}

static void help() {
    std::cout <<
        "Comandos disponibles:\n"
        "  step [n]          Ejecuta n instrucciones (por defecto 1)\n"
        "  run [limite]      Ejecuta varias instrucciones (por defecto 1000000)\n"
        "  pc                Muestra el PC\n"
        "  regs [registros]  Muestra todos o los indicados (x5, a0, sp...)\n"
        "  reg xN            Muestra un registro o alias (ej. reg a0)\n"
        "  mem INICIO FIN    Muestra bytes de memoria (hex o decimal)\n"
        "  memw dir [n]      Muestra n palabras de 32 bits desde dir\n"
        "  break dir         Agrega breakpoint; delete dir lo quita; breaks lista\n"
        "  reset             Reinicia PC y registros\n"
        "  help              Muestra esta ayuda\n"
        "  exit              Cierra el simulador\n";
}

static int parseRegister(const std::string& name) {
    static const char* aliases[32] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };
    if (name == "fp") return 8;
    for (int i = 0; i < 32; ++i)
        if (name == aliases[i] || name == "x" + std::to_string(i)) return i;
    return -1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: riscv-simulator <programa.bin>\n";
        return 1;
    }

    try {
        Simulator simulator;
        if (!simulator.loadProgram(argv[1])) {
            std::cerr << "No se pudo abrir el archivo: " << argv[1] << '\n';
            return 1;
        }

        std::cout << '"' << argv[1] << "\" cargado desde 0x00000000.\n";
        help();

        std::string line;
        std::set<uint32_t> breakpoints;
        while (std::cout << "> " && std::getline(std::cin, line)) {
            std::istringstream input(line);
            std::vector<std::string> args;
            std::string token;
            while (input >> token) args.push_back(token);
            if (args.empty()) continue;

            try {
                const auto& cmd = args[0];
                if (cmd == "exit" || cmd == "quit") break;
                else if (cmd == "help") help();
                else if (cmd == "pc") simulator.showPC();
                else if (cmd == "regs") {
                    if (args.size() == 1) simulator.showRegisters();
                    else for (std::size_t i = 1; i < args.size(); ++i) {
                        if (args[i] == "pc") simulator.showPC();
                        else {
                            const int index = parseRegister(args[i]);
                            if (index < 0) throw std::invalid_argument("Registro invalido: " + args[i]);
                            simulator.showRegister(static_cast<unsigned>(index));
                        }
                    }
                }
                else if (cmd == "reg") {
                    if (args.size() != 2) throw std::invalid_argument("Uso: reg REGISTRO");
                    const int index = parseRegister(args[1]);
                    if (index < 0) throw std::invalid_argument("Registro invalido: " + args[1]);
                    simulator.showRegister(static_cast<unsigned>(index));
                }
                else if (cmd == "mem") {
                    if (args.size() != 3) throw std::invalid_argument("Uso: mem INICIO FIN");
                    simulator.showMemory(parseAddress(args[1]), parseAddress(args[2]));
                }
                else if (cmd == "memw") {
                    if (args.size() < 2 || args.size() > 3)
                        throw std::invalid_argument("Uso: memw DIRECCION [CANTIDAD]");
                    const uint32_t count = args.size() == 3 ? parseAddress(args[2]) : 1;
                    simulator.showMemoryWords(parseAddress(args[1]), count);
                }
                else if (cmd == "step") {
                    if (args.size() > 2) throw std::invalid_argument("Uso: step [CANTIDAD]");
                    const uint64_t count = args.size() == 2 ? std::stoull(args[1]) : 1;
                    for (uint64_t i = 0; i < count && !simulator.isHalted(); ++i)
                        if (!simulator.step()) break;
                }
                else if (cmd == "run") {
                    uint64_t limit = args.size() >= 2 ? std::stoull(args[1]) : 1000000;
                    uint64_t count = 0;
                    while (!simulator.isHalted() && count < limit) {
                        if (count > 0 && breakpoints.count(simulator.getPC())) break;
                        if (!simulator.step()) break;
                        ++count;
                    }
                    if (!simulator.isHalted() && count == limit)
                        std::cout << "Se alcanzo el limite de " << limit << " instrucciones.\n";
                    else if (!simulator.isHalted() && breakpoints.count(simulator.getPC()))
                        std::cout << "Breakpoint en 0x" << std::hex << std::setw(8)
                                  << std::setfill('0') << simulator.getPC() << std::dec
                                  << std::setfill(' ') << ".\n";
                    std::cout << "Instrucciones ejecutadas en esta corrida: " << count << '\n';
                }
                else if (cmd == "break" || cmd == "delete") {
                    if (args.size() != 2) throw std::invalid_argument("Uso: break|delete DIRECCION");
                    const auto address = parseAddress(args[1]);
                    if (cmd == "break") breakpoints.insert(address);
                    else breakpoints.erase(address);
                }
                else if (cmd == "breaks") {
                    for (const auto address : breakpoints)
                        std::cout << "breakpoint 0x" << std::hex << std::setw(8)
                                  << std::setfill('0') << address << std::dec
                                  << std::setfill(' ') << '\n';
                }
                else if (cmd == "reset") {
                    simulator.reset();
                    std::cout << "PC y registros reiniciados; el programa sigue cargado.\n";
                }
                else std::cout << "Comando desconocido. Escribe 'help'.\n";
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error fatal: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
