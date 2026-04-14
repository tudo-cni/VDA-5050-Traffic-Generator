// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "mqtt/async_client.h"
#include "utils/command.h"

class Client : public mqtt::async_client, public mqtt::callback {
 public:
  Client(const std::string& ip);

  void message_arrived(mqtt::const_message_ptr msg) override;

  const std::vector<Entry>& getReceivedMessages() const;
  const std::vector<SendEntry>& getSendEntries() const;

  void stop();

  void publishMessage(const std::string& topic, const std::string& payload);

 private:
  std::vector<Entry> received_msgs_;
  std::vector<SendEntry> send_entries_;
};
