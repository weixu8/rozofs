# Copyright (c) 2010 Fizians SAS. <http://www.fizians.com>
# This file is part of Rozofs.
#
# Rozofs is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation, version 2.
#
# Rozofs is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see
# <http://www.gnu.org/licenses/>.

#
# Project settings.
#
cmake_minimum_required(VERSION 2.6)
project(rozofs C)
set(MAJOR "0")
set(MINOR "13")
set(REVISION "0")
set(VERSION "${MAJOR}.${MINOR}.${REVISION}")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake")

#
# Find packages
#
find_package(UUID REQUIRED)
find_package(CRYPT REQUIRED)
find_package(CONFIG REQUIRED)
find_package(XATTR REQUIRED)
find_package(PTHREAD REQUIRED)
find_package(FUSE REQUIRED)
find_package(Doxygen QUIET)
find_program(DEBUILD NAMES debuild)

IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#
# Project config.
#
if(NOT SYSCONFDIR)
    set(SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc")
endif(NOT SYSCONFDIR)

set(ROZOFS_CONFIG_DIR "${SYSCONFDIR}/rozofs")

set(EXPORTD_DEFAULT_CONFIG "${ROZOFS_CONFIG_DIR}/export.conf")

set(STORAGED_DEFAULT_CONFIG "${ROZOFS_CONFIG_DIR}/storage.conf")

if(NOT DAEMON_PID_DIRECTORY)
    set(DAEMON_PID_DIRECTORY "/var/run")
endif(NOT DAEMON_PID_DIRECTORY)

if(NOT ROZOFS_RPC_BUFFER_SIZE)
    set(ROZOFS_RPC_BUFFER_SIZE 0x0)
endif(NOT ROZOFS_RPC_BUFFER_SIZE)

string(REGEX MATCH "$\\\\/" TRAILING_SLASH "${DAEMON_PID_DIRECTORY}")
if(NOT TRAILING_SLASH)
    set(DAEMON_PID_DIRECTORY "${DAEMON_PID_DIRECTORY}/")
endif(NOT TRAILING_SLASH)

add_subdirectory(rozofs)
add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(tests)
add_subdirectory(tests/fs_ops/fileop)
add_subdirectory(tests/fs_ops/pjd-fstest)

set(ROZOFS_PACKAGE_FILE_NAME "rozofs-${VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(ROZOFS_SOURCE_PACKAGE_FILE_NAME "rozofs-${VERSION}")

configure_file("${PROJECT_SOURCE_DIR}/cmake/uninstall.in" "${PROJECT_BINARY_DIR}/uninstall.cmake" @ONLY)
configure_file("${PROJECT_SOURCE_DIR}/cmake/debian.in" "${PROJECT_BINARY_DIR}/debian.cmake" @ONLY)
configure_file("${PROJECT_SOURCE_DIR}/cmake/ubuntu.in" "${PROJECT_BINARY_DIR}/ubuntu.cmake" @ONLY)
configure_file("${PROJECT_SOURCE_DIR}/cmake/config.h.in" "${PROJECT_BINARY_DIR}/config.h")
configure_file("${PROJECT_SOURCE_DIR}/cmake/export.conf.in" "${PROJECT_BINARY_DIR}/src/exportd/export.conf.sample")
configure_file("${PROJECT_SOURCE_DIR}/cmake/storage.conf.in" "${PROJECT_BINARY_DIR}/src/storaged/storage.conf.sample")
configure_file("${PROJECT_SOURCE_DIR}/doc/rozofs.7" "${PROJECT_BINARY_DIR}/doc/rozofs.7")
configure_file("${PROJECT_SOURCE_DIR}/doc/rozofsmount.8" "${PROJECT_BINARY_DIR}/doc/rozofsmount.8")
configure_file("${PROJECT_SOURCE_DIR}/doc/storaged.8" "${PROJECT_BINARY_DIR}/doc/storaged.8")
configure_file("${PROJECT_SOURCE_DIR}/doc/storage.conf.5" "${PROJECT_BINARY_DIR}/doc/storage.conf.5")
configure_file("${PROJECT_SOURCE_DIR}/doc/exportd.8" "${PROJECT_BINARY_DIR}/doc/exportd.8")
configure_file("${PROJECT_SOURCE_DIR}/doc/export.conf.5" "${PROJECT_BINARY_DIR}/doc/export.conf.5")
configure_file("${PROJECT_SOURCE_DIR}/cmake/doxyfile.in" "${PROJECT_BINARY_DIR}/doc/doxyfile" IMMEDIATE @ONLY)

#
# Build settings
#
if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
  "Choose the type of build, options are: None Debug Release Profile RelWithDebInfo MinSizeRel."
  FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -D_GNU_SOURCE" CACHE STRING
    "Flags used by the C compiler during maintainer builds."
    FORCE)

set(CMAKE_C_FLAGS_DEBUG "-g -Wall -DNDEBUGFUNCTION -D_GNU_SOURCE" CACHE STRING
    "Flags used by the C compiler during maintainer builds."
    FORCE)

#
# uninstall target
#
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake")
add_custom_target(package_debian "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/debian.cmake")
add_custom_target(package_ubuntu "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/ubuntu.cmake")


#
# Packing
#
include(InstallRequiredSystemLibraries)

# Binaries
set(CPACK_GENERATOR "STGZ;TGZ")
set(CPACK_STGZ_COMPONENT_INSTALL ON)
set(CPACK_TGZ_COMPONENT_INSTALL ON)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Rozofs filesystem")
set(CPACK_PACKAGE_VENDOR "Fizians SAS")
set(CPACK_PACKAGE_CONTACT "Fizians SAS")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION "${VERSION}")
set(CPACK_PACKAGE_FILE_NAME ""${ROZOFS_PACKAGE_FILE_NAME}"")
set(CPACK_COMPONENTS_ALL client export storage)
set(CPACK_COMPONENT_CLIENT_DISPLAY_NAME "rozofsmount")
set(CPACK_COMPONENT_EXPORT_DISPLAY_NAME "exportd")
set(CPACK_COMPONENT_STORAGE_DISPLAY_NAME "storaged")
set(CPACK_COMPONENT_CLIENT_DESCRIPTION "rozofsmount")
set(CPACK_COMPONENT_EXPORT_DESCRIPTION "exportd")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION "storaged")

# Sources
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${ROZOFS_SOURCE_PACKAGE_FILE_NAME}")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "exportd$;/config.h$;rozofsmount$;sample$;storaged$;.mk$;throughput_server$;throughput$;test_list$;test_storage$;transform_file$;test_htable$;test_transform$;test_volume$;test_dist$;install_manifest.txt;package.cmake;ubuntu.cmake;debian.cmake;/build/;tags;/CMakeFiles/;/_CPack_Packages/;/\\\\.hg/;CMakeCache.txt;Makefile$;\\\\.vim$;\\\\.swp$;uninstall.cmake$;cmake_install.cmake;CPackConfig.cmake;CPackSourceConfig.cmake;~$;tags;/nbproject/;.hgignore;.dep.inc;/\\\\.settings;/\\\\.git;/\\\\.project;/\\\\.cproject")
include(CPack)
