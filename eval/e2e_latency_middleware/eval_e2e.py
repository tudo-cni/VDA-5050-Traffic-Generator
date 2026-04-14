# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: MIT

import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.axes import Axes
import os
import numpy as np

from matplotlib import rcParams

rcParams["font.family"] = "sans-serif"
rcParams["font.sans-serif"] = ["Arial"]
# set font sizes
rcParams["axes.titlesize"] = 12
rcParams["axes.labelsize"] = 12

rcParams["text.usetex"] = True
rcParams["pgf.rcfonts"] = False

script_dir = os.path.dirname(os.path.abspath(__file__))
output_dir = os.path.abspath(os.path.join(script_dir, "..", "..", "output"))


def p(name):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.abspath(os.path.join(script_dir, "..", "..", "output"))
    return os.path.join(output_dir, name)


def plot(
    scenarios,
    output_name,
    figsize,
    custom_annotations=None,
    ylim=None,
    use_quantile=False,
    skip_initial_samples_teleop=False,
):
    fig, ax = plt.subplots(1, 1, figsize=figsize)

    labels = []
    next_pos = 1
    for idx, scenario in enumerate(scenarios):
        file_base, color, scenario_title = scenario
        script_path = os.path.dirname(os.path.realpath(__file__))
        df = pd.read_csv(f"{file_base}", delimiter=",", comment="#")
        df["Sent"] = pd.to_datetime(df["Sent"])
        df["Received"] = pd.to_datetime(df["Received"])

        # For teleop scenarios, detect runs and skip first 100 samples per run
        if skip_initial_samples_teleop:
            # Calculate time differences between consecutive send timestamps (in milliseconds)
            time_diffs = df["Sent"].diff().dt.total_seconds() * 1000

            # Detect run boundaries (gaps > 500ms indicate a new run)
            run_starts = [0] + (time_diffs[time_diffs > 700].index.tolist())
            num_runs = len(run_starts)
            print(f"{scenario_title}: Detected {num_runs} runs")

            # Create a mask to keep all samples except first 50 of each run
            keep_mask = pd.Series([True] * len(df), index=df.index)
            for run_idx, start_idx in enumerate(run_starts):
                # Determine end of samples to skip (first 50 of this run)
                if run_idx < num_runs - 1:
                    next_start = run_starts[run_idx + 1]
                    skip_end = min(start_idx + 50, next_start)
                else:
                    skip_end = min(start_idx + 50, len(df))

                keep_mask[start_idx:skip_end] = False

            df = df[keep_mask]
            df["Sent"] = pd.to_datetime(df["Sent"])
            df["Received"] = pd.to_datetime(df["Received"])

        lat = (df["Received"] - df["Sent"]).dt.total_seconds()

        lat *= 1000  # to ms

        if use_quantile:
            peaks = lat[lat > np.percentile(lat, 99)]
            lat = lat[lat <= np.percentile(lat, 99)]

        print(f"{scenario[0]} - {lat.mean()}")

        vp = ax.violinplot(lat, showmeans=True, positions=[next_pos])

        if use_quantile:
            ax.scatter(
                [next_pos for x in range(len(peaks))], peaks, marker=".", color="gray"
            )
        next_pos += 1
        for body in vp["bodies"]:
            body.set_facecolor(color)
            body.set_alpha(0.4)  # transparency
        vp["cmeans"].set_color(color)
        vp["cbars"].set_color(color)
        vp["cmins"].set_color(color)
        vp["cmaxes"].set_color(color)
        labels.append(scenario_title)

    # vp = ax.violinplot(latency, showmeans=True)
    # ax.set_title(title)
    ax.set_ylabel("Uplink latency [ms]", fontsize=12)
    ax.set_xticks(list(range(1, len(scenarios) + 1)), labels, fontsize=10)
    ax.tick_params(axis="y", labelsize=10)
    if ylim is None:
        plt.ylim(0)
    else:
        ax.set_ylim(ylim)
    ax.grid(axis="y")

    if custom_annotations is not None:
        custom_annotations(ax, fig)

    plt.tight_layout()
    plt.savefig(f"{script_path}/{output_name}.png")
    plt.savefig(
        f"{script_path}/{output_name}.pdf",
        bbox_inches="tight",
        transparent=True,
        pad_inches=0.01,
    )


# Comparison VDA5050
def vda5050_comparison_annotations(ax: Axes, fig):
    ax.axvline(1.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.axvline(2.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.axvline(3.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.axvline(4.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.axvline(5.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.axvline(6.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.annotate(
        "Almost identical latency across middlewares",  # text
        xy=(4, 20),
        xytext=(4, 50),  # text location (data coords)
        fontsize=11,
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
    ax.set_ylim(0, 60)


plot(
    [
        (
            os.path.join(os.path.dirname(__file__), "scenario_vda", "mqtt.csv"),
            "#179c7d",
            "MQTT TCP",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_vda", "zenoh_tcp_peer.csv"
            ),
            "#005b7f",
            "TCP Peer",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_vda", "zenoh_tcp_client.csv"
            ),
            "#005b7f",
            "TCP Client",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_vda", "zenoh_quic_peer.csv"
            ),
            "#005b7f",
            "QUIC Peer",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_vda", "zenoh_quic_client.csv"
            ),
            "#005b7f",
            "QUIC Client",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_vda", "zenoh_udp_peer.csv"
            ),
            "#005b7f",
            "UDP Peer",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_vda", "zenoh_udp_client.csv"
            ),
            "#005b7f",
            "UDP Client",
        ),
    ],
    "scenario_vda_comparison",
    (6.29, 2.5),
    vda5050_comparison_annotations,
)


# Comparison Teleop
def teleop_comparison_annotations(ax: Axes, fig):
    ax.axvline(1.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.axvline(2.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.axvline(3.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.axvline(4.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.axvline(5.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.axvline(6.5, color="gray", linewidth=1, alpha=0.8, zorder=0)
    ax.set_ylim(0, 60)


plot(
    [
        (
            os.path.join(os.path.dirname(__file__), "scenario_teleop", "mqtt.csv"),
            "#179c7d",
            "MQTT TCP",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_teleop", "zenoh_tcp_peer.csv"
            ),
            "#005b7f",
            "TCP Peer",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_teleop", "zenoh_tcp_client.csv"
            ),
            "#005b7f",
            "TCP Client",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_teleop", "zenoh_quic_peer.csv"
            ),
            "#005b7f",
            "QUIC Peer",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_teleop", "zenoh_quic_client.csv"
            ),
            "#005b7f",
            "QUIC Client",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_teleop", "zenoh_udp_peer.csv"
            ),
            "#005b7f",
            "UDP Peer",
        ),
        (
            os.path.join(
                os.path.dirname(__file__), "scenario_teleop", "zenoh_udp_client.csv"
            ),
            "#005b7f",
            "UDP Client",
        ),
    ],
    "scenario_teleop_comparison",
    (6.29, 2.5),
    teleop_comparison_annotations,
    skip_initial_samples_teleop=True,
)
