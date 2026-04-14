// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0

#pragma once

#include "utils/command.h"

class ControllerInterface {
 public:
  virtual ~ControllerInterface() = default;
  virtual void executeCommandBase(const Command& command) = 0;
  virtual void executeCommandUe(size_t number, const Command& command) = 0;
};
