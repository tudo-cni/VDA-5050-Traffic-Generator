# SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
# SPDX-License-Identifier: MIT

import matplotlib.pyplot as plt
import pandas as pd
import os
from matplotlib.patches import Patch
from matplotlib import rcParams


# Reorder base_dirs
base_dirs = [
    (os.path.join(os.path.dirname(__file__), "scenarios", "mqtt"), "MQTT\nTCP"),
    (
        os.path.join(
            os.path.dirname(__file__), "scenarios", "zenoh_tcp_peer_not_predeclared"
        ),
        "Zenoh\nTCP\nPeer\n(not declared)",
    ),
    (
        os.path.join(os.path.dirname(__file__), "scenarios", "zenoh_tcp_peer"),
        "Zenoh\nTCP\nPeer",
    ),
    (
        os.path.join(os.path.dirname(__file__), "scenarios", "zenoh_tcp_client"),
        "Zenoh\nTCP\nClient",
    ),
    (
        os.path.join(os.path.dirname(__file__), "scenarios", "zenoh_quic_peer"),
        "Zenoh\nQUIC\nPeer",
    ),
    (
        os.path.join(os.path.dirname(__file__), "scenarios", "zenoh_quic_client"),
        "Zenoh\nQUIC\nClient",
    ),
    (
        os.path.join(os.path.dirname(__file__), "scenarios", "zenoh_udp_peer"),
        "Zenoh\nUDP\nPeer",
    ),
    (
        os.path.join(os.path.dirname(__file__), "scenarios", "zenoh_udp_client"),
        "Zenoh\nUDP\nClient",
    ),
]

WITH_RLC_OVERHEAD = True

goodput = []
throughput = []
rlc_ack_overhead = []

rcParams["font.family"] = "sans-serif"
rcParams["font.sans-serif"] = ["Arial"]
print(rcParams["font.sans-serif"])
# font size 14 for axis labels, 12 for ticks
rcParams["axes.labelsize"] = 14
rcParams["xtick.labelsize"] = 12
rcParams["ytick.labelsize"] = 12
rcParams["text.usetex"] = True


for idx, entry in enumerate(base_dirs):
    base_dir, title = entry

    df_throughput = pd.read_csv(
        f"{base_dir}/rlc.csv",
        names=["Time", "Direction", "Control", "Length"],
        delimiter=",",
    )
    df_throughput["Time"] = pd.to_datetime(df_throughput["Time"], unit="s")

    df_throughput.set_index("Time", inplace=True)

    df = pd.read_csv(
        f"{base_dir}/base_send.csv", names=["Timestamp", "Payload"], delimiter=","
    )

    df2 = pd.read_csv(
        f"{base_dir}/ue_send.csv", names=["Timestamp", "Payload"], delimiter=","
    )

    df = pd.concat([df, df2], ignore_index=True)

    df["Timestamp"] = pd.to_datetime(df["Timestamp"])
    df = df.sort_values(by="Timestamp")
    start_time = df["Timestamp"].min()
    df_throughput = df_throughput[df_throughput.index >= start_time]

    df.set_index("Timestamp", inplace=True)

    goodput_size = df["Payload"].sum()
    throughput_size = df_throughput["Length"].sum()
    rlc_overhead = df_throughput[df_throughput["Control"].notna()]["Length"].sum()
    goodput.append(goodput_size)
    throughput.append(throughput_size)
    rlc_ack_overhead.append(rlc_overhead)


entries = [title for _, title in base_dirs]

fig, ax = plt.subplots(figsize=(6.29, 4))
if WITH_RLC_OVERHEAD:
    ax.bar(
        entries,
        [good / through * 100 for good, through in zip(goodput, throughput)],
        label="Goodput",
        color="#179c7dcf",
    )
    ax.bar(
        entries,
        [
            (through - good - rlc) / through * 100
            for good, rlc, through in zip(goodput, rlc_ack_overhead, throughput)
        ],
        label="Overhead on RLC layer (excl. RLC ACK)",
        bottom=[good / through * 100 for good, through in zip(goodput, throughput)],
        color="#fdb913b0",
    )
    ax.bar(
        entries,
        [rlc / through * 100 for rlc, through in zip(rlc_ack_overhead, throughput)],
        label="RLC ACK overhead",
        bottom=[
            good / through * 100 + (through - good - rlc) / through * 100
            for good, through, rlc in zip(goodput, throughput, rlc_ack_overhead)
        ],
        color="#bb0056b0",
    )
else:
    ax.bar(
        entries, [100 for _ in enumerate(entries)], label="Overhead", color="#ff7f0e"
    )
    ax.bar(
        entries,
        [good / through * 100 for good, through in zip(goodput, throughput)],
        label="Goodput",
        color="#1f77b4",
    )
ax.set_ylabel("Traffic on RLC layer [\%]")
# ax.legend()
ax.grid(axis="y")
ax.set_xticks(range(len(entries)))
# ax.set_xticklabels(entries, rotation=45)
ax.tick_params(axis="x", direction="in")

ax.set_ybound(0, 100)
plt.tight_layout()

plt.savefig("throughput_bars_relative.png")
plt.savefig(
    "throughput_bars_relative.pdf",
    transparent=True,
    bbox_inches="tight",
    pad_inches=0.01,
)


# set legend font size to 12
plt.rcParams["legend.fontsize"] = 16

handles = []
handles.append(Patch(facecolor="#179c7dcf", label="Goodput"))
handles.append(
    Patch(facecolor="#fdb913cf", label="Overhead on RLC layer (excl. RLC ACK)")
)
handles.append(Patch(facecolor="#bb0056cf", label="RLC ACK overhead"))

fig = plt.figure()
fig.legend(handles=handles, loc="center", ncols=4)
fig.tight_layout()
plt.savefig("throughput_bars_relative_legend.png", bbox_inches="tight")
plt.savefig("throughput_bars_relative_legend.pdf", bbox_inches="tight", pad_inches=0.0)
