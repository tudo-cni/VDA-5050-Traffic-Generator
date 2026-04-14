// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#include "mqtt/mqtt_client.h"
#include "utils/config_loader.h"
#include "utils/main_loop.h"

int main(int argc, char* argv[]) {
  const YAML::Node config = loadConfig(argc, argv);
  std::string socket_uri = config["socket_uri"].as<std::string>();
  std::string output_path_received = config["output_path_received"].as<std::string>();
  std::string output_path_throughput = config["output_path_throughput"].as<std::string>();
  std::string mqtt_hostname = config["mqtt"]["hostname"].as<std::string>();

  Client c(mqtt_hostname);

  c.subscribe("uagv/v2/manufacturer/SN/connection", 0);
  c.subscribe("uagv/v2/manufacturer/SN/factsheet", 0);
  c.subscribe("uagv/v2/manufacturer/SN/visualization", 0);
  c.subscribe("uagv/v2/manufacturer/SN/state", 0);
  c.subscribe("uagv/v2/manufacturer/SN/teleop_state", 0);

  mainLoop([&](const std::string& topic, const std::string& msg) { c.publishMessage(topic, msg); },
           [&](const std::string& topic, const std::string& msg) { c.publishMessage(topic, msg); },
           socket_uri, false);

  c.stop();

  writeToCsv(c.getReceivedMessages(), output_path_received);
  writeToCsv(c.getSendEntries(), output_path_throughput);
}
