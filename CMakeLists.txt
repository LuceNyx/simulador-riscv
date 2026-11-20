cmake_minimum_required(VERSION 3.16)
project(riscv_simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(riscv-simulator
    src/main.cpp
    src/Simulator.cpp
    src/Memory.cpp
    src/Decoder.cpp
)

target_include_directories(riscv-simulator PRIVATE include)

include(CTest)
if(BUILD_TESTING)
    add_executable(simulator-tests
        pruebas/simulator_tests.cpp
        src/Simulator.cpp
        src/Memory.cpp
        src/Decoder.cpp
    )
    target_include_directories(simulator-tests PRIVATE include)
    file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" SIMULATOR_SOURCE_DIR_PATH)
    target_compile_definitions(simulator-tests PRIVATE
        SIMULATOR_SOURCE_DIR="${SIMULATOR_SOURCE_DIR_PATH}"
    )
    add_test(NAME simulator-tests COMMAND simulator-tests)
endif()

if(MSVC)
    target_compile_options(riscv-simulator PRIVATE /W4)
else()
    target_compile_options(riscv-simulator PRIVATE -Wall -Wextra -Wpedantic)
endif()
