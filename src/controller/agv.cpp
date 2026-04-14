// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#include "agv.h"

NS_LOG_COMPONENT_DEFINE("VDA Simulation");

Agv::Agv(const std::shared_ptr<ControllerInterface>& interface, size_t ue_index, const Config& cfg)
    : cfg_(cfg), interface_(std::move(interface)), ue_index_(ue_index), gen_(cfg_.seed) {
  if (cfg_.scenario == Scenario::VDA5050 || cfg_.scenario == Scenario::TELEOP) {
    ns3::Simulator::Schedule(ns3::MilliSeconds(cfg_.vda5050_cfg.send_connection_after_ms),
                             &Agv::sendConnection, this);
    next_state_ = ns3::Simulator::Schedule(ns3::MilliSeconds(cfg_.vda5050_cfg.send_state_after_ms),
                                           &Agv::sendState, this);
    ns3::Simulator::Schedule(ns3::MilliSeconds(cfg_.vda5050_cfg.send_factsheet_request_after_ms),
                             &Agv::sendFactsheetRequest, this);
    next_order_ = ns3::Simulator::Schedule(ns3::MilliSeconds(cfg_.vda5050_cfg.send_order_after_ms),
                                           &Agv::sendOrder, this);
    ns3::Simulator::Schedule(ns3::MilliSeconds(cfg_.vda5050_cfg.start_visualization_after_ms),
                             &Agv::sendVisualization, this);
  }
  if (cfg_.scenario == Scenario::TELEOP) {
    ns3::Simulator::Schedule(
        ns3::MilliSeconds(cfg_.vda5050_cfg.teleop_cfg.request_teleoperation_capabilities_after_ms),
        &Agv::requestTeleoperationCapabilities, this);
    ns3::Simulator::Schedule(ns3::Seconds(cfg_.vda5050_cfg.teleop_cfg.start_s),
                             &Agv::startTeleoperation, this);
    ns3::Simulator::Schedule(ns3::Seconds(cfg_.vda5050_cfg.teleop_cfg.stop_s),
                             &Agv::stopTeleoperation, this);
  }
}

void Agv::sendConnection() {
  NS_LOG_INFO("Send Connection");
  std::string json =
      R"JSON({"headerId":0,"timestamp":"2025-12-31T23:00:00.00Z","version":"2.1.0","manufacturer":"manufacturer","serialNumber":"SN","connectionState":"ONLINE"})JSON" Command
          c{CommandType::PUBLISH_VDA, "uagv/v2/manufacturer/SN/connection", json};
  interface_->executeCommandUe(ue_index_, c);
}

void Agv::sendFactsheetRequest() {
  NS_LOG_INFO("Send Factsheet Request");
  std::string json =
      R"JSON({"headerId":0,"timestamp":"2025-12-31T23:00:00.00Z","version":"2.1.0","manufacturer":"manufacturer","serialNumber":"SN","actions":[{"actionType":"requestFactsheet","actionId":"PLACEHOLDER","blockingType":"NONE"}]})JSON" Command
          c{CommandType::PUBLISH_VDA, "uagv/v2/manufacturer/SN/instantActions", json};
  interface_->executeCommandBase(c);

  ns3::Simulator::Schedule(ns3::MilliSeconds(cfg_.vda5050_cfg.factsheet_response_time_ms),
                           &Agv::sendFactsheet, this);
}

void Agv::sendFactsheet() {
  NS_LOG_INFO("Send Factsheet");
  std::string json =
      R"JSON({"headerId":0,"timestamp":"2025-12-31T23:00:00.00Z","version":"2.1.0","manufacturer":"manufacturer","serialNumber":"SN","typeSpecification":{"seriesName":"","agvKinematic":"","agvClass":"","maxLoadMass":0,"localizationTypes":[],"navigationTypes":[]},"physicalParameters":{"speedMin":0,"speedMax":0,"accelerationMax":0,"decelerationMax":0,"heightMin":0,"heightMax":0,"width":0,"length":0},"protocolLimits":{},"protocolFeatures":{"optionalParameters":[],"agvActions":[]},"agvGeometry":{},"loadSpecification":{},"localizationParameters":{}})JSON" Command
          c{CommandType::PUBLISH_VDA, "uagv/v2/manufacturer/SN/factsheet", json};
  interface_->executeCommandUe(ue_index_, c);
}

void Agv::sendOrder() {
  NS_LOG_INFO("Send Order");
  order_update_id_++;

  if (planned_node_states_.empty() && planned_edge_states_.empty()) {
    // New order
    order_update_id_ = 0;
    std::uniform_int_distribution<> dist_nodes(cfg_.vda5050_cfg.order_len_min,
                                               cfg_.vda5050_cfg.order_len_max);

    int order_length = dist_nodes(gen_);

    for (int i = 0; i < order_length; i++) {
      std::uniform_int_distribution<> dist_node_id(0, 99);
      EdgeState edge_state;
      edge_state.sequenceId = i;
      edge_state.edgeId =
          "edge_" + std::to_string(dist_node_id(gen_)) + "-" + std::to_string(dist_node_id(gen_));
      planned_edge_states_.push_back(edge_state);

      NodeState node_state;
      node_state.sequenceId = i;
      node_state.nodeId = "node_" + std::to_string(dist_node_id(gen_));
      std::uniform_real_distribution<> dist_pos(0.0, 99.9);
      node_state.x = dist_pos(gen_);
      node_state.y = dist_pos(gen_);
      planned_node_states_.push_back(node_state);
    }
  }

  std::uniform_int_distribution<> dist_nodes(1, planned_node_states_.size());
  size_t num = dist_nodes(gen_);

  for (int i = 0; i < num; i++) {
    node_states_.push_back(planned_node_states_.front());
    planned_node_states_.pop_front();

    edge_states_.push_back(planned_edge_states_.front());
    planned_edge_states_.pop_front();
  }

  std::string json =
      R"JSON({"headerId":0,"timestamp":"2025-12-31T23:00:00.00Z","version":"2.1.0","manufacturer":"manufacturer","serialNumber":"SN","orderId":"PLACEHOLDER","orderUpdateId":0,"nodes":[)JSON" for (
          const auto& node_state : node_states_) {
    std::string node = R"JSON({"nodeId":")JSON" + node_state.nodeId + R"JSON(","sequenceId":)JSON" +
                       node_state.sequenceId + R"JSON(,"released":true,"nodePosition":{"x":)JSON" +
                       node_state.nodePosition.x + R"JSON(,"y":)JSON" + node_state.nodePosition.y +
                       R"JSON(,"mapId":""},"actions":[]})JSON";
    json += (node + ",")
  }
  json.pop_back();  // Remove trailing ,

  json += R"JSON(],"edges":[)JSON" for (const auto& edge_state : edge_states_){
      std::string edge =
          R"JSON({"edgeId":")JSON" + edge_state.edgeId + R"JSON(","sequenceId":)JSON" +
          edge_state.sequenceId +
          R"JSON(,"released":true,"startNodeId":"","endNodeId":"","actions":[]})JSON" json +=
      (edge + ",")} json.pop_back();  // Remove trailing ,

  json += R"JSON(]})JSON"

      Command c{CommandType::PUBLISH_VDA, "uagv/v2/manufacturer/SN/order", json};
  interface_->executeCommandBase(c);

  if (!planned_node_states_.empty()) {
    ns3::Simulator::Schedule(ns3::MilliSeconds(cfg_.vda5050_cfg.send_order_continuation_ms),
                             &Agv::sendOrder, this);
  }

  ns3::Simulator::Cancel(next_state_);
  next_state_ = ns3::Simulator::Schedule(
      ns3::MilliSeconds(cfg_.vda5050_cfg.send_state_after_order_ms), &Agv::sendState, this);
}

void Agv::sendState() {
  NS_LOG_INFO("Send State");

  std::string json =
      R"JSON({"headerId":0,"timestamp":"2025-12-31T23:00:00.00Z","version":"2.1.0","manufacturer":"manufacturer","serialNumber":"SN","orderID":"","orderUpdateId":0,"lastNodeId":"PLACEHOLDER","lastNodeSequenceId":0,"nodeStates":[)JSON"

      for (const auto& node : node_states_) {
    std::string nodePosition =
        R"JSON("x":)JSON" + node.x + R"JSON(,"y":)JSON" + node.y + R"JSON(,"mapId":""})JSON";
    json += R"JSON({"sequenceId":)JSON" + node.sequenceId + R"JSON(,"nodeId":)JSON" + node.nodeId +
            R"JSON(,"nodePosition":)JSON" + nodePosition + R"JSON(,"released":true},)JSON"
  }
  json.pop_back();  // Remove trailing ,

  json += R"JSON(],"edgeStates":[)JSON"

          for (const auto& edge : edge_states_){
              json += R"JSON({"sequenceId":)JSON" + edge.sequenceId + R"JSON(,"edgeId":)JSON" +
                      edge.edgeId +
                      R"JSON(,"released":true},)JSON"} json.pop_back();  // Remove trailing ,

  json +=
      R"JSON(],"agvPosition":{"positionInitialized":false,"x":0.0,"y":0.0,"theta":0.0,"mapId":""},"driving":false,"actionStates":[],"batteryState":{"batteryCharge":0,"charging":false},"operatingMode":"AUTOMATIC","errors":[],"safetyState":{"eStop":"NONE","fieldViolation":false}})JSON" Command
          c{CommandType::PUBLISH_VDA, "uagv/v2/manufacturer/SN/state", json};
  interface_->executeCommandUe(ue_index_, c);

  if (node_states_.empty() && edge_states_.empty()) {
    next_state_ = ns3::Simulator::Schedule(
        ns3::MilliSeconds(cfg_.vda5050_cfg.idle_state_interval_ms), &Agv::sendState, this);
  } else {
    if (node_states_.size() == edge_states_.size()) {
      edge_states_.pop_front();
    } else {
      node_states_.pop_front();
    }
    next_state_ = ns3::Simulator::Schedule(ns3::MilliSeconds(cfg_.vda5050_cfg.order_finish_step_ms),
                                           &Agv::sendState, this);
    if (node_states_.empty() && edge_states_.empty() && planned_edge_states_.empty() &&
        planned_node_states_.empty()) {
      NS_LOG_INFO("Schedule start of new order");
      next_order_ = ns3::Simulator::Schedule(
          ns3::MilliSeconds(cfg_.vda5050_cfg.next_order_after_finish_ms), &Agv::sendOrder, this);
    }
  }
}

void Agv::sendVisualization() {
  assert(cfg_.scenario == Scenario::VDA5050 || cfg_.scenario == Scenario::TELEOP);

  NS_LOG_INFO("Send Visualization");

  std::string json =
      R"JSON({"headerId":0,"timestamp":"2025-12-31T23:00:00.00Z","version":"2.1.0","manufacturer":"manufacturer","serialNumber":"SN","agvPosition":{"positionInitialized":true,"x":22.2,"y":22.2,"theta":0,"mapId":""}})JSON" Command
          c{CommandType::PUBLISH_VDA, "uagv/v2/manufacturer/SN/visualization", json};
  interface_->executeCommandUe(ue_index_, c);
  ns3::Simulator::Schedule(ns3::MilliSeconds(cfg_.vda5050_cfg.visualization_period_ms),
                           &Agv::sendVisualization, this);
}

void Agv::startTeleoperation() {
  NS_LOG_INFO("Send Teleoperation Start Action");
  ns3::Simulator::Cancel(next_order_);
  ns3::Simulator::Cancel(next_state_);

  node_states_.clear();
  edge_states_.clear();
  planned_node_states_.clear();
  planned_edge_states_.clear();

  std::string json =
      R"JSON({"headerId":0,"timestamp":"2025-12-31T23:00:00.00Z","version":"2.1.0","manufacturer":"manufacturer","serialNumber":"SN","actions":[{"actionType":"startTeleoperation","actionId":"PLACEHOLDER","blockingType":"NONE"}]})JSON" Command
          c{CommandType::PUBLISH_VDA, "uagv/v2/manufacturer/SN/instantActions", json};
  interface_->executeCommandBase(c);

  teleoperating_ = true;
  ns3::Simulator::Schedule(
      ns3::MilliSeconds(cfg_.vda5050_cfg.teleop_cfg.teleoperation_state_after_toggle_teleop_ms),
      &Agv::sendTeleoperationState, this);

  // Send state as operation mode changed
  next_state_ = ns3::Simulator::Schedule(
      ns3::MilliSeconds(cfg_.vda5050_cfg.teleop_cfg.state_after_toggle_teleop_ms), &Agv::sendState,
      this);
}

void Agv::stopTeleoperation() {
  NS_LOG_INFO("Send Teleoperation Stop Action");
  ns3::Simulator::Cancel(next_state_);
  ns3::Simulator::Cancel(next_control_);

  std::string json =
      R"JSON({"headerId":0,"timestamp":"2025-12-31T23:00:00.00Z","version":"2.1.0","manufacturer":"manufacturer","serialNumber":"SN","actions":[{"actionType":"stopTeleoperation","actionId":"PLACEHOLDER","blockingType":"NONE"}]})JSON";
  Command c{CommandType::PUBLISH_VDA, "uagv/v2/manufacturer/SN/instantActions", json};
  interface_->executeCommandBase(c);

  teleoperating_ = false;
  ns3::Simulator::Schedule(
      ns3::MilliSeconds(cfg_.vda5050_cfg.teleop_cfg.teleoperation_state_after_toggle_teleop_ms),
      &Agv::sendTeleoperationState, this);

  // Send state as operation mode changed
  next_state_ = ns3::Simulator::Schedule(
      ns3::MilliSeconds(cfg_.vda5050_cfg.teleop_cfg.state_after_toggle_teleop_ms), &Agv::sendState,
      this);

  // Restart order simulation
  next_order_ = ns3::Simulator::Schedule(
      ns3::MilliSeconds(
          cfg_.vda5050_cfg.teleop_cfg.restart_order_simulation_after_teleoperation_stop_ms),
      &Agv::sendOrder, this);
}

void Agv::sendTeleoperationState() {
  {
    std::stringstream ss;
    ss << "{\"teleoperating\": " << std::boolalpha << teleoperating_ << "}";
    Command c{CommandType::PUBLISH_TELEOP, "uagv/v2/manufacturer/SN/teleop_state", ss.str()};
    interface_->executeCommandUe(ue_index_, c);
  }

  // Start/Stop Video stream
  {
    CommandType type = teleoperating_ ? CommandType::START_VIDEO : CommandType::STOP_VIDEO;
    Command c{type};
    interface_->executeCommandUe(ue_index_, c);
  }

  if (teleoperating_) {
    // Start control downstream
    next_control_ = ns3::Simulator::Schedule(
        ns3::MilliSeconds(cfg_.vda5050_cfg.teleop_cfg.control_stream_start_after_teleop_state_ms),
        &Agv::sendControl, this);
  }
}

void Agv::sendControl() {
  NS_LOG_INFO("Send Control");
  std::uniform_real_distribution<> dist_speed(0.0, 10.0);
  std::stringstream ss;
  ss << dist_speed(gen_) << ";" << dist_speed(gen_);
  // Here we use command type teleop. But as this is executed on the base, using
  // PUBLISH_VDA would not make a difference
  Command c{CommandType::PUBLISH_TELEOP, "uagv/v2/manufacturer/SN/control", ss.str()};
  interface_->executeCommandBase(c);
  next_control_ = ns3::Simulator::Schedule(
      ns3::MilliSeconds(cfg_.vda5050_cfg.teleop_cfg.control_command_period_ms), &Agv::sendControl,
      this);
}

void Agv::requestTeleoperationCapabilities() {
  NS_LOG_INFO("Send Teleoperation Capability Request Action");
  std::string json =
      R"JSON({"headerId":0,"timestamp":"2025-12-31T23:00:00.00Z","version":"2.1.0","manufacturer":"manufacturer","serialNumber":"SN","actions":[{"actionType":"requestTeleoperationCapability","actionId":"PLACEHOLDER","blockingType":"NONE"}]})JSON" Command
          c{CommandType::PUBLISH_VDA, "uagv/v2/manufacturer/SN/instantActions", json};
  interface_->executeCommandBase(c);

  ns3::Simulator::Schedule(
      ns3::MilliSeconds(cfg_.vda5050_cfg.teleop_cfg.send_teleoperation_capability_after_request_ms),
      &Agv::sendTeleoperationCapabilities, this);
}

void Agv::sendTeleoperationCapabilities() {
  std::string teleop_capabilities =
      "{\"control_protocol\": \"VDA5050_MQTT\","
      "sensors\": ["
      "{"
      "\"id\": 1,"
      "\"type\": \"camera\","
      "\"url\": \"https://127.0.0.1:8889/test\","
      "\"gstreamer_pipeline\": \"gst-launch-1.0 v4l2src device=/dev/video4 ! "
      "nvvidconv ! nvv4l2h264enc ! rtspclientsink protocols=udp "
      "location=rtsp://10.65.80.156:8554/test\""
      "}]}";
  Command c{CommandType::PUBLISH_TELEOP, "uagv/v2/manufacturer/SN/teleoperationCapabilities",
            teleop_capabilities};
  interface_->executeCommandUe(ue_index_, c);
}
