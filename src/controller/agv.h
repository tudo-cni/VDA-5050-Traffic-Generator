// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0

#pragma once

#include <memory>
#include <random>

#include "config.hpp"
#include "controller_interface.h"
#include "ns3/core-module.h"

struct NodeState {
  uint32_t sequenceId{};
  std::string nodeId;
  double x;
  double y;
};

struct EdgeState {
  uint32_t sequenceId{};
  std::string edgeId;
};

class Agv {
 public:
  Agv(const std::shared_ptr<ControllerInterface>& interface, size_t ue_index, const Config& cfg);

  void sendConnection();

  void sendFactsheetRequest();

  void sendFactsheet();

  void sendOrder();

  void sendState();

  void sendVisualization();

  void startTeleoperation();

  void stopTeleoperation();

  void sendTeleoperationState();

  void sendControl();

  void requestTeleoperationCapabilities();

  void sendTeleoperationCapabilities();

 private:
  Config cfg_;

  ns3::EventId next_state_;
  ns3::EventId next_order_;
  ns3::EventId next_control_;

  std::deque<NodeState> planned_node_states_;
  std::deque<EdgeState> planned_edge_states_;

  std::deque<NodeState> node_states_;
  std::deque<EdgeState> edge_states_;

  std::shared_ptr<ControllerInterface> interface_;
  size_t ue_index_;

  std::mt19937 gen_;
  uint32_t order_update_id_ = 0;
  bool teleoperating_ = false;
};
