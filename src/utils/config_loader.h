// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0

#pragma once

#include <yaml-cpp/yaml.h>

#include <iostream>

YAML::Node loadConfig(int argc, char* argv[]) {
  if (argc != 2) {
    throw std::runtime_error("Missing config file path as argument");
  }
  return YAML::LoadFile(argv[1]);
}
