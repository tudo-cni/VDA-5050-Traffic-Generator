// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0

#pragma once

#include "utils/command.h"
#include "zenoh.hxx"

class Zenoh {
 public:
  explicit Zenoh(const std::string& config_path);

  void addSubscriber(const std::string& topic);
  void addPublisher(const std::string& topic);

  void publish(const std::string& topic, const std::string& payload);

  void stop();

  const std::vector<Entry>& getReceivedMessages() const;
  const std::vector<SendEntry>& getSendEntries() const;

 private:
  inline void addEntry(const zenoh::Sample& sample) {
#ifdef EXCLUDE_TIMESTAMP_IN_TELEOP_STATE
    if (sample.get_keyexpr().as_string_view() != "uagv/v2/manufacturer/SN/teleop_state") {
#endif
      auto now = std::chrono::high_resolution_clock::now();
      Entry e{sample.get_payload().as_string().substr(0, 26), now};
      received_msgs_.push_back(std::move(e));
#ifdef EXCLUDE_TIMESTAMP_IN_TELEOP_STATE
    }
#endif
  }

  std::vector<Entry> received_msgs_;
  std::vector<SendEntry> send_entries_;
  std::vector<zenoh::Subscriber<void>> subscriber_;
  std::unordered_map<std::string, zenoh::Publisher> publisher_;
  std::unique_ptr<zenoh::Session> zenoh_;
};
