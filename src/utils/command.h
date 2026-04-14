// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include <chrono>
#include <fstream>
#include <msgpack.hpp>

enum class CommandType { PUBLISH_VDA, PUBLISH_TELEOP, START_VIDEO, STOP_VIDEO, EXIT };
MSGPACK_ADD_ENUM(CommandType);

struct Command {
  CommandType type;
  std::string topic;
  std::string msg;

  MSGPACK_DEFINE(type, topic, msg);
};

std::string getTimestamp(std::chrono::high_resolution_clock::time_point t);

struct Entry {
  std::string send;
  std::chrono::high_resolution_clock::time_point receive;
};

struct SendEntry {
  size_t size;
  std::chrono::high_resolution_clock::time_point tp;
};

void writeToCsv(const std::vector<SendEntry>& first, const std::vector<SendEntry>& second,
                const std::string& filename);
void writeToCsv(const std::vector<SendEntry>& received_msgs, const std::string& filename);
void writeToCsv(const std::vector<Entry>& received_msgs, const std::string& filename);
void writeToCsv(const std::vector<Entry>& first, const std::vector<Entry>& second,
                const std::string& filename);
