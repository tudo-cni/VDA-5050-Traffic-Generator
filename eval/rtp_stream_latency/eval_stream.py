# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: MIT

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

from matplotlib import rcParams

rcParams["font.family"] = "sans-serif"
rcParams["font.sans-serif"] = ["Arial"]
# font size 12 for axis labels, 10 for ticks
rcParams["axes.labelsize"] = 14
rcParams["xtick.labelsize"] = 12
rcParams["ytick.labelsize"] = 12

rcParams["text.usetex"] = True
rcParams["pgf.rcfonts"] = False


def p(name):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.abspath(os.path.join(script_dir, "..", "..", "output"))
    return os.path.join(output_dir, name)


def plot_dual(data_file1, label1, data_file2, label2, output_name):
    df1 = pd.read_csv(data_file1)
    df2 = pd.read_csv(data_file2)
    df1 = df1[375:]
    df2 = df2[375:]
    df1["timestamp_core"] = pd.to_datetime(df1["timestamp_core"])
    df1["timestamp_ue"] = pd.to_datetime(df1["timestamp_ue"])
    df1["latency"] = (df1["timestamp_core"] - df1["timestamp_ue"]) / np.timedelta64(
        1, "ms"
    )
    df2["timestamp_core"] = pd.to_datetime(df2["timestamp_core"])
    df2["timestamp_ue"] = pd.to_datetime(df2["timestamp_ue"])
    df2["latency"] = (df2["timestamp_core"] - df2["timestamp_ue"]) / np.timedelta64(
        1, "ms"
    )

    # cut to 99.99 percentile
    p1 = np.percentile(df1["latency"], 99.99)
    p2 = np.percentile(df2["latency"], 99.99)
    df1 = df1[df1["latency"] <= p1]
    df2 = df2[df2["latency"] <= p2]
    fig, ax = plt.subplots(figsize=(6.29, 2.5))
    ax.grid("y")
    ax.set_ylabel("Uplink latency [ms]")
    ax.set_xticks([])
    ax.set_ylim(0, 50)

    vp1 = ax.violinplot(
        df1["latency"], showmeans=True, positions=[1], bw_method="silverman"
    )
    vp2 = ax.violinplot(df2["latency"], showmeans=True, positions=[2], bw_method=0.17)
    for vp, color in zip([vp1, vp2], ["#008598", "#6b9e42"]):
        vp["bodies"][0].set_facecolor(color)
        vp["bodies"][0].set_alpha(0.4)
        vp["cmeans"].set_color(color)
        vp["cbars"].set_color(color)
        vp["cmins"].set_color(color)
        vp["cmaxes"].set_color(color)
    ax.set_xticks([1, 2], [label1, label2])
    plt.tight_layout()
    plt.savefig(f"{output_name}.png")
    plt.savefig(
        f"{output_name}.pdf", bbox_inches="tight", pad_inches=0.05, transparent=True
    )


if __name__ == "__main__":
    plot_dual(
        os.path.join(os.path.dirname(__file__), "virtual_MQTT_Teleop.csv"),
        "Virtual-Radio Open RAN",
        os.path.join(os.path.dirname(__file__), "testbed_MQTT_Teleop.csv"),
        "Over-the-Air Open RAN",
        "teleop_comparison",
    )
