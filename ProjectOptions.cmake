include(CMakeDependentOption)
include(CheckCXXCompilerFlag)

macro(amoba_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(amoba_setup_options)
  option(amoba_ENABLE_COVERAGE "Enable coverage reporting" OFF)

  amoba_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR amoba_PACKAGING_MAINTAINER_MODE)
    option(amoba_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(amoba_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(amoba_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(amoba_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(amoba_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(amoba_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(amoba_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(amoba_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(amoba_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(amoba_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(amoba_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(amoba_ENABLE_PCH "Enable precompiled headers" OFF)
    option(amoba_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(amoba_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(amoba_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(amoba_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(amoba_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(amoba_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(amoba_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(amoba_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(amoba_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(amoba_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(amoba_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(amoba_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(amoba_ENABLE_PCH "Enable precompiled headers" OFF)
    option(amoba_ENABLE_CACHE "Enable ccache" OFF)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      amoba_ENABLE_IPO
      amoba_WARNINGS_AS_ERRORS
      amoba_ENABLE_USER_LINKER
      amoba_ENABLE_SANITIZER_ADDRESS
      amoba_ENABLE_SANITIZER_LEAK
      amoba_ENABLE_SANITIZER_UNDEFINED
      amoba_ENABLE_SANITIZER_THREAD
      amoba_ENABLE_SANITIZER_MEMORY
      amoba_ENABLE_UNITY_BUILD
      amoba_ENABLE_CLANG_TIDY
      amoba_ENABLE_CPPCHECK
      amoba_ENABLE_COVERAGE
      amoba_ENABLE_PCH
      amoba_ENABLE_CACHE)
  endif()
endmacro()

macro(amoba_global_options)
  if(amoba_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    amoba_enable_ipo()
  endif()

  amoba_supports_sanitizers()
endmacro()

macro(amoba_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(amoba_warnings INTERFACE)
  add_library(amoba_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  amoba_set_project_warnings(
    amoba_warnings
    ${amoba_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(amoba_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(amoba_options)
  endif()

  include(cmake/Sanitizers.cmake)
  amoba_enable_sanitizers(
    amoba_options
    ${amoba_ENABLE_SANITIZER_ADDRESS}
    ${amoba_ENABLE_SANITIZER_LEAK}
    ${amoba_ENABLE_SANITIZER_UNDEFINED}
    ${amoba_ENABLE_SANITIZER_THREAD}
    ${amoba_ENABLE_SANITIZER_MEMORY})

  set_target_properties(amoba_options PROPERTIES UNITY_BUILD ${amoba_ENABLE_UNITY_BUILD})

  if(amoba_ENABLE_PCH)
    target_precompile_headers(
      amoba_options
      INTERFACE
      # TODO add precompiled headers
      <vector>
      <string>
      <utility>)
  endif()

  if(amoba_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    amoba_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(amoba_ENABLE_CLANG_TIDY)
    amoba_enable_clang_tidy(amoba_options ${amoba_WARNINGS_AS_ERRORS})
  endif()

  if(amoba_ENABLE_CPPCHECK)
    amoba_enable_cppcheck(${amoba_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(amoba_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    amoba_enable_coverage(amoba_options)
  endif()

  if(amoba_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(myproject_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()
endmacro()
