cmake_minimum_required(VERSION 3.17)
project(SPO_lab1_0 C)

set(CMAKE_C_STANDARD 99)

add_executable(SPO_lab1_0 Devices.c main.c xfs.c xfs.h xfs_structs.c xfs_types.h list.c list.h)
target_link_libraries(SPO_lab1_0  blkid)