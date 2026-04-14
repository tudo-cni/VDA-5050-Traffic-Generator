<!-- 
   SPDX-FileCopyrightText: Copyright (C) Lars Toenning <dev at ltoenning dot de>
   SPDX-License-Identifier: MIT
-->

This repository is related to the publication
*Teleoperating Mobile Robots via VDA 5050 – A First Middleware Evaluation within Open Industrial Networks* presented at the IEEE International Conference on Factory Communication Systems (WFCS) in Offenburg, Germany in April 2026.

It provides a VDA 5050 message simulation using the discrete-event simulator ns-3 including the proposed message exchange for teleoperation.
Additionally, evaluation results obtained using the VDA 5050 simulator with ns-3 in real-time mode over an open industrial 5G network can be found in the ``eval`` subfolder.
The evaluation was conducted using the MQTT and Zenoh middleware. 


## VDA 5050 Simulation

The simulation consists of a three parts: **Controller**, **Base** and **UE**.
- The **Controller** contains the simulation logic using the VDA 5050, running the ns-3 simulation.
It schedules VDA 5050 messages and teleoperation-related commands and delegates them either to the UE (representing the mobile robot) or Base (representing the master control according to the VDA 5050).
- The **Base** and **UE*** execute these commands, for example by publishing MQTT or Zenoh messages. 

### Build the Simulation

The VDA 5050 simulation is built using CMake.

The following dependencies must be installed:

- ns-3
- msgpack-cxx
- Paho MQTT
- Zenoh-cxx
- yaml-cpp
- gstreamer
- ZeroMQ

Build the project with:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Execute the Simulation via a industrial 5G network

For the 5G Core, Open5GS is used from the [srsRAN Project Docker Compose file](https://github.com/srsran/srsRAN_Project/blob/main/docker/docker-compose.yml). 
Ensure that the 5G Core and the srsRAN gNB are started first.

The ``.vscode`` subfolder contains presets to start the Controller, Base, and UE client applications in different configurations.
Launch the UE client application on the UE. If using a UE simulation such as srsue from the srsRAN 4G project, run it within the corresponding network namespace.
The Base client application should be launched on the same machine as the 5G Core.
Base and UE client application can be configured via the YAML configuration files found in the ``src/base`` or ``src/ue`` folder.
These configuration files configure the logging output path, which can be used to extract some metrics, used for evaluation.

**After** starting the UE and Base client applications, launch the Controller. 
The Controller is configured via the ``config.yaml`` file in the project’s top-level directory, which contains simulation parameters.

### Using Zenoh with QUIC

To generate TLS certificates required to operate with QUIC, execute the following commands:


1. Create Root CA
   ```bash
   openssl genrsa -out rootCA.key 2048
   openssl req -x509 -new -nodes -key rootCA.key -sha256 -days 1024 -out rootCA.crt
   ```

2. Create Local Key 
   ```bash
   openssl genrsa -out localhost.key 2048
   openssl req -new -key localhost.key -out localhost.csr -config localhost.cnf
   openssl x509 -req -in localhost.csr -CA rootCA.pem -CAkey rootCA.key -out localhost.crt -days 500 -sha256 -copy_extensions copy
   ```

   with config

   ```
   [ req ]
      default_bits       = 2048
      distinguished_name = req_distinguished_name
      req_extensions     = v3_req
      prompt             = no

      [ req_distinguished_name ]
      C  = AU
      ST = Some-State
      O  = Internet Widgits Pty Ltd
      CN = 127.0.0.1

      [ v3_req ]
      keyUsage = critical, digitalSignature, keyEncipherment
      extendedKeyUsage = serverAuth
      subjectAltName = @alt_names

      [ alt_names ]
      DNS.1 = localhost
      IP.1 = 127.0.0.1
      IP.2 = 10.45.1.2
      IP.3 = 10.53.1.1
      IP.4 = 10.53.1.3
   ```

# Acknowledgement

To acknowledge this work in your publication please refer to the following publication:

```bibtex
@inproceedings{WagnerToenning2026b,
   author = {Wagner, Niklas A. and Tönning, Lars and Lünsch, Dennis and Jost, Jana and Wietfeld, Christian and Detzner, Peter},
   booktitle = {IEEE International Conference on Factory Communication Systems (WFCS)},
   month = apr,
   title = {{Teleoperating Mobile Robots via VDA 5050 – A First Middleware Evaluation within Open Industrial Networks}},
   year = {2026}
}
```

