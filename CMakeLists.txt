cmake_minimum_required (VERSION 3.0)
project (ICON_SDK_CPP)

# === GLOBAL =======================================================================================================
# --- Directories
set (ROOTDIR ${PROJECT_SOURCE_DIR})
set (LIBDIR ${ROOTDIR}/lib/${CMAKE_ARCHITECTURE}/)
set (RELEASE_DIR ${ROOTDIR}/release/${CMAKE_ARCHITECTURE}/)
set (VCPKG_ROOT $ENV{VCPKG_ROOT})

# --- Linker Paths
link_directories (${LIBDIR})
if (MSVC)
    # VCPKG directories on Windows
    link_directories (${VCPKG_ROOT}/installed/${CMAKE_ARCHITECTURE}-windows/lib/)
    link_directories (${VCPKG_ROOT}/installed/${CMAKE_ARCHITECTURE}-windows/lib/manual-link/)
endif (MSVC)

# --- Includes Paths
include_directories (${ROOTDIR}/third_party)
if (MSVC)
    # VCPKG directories on Windows
    include_directories (${VCPKG_ROOT}/installed/${CMAKE_ARCHITECTURE}-windows/include/)
endif (MSVC)

# --- Packages
find_package (GTest REQUIRED)
find_package (spdlog CONFIG REQUIRED)
find_package (OpenSSL REQUIRED)
find_package (CURL REQUIRED)

# --- Dependency : icon-sdk-cpp, common-lib-cpp
add_subdirectory (icon-sdk-cpp)
include_directories (icon-sdk-cpp/src)
include_directories (icon-sdk-cpp/common-lib-cpp/src)

# --- CMAKE Configs
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RELEASE_DIR})
if (MSVC)
    set (CMAKE_CXX_FLAGS_RELEASE "/std:c++17 /MD /O2")
    set (CMAKE_CXX_FLAGS_DEBUG "/std:c++17 /MDd /Od /Zi")
else ()
    # Assume g++
    set (CMAKE_CXX_FLAGS_RELEASE "-std=c++17 -O2 -pthread")
    set (CMAKE_CXX_FLAGS_DEBUG "-std=c++17 -g -pthread")
endif (MSVC)

# --- Librairies definition
set (MYSQL_LIBRARIES mysqlcppconn8)
if (MSVC)
else ()
    set (FILESYSTEM_LIBRARIES stdc++fs)
endif (MSVC)

# === Aegis =======================================================================================================

# Directories
set (AEGIS_ROOT ${ROOTDIR})
set (AEGIS_SRCDIR ${AEGIS_ROOT}/src)
set (AEGIS_TESTDIR ${AEGIS_ROOT}/tests)

# --- SPDLOG defines
add_definitions (-DSPDLOG_FMT_EXTERNAL)
add_definitions (-DFMT_HEADER_ONLY)

# Includes
include_directories (${AEGIS_SRCDIR})

# Aegis source code
file (GLOB_RECURSE AEGIS_SRC
    ${AEGIS_SRCDIR}/*.cpp     ${AEGIS_SRCDIR}/*.h
)

# --- Tests source code
file (GLOB_RECURSE AEGIS_TESTS_SRC
    ${AEGIS_TESTDIR}/main.cpp
)

# --- Aegis Library
add_library (Aegis_lib STATIC ${AEGIS_SRC})
target_include_directories (Aegis_lib PRIVATE ${AEGIS_SRCDIR})
target_link_libraries (Aegis_lib SDK_lib common_lib ${MYSQL_LIBRARIES} ${FILESYSTEM_LIBRARIES})

# --- Aegis executable
add_executable (Aegis ${AEGIS_SRC})
target_include_directories (Aegis PRIVATE ${AEGIS_SRCDIR})
target_link_libraries (Aegis SDK_lib common_lib ${MYSQL_LIBRARIES} ${FILESYSTEM_LIBRARIES})
set_target_properties(Aegis PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(Aegis PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# --- Aegis Tests executable
add_executable (Aegis_Tests ${AEGIS_TESTS_SRC})
target_include_directories (Aegis_Tests PRIVATE ${AEGIS_SRCDIR})
target_link_libraries(Aegis_Tests Aegis_lib common_lib ${GTEST_LIBRARIES} ${PTHREAD_LIBRARIES} ${MYSQL_LIBRARIES})
set_target_properties(Aegis_Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(Aegis_Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
