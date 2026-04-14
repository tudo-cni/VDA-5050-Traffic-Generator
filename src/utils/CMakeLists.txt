# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: GPL-2.0

add_library(Utils command.h command.cpp main_loop.cpp)
target_link_libraries(Utils zmq msgpack-cxx yaml-cpp PkgConfig::gstreamer PkgConfig::gstreamer-sdp PkgConfig::gstreamer-app PkgConfig::gstreamer-video)
target_include_directories(Utils PUBLIC ${PROJECT_SOURCE_DIR}/src)

if(EXCLUDE_TIMESTAMP_IN_TELEOP_STATE)
    target_compile_definitions(Utils PUBLIC EXCLUDE_TIMESTAMP_IN_TELEOP_STATE)
endif()
