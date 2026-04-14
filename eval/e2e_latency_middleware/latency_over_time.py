# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: MIT

import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.axes import Axes
import os

from matplotlib import rcParams

rcParams["font.family"] = "sans-serif"
rcParams["font.sans-serif"] = ["Arial"]
# font size 14 for axis labels, 12 for ticks
rcParams["axes.labelsize"] = 14
rcParams["xtick.labelsize"] = 12
rcParams["ytick.labelsize"] = 12

rcParams["text.usetex"] = True
rcParams["pgf.rcfonts"] = False


def main(data_file, output_name, annotation_fct=None, ylims=None, figsize=(6.29, 3)):
    df = pd.read_csv(data_file, delimiter=",", comment="#")
    df["Sent"] = pd.to_datetime(df["Sent"])
    df["Received"] = pd.to_datetime(df["Received"])

    df["lat"] = (df["Received"] - df["Sent"]).dt.total_seconds()
    df["lat"] *= 1000
    df["since_start"] = (df["Sent"] - df["Sent"].min()).dt.total_seconds()

    fig, ax = plt.subplots(figsize=figsize)

    if annotation_fct:
        annotation_fct(ax)

    ax.set_ybound(0)
    ax.set_xlim(0, 60)

    if ylims is not None:
        ax.set_ylim(ylims[0], ylims[1])

    df["avg"] = df["lat"]
    ax.plot(df["since_start"], df["avg"], color="#179c7d")
    ax.grid(axis="both")
    # Moving average over 10 entries
    # ax.set_title("E2E Uplink Latency High Frequency 320 Byte@50 Hz\n(MQTT, moving avg over 10 entries)")
    ax.set_xlabel("Scenario time [s]")
    ax.set_ylabel("Uplink latency [ms]")
    ax.set_ybound(0)
    plt.tight_layout()
    plt.savefig(f"{output_name}.png")
    plt.savefig(f"{output_name}.pdf")


# Teleop
def teleop_annotation(ax: Axes):
    ax.axvspan(20, 40, facecolor="#f58220", alpha=0.25, zorder=0)
    ax.annotate(
        "Running teleoperation",  # text
        xy=(22, 30),  # point to point at (data coords)
        xytext=(23, 50),  # text location (data coords)
        fontsize=12,
        ha="left",
        va="center",
        bbox=dict(
            boxstyle="round,pad=0.4",  # box style and padding
            fc="white",  # facecolor
            ec="#f58220",  # edgecolor
            lw=1.0,  # edge linewidth
            alpha=0.95,  # box transparency
        ),
    )
    ax.annotate(
        "Initial peak due to \nbuffering of video stream",  # text
        xy=(21, 40),  # point to point at (data coords)
        xytext=(10.5, 50),  # text location (data coords)
        arrowprops=dict(arrowstyle="->", color="#f58220", lw=1.5),
        fontsize=12,
        ha="center",
        va="center",
        bbox=dict(
            boxstyle="round,pad=0.4",  # box style and padding
            fc="white",  # facecolor
            ec="#179c7d",  # edgecolor
            lw=1.0,  # edge linewidth
            alpha=0.95,  # box transparency
        ),
    )


main(
    (
        os.path.join(
            os.path.dirname(__file__), "latency_over_time", "mqtt_teleop_latency.csv"
        )
    ),
    "latency_over_time_teleop_mqtt",
    teleop_annotation,
)
