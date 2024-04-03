add_executable(amoba amoba.cpp)
target_link_libraries(amoba PRIVATE libamoba amoba::amoba_options amoba::amoba_warnings)
configure_file("config.hpp.in" "${CMAKE_BINARY_DIR}/configured_files/include/internal_use_only/config.hpp" ESCAPE_QUOTES)
target_include_directories(amoba PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
