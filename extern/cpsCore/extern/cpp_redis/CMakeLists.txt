cmake_minimum_required(VERSION 3.13)

###
# project
###
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.12)
  cmake_policy(SET CMP0048 NEW)
else ()
  # for cmake >= 3.0
endif ()
if (POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif ()

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_CXX_STANDARD 11)

set(LIBRARY_NAME cpp_redis)

include(sources.cmake)
set(BUILD_EXAMPLES OFF)
set(BUILD_TESTS OFF)
add_subdirectory(tacopie)


set(cpp_redis_includes ${CMAKE_CURRENT_LIST_DIR}/includes)
set(deps_includes ${CMAKE_CURRENT_LIST_DIR}/deps/include)

set(deps_libraries ${PROJECT_SOURCE_DIR}/deps/lib)

###
# main target
###
add_library(cpp_redis SHARED ${cpp_redis_sources})

###
# includes
###
target_include_directories(cpp_redis PUBLIC
        $<BUILD_INTERFACE:${cpp_redis_includes}>
        $<INSTALL_INTERFACE:include>)

find_package(Threads REQUIRED)
target_link_libraries(cpp_redis PRIVATE Threads::Threads)


target_link_libraries(cpp_redis PUBLIC tacopie)


###
# parameters
###
set(READ_SIZE "4096" CACHE STRING "Number of bytes read from sockets")
if (READ_SIZE MATCHES "^[1-9]+")
  target_compile_definitions(cpp_redis PRIVATE __CPP_REDIS_READ_SIZE=${READ_SIZE})
endif ()

option(LOGGING_ENABLED "If true enables logging calls to cpp_redis::level()" OFF)
if (LOGGING_ENABLED)
  target_compile_definitions(cpp_redis PRIVATE __CPP_REDIS_LOGGING_ENABLED)
endif ()

option(USE_CUSTOM_TCP_CLIENT "If true user must provide a custom tcp_client_iface instance, otherwise uses tacopie" OFF)
if (USE_CUSTOM_TCP_CLIENT)
  target_compile_definitions(cpp_redis PRIVATE __CPP_REDIS_USE_CUSTOM_TCP_CLIENT)
endif ()

###
# install
###
install(DIRECTORY ${cpp_redis_includes}/ DESTINATION include USE_SOURCE_PERMISSIONS)

include(GNUInstallDirs)
install(TARGETS cpp_redis tacopie
        LIBRARY DESTINATION lib)
