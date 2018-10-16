#!/bin/bash

clear
gcc pmhelper.c property.c multi_property.c node.c pib.c cib.c neat_pm.c -Wall -ggdb -o neat_pm -ljansson
./test
