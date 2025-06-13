file(GLOB FILE_SOURCES "*.cpp")
file(GLOB FILE_HEADERS "../../include/file/*.h")

add_library(file OBJECT
    ${FILE_SOURCES}
    ${FILE_HEADERS}
)

target_include_directories(file PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)