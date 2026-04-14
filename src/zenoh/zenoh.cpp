// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0

#include "zenoh.h"

Zenoh::Zenoh(const std::string& config_path) {
  zc_init_log_from_env_or("info");
  zenoh::Config cfg = zenoh::Config::from_file(config_path);
  zenoh_ = std::make_unique<zenoh::Session>(zenoh::Session::open(std::move(cfg)));
}

void Zenoh::addPublisher(const std::string& topic) {
  auto pub = zenoh_->declare_publisher(topic);
  publisher_.emplace(topic, std::move(pub));
}

void Zenoh::addSubscriber(const std::string& topic) {
  auto sub = zenoh_->declare_subscriber(
      topic, [this](const zenoh::Sample& sample) { addEntry(sample); }, zenoh::closures::none);
  subscriber_.push_back(std::move(sub));
}

void Zenoh::publish(const std::string& topic, const std::string& payload) {
  send_entries_.push_back(
      {topic.size() + payload.size(), std::chrono::high_resolution_clock::now()});
  if (!publisher_.empty()) {
    publisher_.at(topic).put(payload);
  } else {
    zenoh_->put(topic, payload);
  }
}

void Zenoh::stop() { zenoh_.reset(); }

const std::vector<Entry>& Zenoh::getReceivedMessages() const { return received_msgs_; }

const std::vector<SendEntry>& Zenoh::getSendEntries() const { return send_entries_; }
