// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#include "command.h"

std::string getTimestamp(std::chrono::high_resolution_clock::time_point t) {
  auto now = std::chrono::time_point_cast<std::chrono::microseconds>(t);
  auto duration = now.time_since_epoch();

  auto secs = std::chrono::duration_cast<std::chrono::seconds>(duration);
  auto micros = std::chrono::duration_cast<std::chrono::microseconds>(duration - secs);

  std::time_t time_now = std::chrono::high_resolution_clock::to_time_t(
      std::chrono::high_resolution_clock::time_point(secs));
  std::tm tm_now = *std::gmtime(&time_now);  // or std::localtime for local time
  std::ostringstream oss;

  oss << std::put_time(&tm_now, "%Y-%m-%dT%H:%M:%S");
  oss << '.' << std::setfill('0') << std::setw(6) << micros.count();
  return oss.str();
}

void writeToCsv(const std::vector<Entry>& received_msgs, const std::string& filename) {
  std::ofstream out(filename);
  for (const auto& received : received_msgs) {
    out << received.send << "," << getTimestamp(received.receive) << std::endl;
  }
  out.close();
}

void writeToCsv(const std::vector<Entry>& first, const std::vector<Entry>& second,
                const std::string& filename) {
  std::vector<Entry> received_msgs;
  received_msgs.insert(received_msgs.end(), first.begin(), first.end());
  received_msgs.insert(received_msgs.end(), second.begin(), second.end());
  writeToCsv(received_msgs, filename);
}

void writeToCsv(const std::vector<SendEntry>& send_entries, const std::string& filename) {
  std::ofstream out(filename);
  for (const auto& send : send_entries) {
    out << getTimestamp(send.tp) << "," << send.size << std::endl;
  }
  out.close();
}

void writeToCsv(const std::vector<SendEntry>& first, const std::vector<SendEntry>& second,
                const std::string& filename) {
  std::vector<SendEntry> send_entries;
  send_entries.insert(send_entries.end(), first.begin(), first.end());
  send_entries.insert(send_entries.end(), second.begin(), second.end());
  writeToCsv(send_entries, filename);
}
