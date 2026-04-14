// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0

#pragma once

#include <zmq.hpp>

#include "controller_interface.h"

class ControllerInterfaceZmq : public ControllerInterface {
 public:
  explicit ControllerInterfaceZmq(size_t number_ues);

  void executeCommandBase(const Command& command) override;
  void executeCommandUe(size_t number, const Command& command) override;

 private:
  void executeCommand(const std::shared_ptr<zmq::socket_t> socket, const Command& command);

  zmq::context_t context_;
  std::vector<std::shared_ptr<zmq::socket_t>> ues_;
  std::shared_ptr<zmq::socket_t> base_;
};
