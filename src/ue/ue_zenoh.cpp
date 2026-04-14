// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0

#include "utils/config_loader.h"
#include "utils/main_loop.h"
#include "zenoh/zenoh.h"

int main(int argc, char* argv[]) {
  const YAML::Node config = loadConfig(argc, argv);
  std::string socket_uri = config["socket_uri"].as<std::string>();
  std::string output_path_received = config["output_path_received"].as<std::string>();
  std::string output_path_throughput = config["output_path_throughput"].as<std::string>();

  const std::string config_path_vda = config["zenoh"]["config_path_vda"].as<std::string>();
  const std::string config_path_teleop = config["zenoh"]["config_path_teleop"].as<std::string>();
  const bool predeclare_publisher = config["zenoh"]["predeclare_publisher"].as<bool>();
  Zenoh client_vda(config_path_vda);
  std::optional<Zenoh> client_teleop;

  client_teleop.emplace(config_path_teleop);

  if (predeclare_publisher) {
    // Pre-declare publisher to use Zenoh optimization
    client_vda.addPublisher("uagv/v2/manufacturer/SN/connection");
    client_vda.addPublisher("uagv/v2/manufacturer/SN/state");
    client_vda.addPublisher("uagv/v2/manufacturer/SN/factsheet");
    client_vda.addPublisher("uagv/v2/manufacturer/SN/visualization");

    client_teleop.value().addPublisher("uagv/v2/manufacturer/SN/teleop_capabilities");
    client_teleop.value().addPublisher("uagv/v2/manufacturer/SN/teleop_state");
  }

  client_vda.addSubscriber("uagv/v2/manufacturer/SN/instantActions");
  client_vda.addSubscriber("uagv/v2/manufacturer/SN/order");
  client_teleop.value().addSubscriber("uagv/v2/manufacturer/SN/control");

  mainLoop(
      [&](const std::string& topic, const std::string& msg) { client_vda.publish(topic, msg); },
      [&](const std::string& topic, const std::string& msg) {
        if (!client_teleop.has_value()) return;
        client_teleop.value().publish(topic, msg);
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
