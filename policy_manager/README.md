
# Quick start

## Build

To build the policy manager run

```
$ cd pm
$ mkdir build
$ cd build
$ cmake ..
$ make
```

The policy manager can now be started with

```
$ ./pm
```

## Install

To install the policy manager run

```
$ sudo make install
```

This will install the policy manager in /usr/local/bin and create a new systemd service **neat_pm.service**.

To start the policy manager run

```
$ sudo systemctl start neat_pm
```

PM will now run as a background service.

To have the PM start at system boot run

```
$ sudo systemctl enable neat_pm
```
