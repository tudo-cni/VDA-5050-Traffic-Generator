// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#include <memory>

#include "utils/command.h"
#include "utils/config_loader.h"
#include "utils/main_loop.h"
#include "zenoh/zenoh.h"

int main(int argc, char* argv[]) {
  const YAML::Node config = loadConfig(argc, argv);
  std::string socket_uri = config["socket_uri"].as<std::string>();
  std::string output_path_received = config["output_path_received"].as<std::string>();
  std::string output_path_throughput = config["output_path_throughput"].as<std::string>();

  const std::string config_path = config["zenoh"]["config_path"].as<std::string>();
  const bool predeclare_publisher = config["zenoh"]["predeclare_publisher"].as<bool>();
  Zenoh z(config_path);

  if (predeclare_publisher) {
    // Pre-declare publisher to use Zenoh optimization
    z.addPublisher("uagv/v2/manufacturer/SN/instantActions");
    z.addPublisher("uagv/v2/manufacturer/SN/order");
    z.addPublisher("uagv/v2/manufacturer/SN/control");
  }

  z.addSubscriber("uagv/v2/manufacturer/SN/connection");
  z.addSubscriber("uagv/v2/manufacturer/SN/factsheet");
  z.addSubscriber("uagv/v2/manufacturer/SN/visualization");
  z.addSubscriber("uagv/v2/manufacturer/SN/state");

  mainLoop([&](const std::string& topic, const std::string& msg) { z.publish(topic, msg); },
           [&](const std::string& topic, const std::string& msg) { z.publish(topic, msg); },
           socket_uri, true);

  z.stop();

  writeToCsv(z.getReceivedMessages(), output_path_received);
  writeToCsv(z.getSendEntries(), output_path_throughput);
}
