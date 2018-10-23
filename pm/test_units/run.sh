#!/usr/bin/env bash
clear
gcc ../pmhelper.c ../parse_json.c ../node.c pmhelper_test.c parse_json_test.c node_test.c test_constants.c test_all.c ./../../../Unity/src/unity.c -Wall -o test_all.out -ljansson 
./test_all.out
rm test_all.out