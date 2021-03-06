
# Policy Manager
## Requirements
**Note**: Developed and tested on Ubuntu 18.04 64-bit and Debian GNU/Linux 9,
* `ljansson`
* `libuv`
* `libulfius-dev`
* `libmicrohttpd-dev`

### Install requirements
##### Packages
| Prerequisite        | Install command           | 
| ------------- |:-------------:|
| If you **already have** the NEAT dependencies | `apt-get install libmicrohttpd-dev libulfius-dev` |
| If you **don't have** the NEAT dependencies. | `apt-get install cmake libjansson-dev libuv1-dev libmicrohttpd-dev libulfius-dev` |

## Quick Start

Start with copying your **PIB files** to the backend folder. To copy the already existing PIB files to backend, go to /neat folder then run:

```
$ mkdir -p ~/.neat/infobase/pib/profile
$ mkdir -p ~/.neat/infobase/pib/policy
$ cp policy_manager/json_examples/pib/*.profile ~/.neat/infobase/pib/profile
$ cp policy_manager/json_examples/pib/*.policy ~/.neat/infobase/pib/policy
```

To **build & run** the policy manager, go into the /neat folder then run:

```
$ cd policy_manager
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./pm
```

Possible **arguments** to PM:
```
./pm -debug           (Print debug messages in the console)
./pm -log             (Write all log messages to file: Log.txt)
```

## Install as a daemon

To install the policy manager as a daemon:

```
$ sudo make install
```

This will install the policy manager in /usr/local/bin and create a new systemd service **neat_pm.service**.

To start the policy manager:

```
$ sudo systemctl start neat_pm
```

PM will now run as a background service.

To have the PM start at system boot run

```
$ sudo systemctl enable neat_pm
```
