# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: GPL-2.0-only

cmake_minimum_required(VERSION 3.14)
project(middleware_controller)


find_package(ns3 REQUIRED)
find_package(msgpack-cxx REQUIRED)
find_package(PahoMqttCpp REQUIRED)
find_package(zenohc REQUIRED)
find_package(zenohcxx REQUIRED)
find_package(yaml-cpp REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)

set(CMAKE_CXX_STANDARD 20)

option(EXCLUDE_TIMESTAMP_IN_TELEOP_STATE "Do not replace teleop state message content with timestamp" OFF)


add_subdirectory(src)
