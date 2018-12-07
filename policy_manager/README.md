
# Quick start

## Build

To build the policy manager:

```
$ cd policy_manager
$ mkdir build
$ cd build
$ cmake ..
$ make
```

The policy manager can now be executed with

```
$ ./pm
```

# Install as a daemon

To install the policy manager:

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
