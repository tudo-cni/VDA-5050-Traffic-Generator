# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: GPL-2.0

add_executable(UeMqtt ue_mqtt.cpp)
target_link_libraries(UeMqtt Utils Mqtt)

add_executable(UeZenoh ue_zenoh.cpp)
target_link_libraries(UeZenoh Utils Zenoh)
