# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: GPL-2.0-only

add_subdirectory(base)
add_subdirectory(controller)
add_subdirectory(ue)
add_subdirectory(utils)
add_subdirectory(mqtt)
add_subdirectory(zenoh)

# Add clang-format target to execute formatting on all source files
find_program(CLANG_FORMAT_EXECUTABLE clang-format)

if(CLANG_FORMAT_EXECUTABLE)
    file(GLOB_RECURSE ALL_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/include/*.hpp
    )

    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE}
        -i
        -style=file
        ${ALL_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()
