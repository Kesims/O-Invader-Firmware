cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(bugpack_base LANGUAGES C)

# Fetch all component source files
file(GLOB_RECURSE COMPONENT_SOURCES src/components/*.c)

target_sources(app PRIVATE
        src/main.c
        ${COMPONENT_SOURCES} # Include all component source files
)

# Include all component header files
target_include_directories(app PRIVATE src/components)

# Set the path to the pm_static.yml file
set(PM_STATIC_YML_FILE pm_static.yml)

# Include the pm_static.yml file for MCUboot
set_property(GLOBAL PROPERTY PROPERTY_PM_STATIC_YML_FILE ${PM_STATIC_YML_FILE})
