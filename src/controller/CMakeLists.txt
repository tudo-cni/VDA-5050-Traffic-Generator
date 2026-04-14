# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: GPL-2.0-only

add_executable(Controller
                    controller.cpp
                    controller_interface_zmq.cpp
                    agv.cpp
                    config.cpp
)
target_link_libraries(Controller
                        zmq
                        ns3::core
                        vda5050_message_structs
                        Utils
                        yaml-cpp
)
