cmake_minimum_required(VERSION 3.16)
project(nekosys C CXX ASM)

set(CMAKE_CXX_STANDARD 14)

set(TOOLCHAIN_ROOT /home/$ENV{USER}/opt/neko-gcc/bin)
set(TOOLCHAIN_PREFIX ${TOOLCHAIN_ROOT}/i686-neko-)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER nasm)
set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}ld)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}ar)

add_compile_options(-O2 -fno-exceptions -Wall)
