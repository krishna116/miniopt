# generate packres.cpp -----------------------------------
set(LIBRARY_SOURCE_FILE_LIST
    ${CMAKE_SOURCE_DIR}/src/miniopt.h
    ${CMAKE_SOURCE_DIR}/src/miniopt.c
)
set(PACKRES_CPP "${CMAKE_CURRENT_BINARY_DIR}/packres.cpp")
add_custom_command(
    OUTPUT ${PACKRES_CPP}
    COMMAND ${CMAKE_COMMAND}  "-DIN_FILE_LIST=${LIBRARY_SOURCE_FILE_LIST}"
                              "-DOUT=${PACKRES_CPP}"
                              -P ${CMAKE_SOURCE_DIR}/cmake/packres.cmake
    DEPENDS ${SOURCE_FILE_LIST}
)

# config -------------------------------------------
configure_file(config.h.txt ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# add executable -------------------------------------------
add_executable(${PROJECT_NAME} 
    miniopt.cpp
    ${PACKRES_CPP}
    ${CMAKE_SOURCE_DIR}/src/miniopt.c
)

target_include_directories(${PROJECT_NAME} 
PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(TARGET ${PROJECT_NAME} 
POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "== Code generator build finished =="
    COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_FILE:${PROJECT_NAME}>"
)

# install -------------------------------------------
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)