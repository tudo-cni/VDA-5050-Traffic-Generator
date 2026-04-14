<!--
    SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
    SPDX-License-Identifier: MIT
-->
sudo ip netns add ue1
sudo ip ro add 10.45.0.0/16 via 10.53.1.2
sudo ip link set mtu 1400 dev br-4a8ee3f83e4b

## After starting UE

sudo ip netns exec ue1 ip link set mtu 1400 tun_srsue
sudo ip netns exec ue1 ip route add default via 10.45.1.1 dev tun_srsue

## Run simulation

WARNING: Execute the UE application in the ue netns!

```bash
sudo ip netns exec ue1 ./build/src/ue/UeMqtt src/ue/ue.yaml
```

or

```bash
sudo ip netns exec ue1 ./build/src/ue/UeZenoh src/ue/ue.yaml
```

## Netem Delay

**On host**
sudo tc qdisc add dev 5gc_bridge root netem delay 30ms

**In Zenoh Container:**
```
apk update
apk add iproute2
tc qdisc add dev eth0 root netem delay 30ms
```
