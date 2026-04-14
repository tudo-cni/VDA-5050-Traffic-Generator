// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0-only

#include "main_loop.h"

#include <memory>
#include <zmq.hpp>

#include "utils/camera_stream.h"
#include "utils/command.h"

void mainLoop(std::function<void(const std::string&, const std::string&)> send_vda,
              std::function<void(const std::string&, const std::string&)> send_teleop,
              const std::string& socket_uri, bool use_camera) {
  zmq::context_t context(1);
  std::shared_ptr<zmq::socket_t> pub =
      std::make_shared<zmq::socket_t>(context, zmq::socket_type::pull);

  pub->connect(socket_uri);

  std::unique_ptr<CameraStream> camera;

  while (true) {
    zmq::message_t message;
    pub->recv(message, zmq::recv_flags::none);

    auto str = msgpack::unpack(static_cast<const char*>(message.data()), message.size());

    Command c;
    str.get().convert(c);

    if (c.type == CommandType::PUBLISH_VDA) {
      std::string timestamp = getTimestamp(std::chrono::high_resolution_clock::now());
      c.msg = c.msg.replace(0, timestamp.length(), timestamp);
      send_vda(c.topic, c.msg);
    } else if (c.type == CommandType::PUBLISH_TELEOP) {
#ifdef EXCLUDE_TIMESTAMP_IN_TELEOP_STATE
      if (c.topic != "uagv/v2/manufacturer/SN/teleop_state") {
#endif
        std::string timestamp = getTimestamp(std::chrono::high_resolution_clock::now());
        c.msg = c.msg.replace(0, timestamp.length(), timestamp);
#ifdef EXCLUDE_TIMESTAMP_IN_TELEOP_STATE
      }
#endif
      send_teleop(c.topic, c.msg);
    } else if (use_camera && c.type == CommandType::START_VIDEO) {
      camera = std::make_unique<CameraStream>();
    } else if (use_camera && c.type == CommandType::STOP_VIDEO) {
      camera.reset();
    } else if (c.type == CommandType::EXIT) {
      break;
    } else {
      throw std::runtime_error("Invalid command");
    }
  }
}
