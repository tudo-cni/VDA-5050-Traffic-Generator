// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0

#pragma once

#include <functional>
#include <string>

void mainLoop(std::function<void(const std::string&, const std::string&)> send_vda,
              std::function<void(const std::string&, const std::string&)> send_teleop,
              const std::string& socket_uri, bool use_camera);
