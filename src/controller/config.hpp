// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include <yaml-cpp/yaml.h>

struct TeleopConfig
{
    int start_s;
    int stop_s;
    uint32_t control_command_period_ms;
    uint32_t request_teleoperation_capabilities_after_ms;
    uint32_t send_teleoperation_capability_after_request_ms;
    uint32_t teleoperation_state_after_toggle_teleop_ms;
    uint32_t state_after_toggle_teleop_ms;
    uint32_t restart_order_simulation_after_teleoperation_stop_ms;
    uint32_t control_stream_start_after_teleop_state_ms;
};

struct Vda5050Config
{
    std::string manufacturer;
    std::string serial_number;
    uint32_t visualization_period_ms;
    uint32_t send_connection_after_ms;
    uint32_t send_state_after_ms;
    uint32_t send_factsheet_request_after_ms;
    uint32_t send_order_after_ms;
    uint32_t start_visualization_after_ms;
    uint32_t factsheet_response_time_ms;
    uint32_t send_order_continuation_ms;
    uint32_t send_state_after_order_ms;
    uint32_t idle_state_interval_ms;
    uint32_t order_finish_step_ms;
    uint32_t next_order_after_finish_ms;
    uint32_t order_len_min;
    uint32_t order_len_max;
    TeleopConfig teleop_cfg{};
};

enum class Scenario
{
    VDA5050,
    TELEOP,
};

struct Config
{
    int length_s;
    int seed;
    Scenario scenario;
    Vda5050Config vda5050_cfg{};
};

Config parseConfig(const std::string &path);
