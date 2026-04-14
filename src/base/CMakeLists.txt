# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: GPL-2.0

add_executable(BaseMqtt base_mqtt.cpp)
target_link_libraries(BaseMqtt Utils Mqtt)

add_executable(BaseZenoh base_zenoh.cpp)
target_link_libraries(BaseZenoh Utils Zenoh)
