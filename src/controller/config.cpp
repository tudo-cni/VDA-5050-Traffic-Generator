// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#include "config.hpp"

template <typename T>
static T getValue(const YAML::Node& node, const std::string& name) {
  if (!node[name]) {
    throw std::invalid_argument((std::ostringstream() << "Missing parameter " << name).str());
  }
  try {
    return node[name].as<T>();
  } catch (const YAML::TypedBadConversion<T>& ex) {
    throw std::invalid_argument((std::ostringstream() << "Failed to convert " << name).str());
  }
}

static Scenario stringToScenario(const std::string& scenario) {
  if (scenario == "vda5050") {
    return Scenario::VDA5050;
  } else if (scenario == "teleop") {
    return Scenario::TELEOP;
  }
  throw std::invalid_argument("Config does not specify a valid scenario!");
}

static TeleopConfig parseTeleopConfig(const YAML::Node& node) {
  TeleopConfig c{};

  c.start_s = getValue<int>(node, "start_s");
  c.stop_s = getValue<int>(node, "stop_s");
  c.control_command_period_ms = getValue<int>(node, "control_command_period_ms");
  c.request_teleoperation_capabilities_after_ms =
      getValue<int>(node, "request_teleoperation_capabilities_after_ms");
  c.send_teleoperation_capability_after_request_ms =
      getValue<int>(node, "send_teleoperation_capability_after_request_ms");
  c.teleoperation_state_after_toggle_teleop_ms =
      getValue<int>(node, "teleoperation_state_after_toggle_teleop_ms");
  c.state_after_toggle_teleop_ms = getValue<int>(node, "state_after_toggle_teleop_ms");
  c.restart_order_simulation_after_teleoperation_stop_ms =
      getValue<int>(node, "restart_order_simulation_after_teleoperation_stop_ms");
  c.control_stream_start_after_teleop_state_ms =
      getValue<int>(node, "control_stream_start_after_teleop_state_ms");

  return c;
}

static Vda5050Config parseVda5050Config(const YAML::Node& node) {
  Vda5050Config c{};

  c.manufacturer = getValue<std::string>(node, "manufacturer");
  c.serial_number = getValue<std::string>(node, "serial_number");
  c.visualization_period_ms = getValue<int>(node, "visualization_period_ms");
  c.send_connection_after_ms = getValue<int>(node, "send_connection_after_ms");
  c.send_state_after_ms = getValue<int>(node, "send_state_after_ms");
  c.send_factsheet_request_after_ms = getValue<int>(node, "send_factsheet_request_after_ms");
  c.send_order_after_ms = getValue<int>(node, "send_order_after_ms");
  c.start_visualization_after_ms = getValue<int>(node, "start_visualization_after_ms");
  c.factsheet_response_time_ms = getValue<int>(node, "factsheet_response_time_ms");
  c.send_order_continuation_ms = getValue<int>(node, "send_order_continuation_ms");
  c.send_state_after_order_ms = getValue<int>(node, "send_state_after_order_ms");
  c.idle_state_interval_ms = getValue<int>(node, "idle_state_interval_ms");
  c.order_finish_step_ms = getValue<int>(node, "order_finish_step_ms");
  c.next_order_after_finish_ms = getValue<int>(node, "next_order_after_finish_ms");
  c.order_len_min = getValue<int>(node, "order_len_min");
  c.order_len_max = getValue<int>(node, "order_len_max");

  // Currently also always reading the teleop config
  c.teleop_cfg = parseTeleopConfig(node["teleop"]);

  return c;
}

Config parseConfig(const std::string& path) {
  Config c{};
  YAML::Node config = YAML::LoadFile(path);
  c.length_s = getValue<int>(config, "length_s");
  c.seed = getValue<int>(config, "seed");
  c.scenario = stringToScenario(getValue<std::string>(config, "scenario"));

  switch (c.scenario) {
    case Scenario::VDA5050:
      c.vda5050_cfg = parseVda5050Config(config["vda5050"]);
      break;
    case Scenario::TELEOP:
      c.vda5050_cfg = parseVda5050Config(config["vda5050"]);
      break;
  }

  return c;
}
