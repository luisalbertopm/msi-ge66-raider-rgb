cmake_minimum_required(VERSION 3.10)

project(MSI-GE66-Raider-RGB VERSION 0.1 LANGUAGES C)

find_package(hidapi REQUIRED)

add_executable(msi-rgb keyboard.h aurora.h hid.h hid.c main.c)

target_link_libraries(msi-rgb PRIVATE hidapi::hidapi)