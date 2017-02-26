# add sources
set(EDM_SAMPLE_SERVER_SOURCES
    main.c)

# add libraries
set(LIBS
    enet)

# add main executable and link it
add_executable(edm_sample_server ${EDM_SAMPLE_SERVER_SOURCES})
target_link_libraries(edm_sample_server ${LIBS})
