// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#include "controller_interface_zmq.h"

#include <iostream>
ControllerInterfaceZmq::ControllerInterfaceZmq(size_t number_ues) : context_(1) {
  for (size_t i = 0; i < number_ues; i++) {
    auto ue = std::make_shared<zmq::socket_t>(context_, zmq::socket_type::push);
    ue->bind("ipc:///tmp/zmq_ue_" + std::to_string(i));
    ues_.push_back(std::move(ue));
  }
  base_ = std::make_shared<zmq::socket_t>(context_, zmq::socket_type::push);
  base_->bind("ipc:///tmp/zmq_base");
}

void ControllerInterfaceZmq::executeCommandBase(const Command& command) {
  executeCommand(base_, command);
}

void ControllerInterfaceZmq::executeCommandUe(size_t number, const Command& command) {
  executeCommand(ues_.at(number), command);
}

void ControllerInterfaceZmq::executeCommand(const std::shared_ptr<zmq::socket_t> socket,
                                            const Command& command) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, command);
  zmq::message_t msg(buffer.data(), buffer.size());
  socket->send(msg, zmq::send_flags::none);
}
