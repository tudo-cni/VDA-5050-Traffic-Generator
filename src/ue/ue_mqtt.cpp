// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#include <memory>
#include <optional>

#include "mqtt/mqtt_client.h"
#include "utils/config_loader.h"
#include "utils/main_loop.h"

int main(int argc, char* argv[]) {
  const YAML::Node config = loadConfig(argc, argv);
  std::string socket_uri = config["socket_uri"].as<std::string>();
  std::string output_path_received = config["output_path_received"].as<std::string>();
  std::string output_path_throughput = config["output_path_throughput"].as<std::string>();

  std::string mqtt_hostname = config["mqtt"]["hostname"].as<std::string>();
  Client client_vda(mqtt_hostname);

  std::optional<Client> client_teleop;

  client_teleop.emplace(mqtt_hostname);
  client_vda.subscribe("uagv/v2/manufacturer/SN/instantActions", 0);
  client_vda.subscribe("uagv/v2/manufacturer/SN/order", 0);
  client_teleop.value().subscribe("uagv/v2/manufacturer/SN/control", 0);

  mainLoop([&](const std::string& topic,
               const std::string& msg) { client_vda.publishMessage(topic, msg); },
           [&](const std::string& topic, const std::string& msg) {
             if (!client_teleop.has_value()) return;
             client_teleop.value().publishMessage(topic, msg);
           },
           socket_uri, true);

  client_vda.stop();
  if (client_teleop.has_value()) {
    client_teleop.value().stop();
    writeToCsv(client_vda.getReceivedMessages(), client_teleop.value().getReceivedMessages(),
               output_path_received);
    writeToCsv(client_vda.getSendEntries(), client_teleop.value().getSendEntries(),
               output_path_throughput);
  } else {
    writeToCsv(client_vda.getReceivedMessages(), output_path_received);
    writeToCsv(client_vda.getSendEntries(), output_path_throughput);
  }
}
