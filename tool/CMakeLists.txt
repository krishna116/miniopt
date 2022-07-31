add_executable(${PROJECT_NAME} 
    miniopt.cpp
    ${CMAKE_SOURCE_DIR}/src/miniopt.c
)

configure_file(config.h.txt ${CMAKE_CURRENT_BINARY_DIR}/config.h)

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

#-------------------------------------------
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)