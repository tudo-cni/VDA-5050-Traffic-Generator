# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: GPL-2.0-only

add_library(Mqtt mqtt_client.cpp)
target_link_libraries(Mqtt PahoMqttCpp::paho-mqttpp3-static Utils)
if(EXCLUDE_TIMESTAMP_IN_TELEOP_STATE)
    target_compile_definitions(Mqtt PUBLIC EXCLUDE_TIMESTAMP_IN_TELEOP_STATE)
endif()
