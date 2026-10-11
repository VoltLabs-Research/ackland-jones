cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(ackland-jones VERSION 2.0.0 LANGUAGES CXX)

if(NOT TARGET coretoolkit::coretoolkit)
    find_package(coretoolkit REQUIRED)
endif()

volt_add_plugin(ackland-jones
    MAIN_SOURCE src/plugin.cpp
)
