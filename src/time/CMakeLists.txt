file(GLOB TIME_SOURCES "*.cpp")
file(GLOB TIME_HEADERS "../../include/time/*.h")

add_library(time OBJECT
    ${TIME_SOURCES}
    ${TIME_HEADERS}
)

target_include_directories(time PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)