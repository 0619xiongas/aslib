file(GLOB LOG_SOURCES "*.cpp")
file(GLOB LOG_HEADERS "../../include/log/*.h")

add_library(log OBJECT
    ${LOG_SOURCES}
    ${LOG_HEADERS}
)

target_include_directories(log PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)