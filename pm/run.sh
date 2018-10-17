#!/bin/bash

clear
gcc pmhelper.c property.c multi_property.c node.c pib.c cib.c pm.c -Wall -ggdb -o test -ljansson -luv
./test
