# Unit testing

## Unity

The framework used to execute the unit tests is **Unity**. Download Unity by entering the /neat folder then run:

```
$ cd ..
$ git clone https://github.com/ThrowTheSwitch/Unity
```

It can also be manually downloaded from: https://github.com/ThrowTheSwitch/Unity

## Quick start

To **build** the unit tests to the policy manager, go into the /neat folder then run:

```
$ cd policy_manager/unit_testing
$ mkdir build
$ cd build
$ cmake ..
$ make
```

The unit tests can now be **executed** with

```
$ ./test_pm
```
