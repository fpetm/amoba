add_library(libamoba STATIC
  ${CMAKE_SOURCE_DIR}/libamoba/amoba/amoba.h ${CMAKE_SOURCE_DIR}/libamoba/amoba/amobas/inf_am.h
  ${CMAKE_SOURCE_DIR}/libamoba/inf_am.hpp ${CMAKE_SOURCE_DIR}/libamoba/inf_am.cpp
  ${CMAKE_SOURCE_DIR}/libamoba/offsets.cpp
  ${CMAKE_SOURCE_DIR}/libamoba/util.hpp ${CMAKE_SOURCE_DIR}/libamoba/util.cpp)
target_include_directories(libamoba PUBLIC ${CMAKE_SOURCE_DIR}/libamoba)
target_link_libraries(libamoba PRIVATE amoba::amoba_options amoba::amoba_warnings)
