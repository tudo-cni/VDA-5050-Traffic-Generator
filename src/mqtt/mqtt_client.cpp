// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#include "mqtt_client.h"

Client::Client(const std::string& ip) : mqtt::async_client(ip, "") {
  this->connect()->wait();
  this->set_callback(*this);
}

void Client::message_arrived(mqtt::const_message_ptr msg) {
  auto now = std::chrono::high_resolution_clock::now();
#ifdef EXCLUDE_TIMESTAMP_IN_TELEOP_STATE
  if (msg->get_topic() != "uagv/v2/manufacturer/SN/teleop_state") {
#endif
    Entry e{msg->get_payload_str().substr(0, 26), now};
    received_msgs_.push_back(std::move(e));
#ifdef EXCLUDE_TIMESTAMP_IN_TELEOP_STATE
  }
#endif
}

const std::vector<Entry>& Client::getReceivedMessages() const { return received_msgs_; }

const std::vector<SendEntry>& Client::getSendEntries() const { return send_entries_; }

void Client::stop() { this->disconnect()->wait(); }

void Client::publishMessage(const std::string& topic, const std::string& payload) {
  send_entries_.push_back(
      {topic.size() + payload.size(), std::chrono::high_resolution_clock::now()});
  this->publish(topic, payload, topic.ends_with("connection") ? 1 : 0, false);
}
