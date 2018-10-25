cmake_minimum_required (VERSION 2.6)
project (PM C)
add_executable(PM pmhelper.c parse_json.c node.c pib.c cib.c pm.c)
target_link_libraries (PM "-ljansson -luv")
