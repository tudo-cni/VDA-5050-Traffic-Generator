# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: GPL-2.0

add_library(Zenoh zenoh.cpp)
target_link_libraries(Zenoh Utils zenohcxx_zenohc)
