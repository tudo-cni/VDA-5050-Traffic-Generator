<!--
    SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
    SPDX-License-Identifier: CC0-1.0
-->

Capture traffic for video througput:

```bash
tshark -i 5gc_bridge -t u -w core.pcap
sudo ip netns exec ue1 tshark -i tun_srsue -t u -w /tmp/ue.pcap

tshark -r core.pcap -Y "!(arp)" -w core.pcap
tshark -r core.pcap -Y "!(sctp)" -w core.pcap
tshark -r core.pcap -Y "!(gtp)" -w core.pcap
```

Unfortunately, the script to convert the PCAP into the format required for plotting is currently not provided within this repository.
