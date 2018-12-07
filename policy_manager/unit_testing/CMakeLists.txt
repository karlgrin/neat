cmake_minimum_required (VERSION 2.6)
project (test_pm C)
add_executable(test_pm ../pmhelper.c ../parse_json.c ../node.c pmhelper_test.c parse_json_test.c node_test.c test_constants.c test_all.c ./../../../Unity/src/unity.c)
target_link_libraries (test_pm "-ljansson -luv")
