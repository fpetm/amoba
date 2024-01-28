add_executable(amoba amoba.cpp)
configure_file("config.hpp.in" "${CMAKE_BINARY_DIR}/configured_files/include/internal_use_only/config.hpp" ESCAPE_QUOTES)
target_include_directories(amoba PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
