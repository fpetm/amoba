add_executable(amoba amoba.cpp)

set(BOOST_INCLUDEDIR    "${PROJECT_SOURCE_DIR}/extern/boost_1_82_0")
set(BOOST_LIBRARYDIR    "${PROJECT_SOURCE_DIR}/extern/boost_1_82_0/lib64-msvc-12.0")
set(BOOST_ROOT          "${PROJECT_SOURCE_DIR}/extern/boost_1_82_0/boost")
find_package(Boost REQUIRED)
target_link_libraries(amoba PRIVATE libamoba amoba::amoba_options amoba::amoba_warnings ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)
configure_file("config.hpp.in" "${CMAKE_BINARY_DIR}/configured_files/include/internal_use_only/config.hpp" ESCAPE_QUOTES)
target_include_directories(amoba PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include" ${PROJECT_SOURCE_DIR}/extern/websocketpp ${Boost_INCLUDE_DIRS})
