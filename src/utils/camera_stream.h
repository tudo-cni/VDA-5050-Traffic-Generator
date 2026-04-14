// SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
// SPDX-License-Identifier: GPL-2.0

#pragma once

#include <gst/gst.h>

#include <iostream>

class CameraStream {
 public:
  CameraStream() {
    gst_init(nullptr, nullptr);
    GstElement *src, *encoder, *sink;

    GError* error = nullptr;

    pipeline = gst_parse_launch(
        "gst-launch-1.0 videotestsrc is-live=true ! "
        "video/x-raw,format=NV12,field=none,framerate=60/"
        "1,width=1280,height=800 ! nvh264enc zerolatency=true bitrate=2000 ! "
        "queue leaky=downstream max-size-time=20000000 ! rtspclientsink "
        "rtp-blocksize=1300 location=rtsp://10.53.1.4:8554/test",
        &error);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
  }

  ~CameraStream() {
    // Only send EOS if pipeline could get into playing state
    GstState current_state;
    GstState pending_state;
    GstClockTime timeout = GST_CLOCK_TIME_NONE;
    GstStateChangeReturn ret =
        gst_element_get_state(pipeline, &current_state, &pending_state, timeout);
    if (ret == GST_STATE_CHANGE_SUCCESS) {
      GstEvent* eos_event = gst_event_new_eos();
      gst_element_send_event(pipeline, eos_event);
    }

    /* Wait until error or EOS */
    GstBus* bus = gst_element_get_bus(pipeline);
    GstMessage* msg = gst_bus_timed_pop_filtered(
        bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* See next tutorial for proper error message handling/parsing */
    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
      std::cerr << "ERROR" << std::endl;
    }

    /* Free resources */
    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
  }

 private:
  GstElement* pipeline;
};
