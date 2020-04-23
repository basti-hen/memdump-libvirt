cmake_minimum_required(VERSION 3.6)
project("Trufflepig MemDumpLibvirt")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wextra -Wall -fPIC -fPIE")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(SRC src/main.cpp)

add_executable(memdump-libvirt ${SRC})
target_link_libraries(memdump-libvirt virt virt-qemu)
