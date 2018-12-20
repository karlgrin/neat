cmake_minimum_required (VERSION 2.6)
set(CMAKE_BUILD_TYPE Debug)
project (pm C)
add_executable(pm pm_helper.c parse_json.c rest.c node.c pib.c cib.c pm.c)
target_link_libraries (pm "-ljansson -luv -lpthread -lulfius")

set(SERVICE_OUT "neat_pm.service")
set(SERVICE_IN "${SERVICE_OUT}.in")
set(SERVICE_DIR "/etc/systemd/system")

set(CMAKE_INSTALL_BINDIR "bin")

# NEAT PM service for systemd
configure_file(${CMAKE_SOURCE_DIR}/${SERVICE_IN} ${CMAKE_SOURCE_DIR}/${SERVICE_OUT})
message("Generated neat_pm.service")

install(TARGETS pm DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_SOURCE_DIR}/${SERVICE_OUT} DESTINATION ${SERVICE_DIR})
