# Config header:
configure_file(
        "udp_filtering_proxy_config.h.in"
        "${PROJECT_BINARY_DIR}/udp_filtering_proxy_config.h")

# add binary dir to the search path for include files,
# so that we will find client_config.h
include_directories("${PROJECT_BINARY_DIR}")

# add sources
set(UDP_FILTERING_PROXY_SOURCES
        simple_event_loop.h
        simple_event_loop.c
        log.h
        log.c
        main.c
        udp_filtering_proxy.h
        udp_filtering_proxy.c
        incoming_from_world.h
        incoming_from_world.c
        incoming_common.c
        incoming_common.h
        client_data.c
        client_data.h
        incoming_from_server.c
        incoming_from_server.h)

FIND_LIBRARY(EVENT_LIB event)
FIND_LIBRARY(AVL_LIB avl)
FIND_LIBRARY(PTHREAD_LIB pthread)
# add libraries
set(LIBS ${EVENT_LIB} ${AVL_LIB} ${PTHREAD_LIB})

# add main executable
add_executable(udp_filtering_proxy ${UDP_FILTERING_PROXY_SOURCES})

target_link_libraries(udp_filtering_proxy ${LIBS})

