// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#include <chrono>
#include <random>
#include <thread>
#include <variant>
#include <vector>
#include <zmq.hpp>

#include "agv.h"
#include "config.hpp"
#include "controller_interface.h"
#include "controller_interface_zmq.h"
#include "ns3/core-module.h"
#include "utils/command.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: Pass path to config YAML as first argument" << std::endl;
    return 1;
  }

  Config cfg{};
  try {
    cfg = parseConfig(argv[1]);
  } catch (const std::invalid_argument& ex) {
    std::cerr << ex.what() << std::endl;
    return 1;
  }

  ns3::GlobalValue::Bind("SimulatorImplementationType",
                         ns3::StringValue("ns3::RealtimeSimulatorImpl"));
  ns3::ShowProgress progress(ns3::Seconds(1), std::cerr);

  auto interface = std::make_shared<ControllerInterfaceZmq>(1);
  Agv agv(interface, 0, cfg);
  ns3::Simulator::Stop(ns3::Seconds(cfg.length_s));
  ns3::Simulator::Run();

  {
    Command c{CommandType::EXIT, "", ""};
    interface->executeCommandUe(0, c);
  }
  {
    Command c{CommandType::EXIT, "Test", "Test"};
    interface->executeCommandBase(c);
  }

  std::this_thread::sleep_for(std::chrono::seconds(2));

  ns3::Simulator::Destroy();
}
