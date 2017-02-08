# Config header:
configure_file (
    "client_config.h.in"
    "${PROJECT_BINARY_DIR}/client_config.h")

# add binary dir to the search path for include files,
# so that we will find client_config.h
include_directories("${PROJECT_BINARY_DIR}")

# add sources
set(EDM_CLIENT_SOURCES
    main.c)

# add libraries
set(LIBS
    enet)

# add main executable
add_executable(edm_client ${EDM_CLIENT_SOURCES})

# add Enet library
target_link_libraries(edm_client ${LIBS})

