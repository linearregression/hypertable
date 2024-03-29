#
# Copyright (C) 2007 Doug Judd (Zvents, Inc.)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# 

project (HYPERTABLE)

# Enable testing
enable_testing()

if (CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -pedantic -Wno-long-long -Wno-variadic-macros)
endif (CMAKE_COMPILER_IS_GNUCXX)

# Uncomment the following to build dynamic libraries
# set (BUILD_SHARED_LIBS ON)

# Locate required packages
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(ZLIB REQUIRED)
find_package(Lzo2 REQUIRED)
find_package(Boost REQUIRED)
find_package(Log4cpp REQUIRED)
find_package(EXPAT REQUIRED)
find_package(Doxygen)
find_package(Tcmalloc)

# include directories
include_directories(${Boost_INCLUDE_DIRS} ${Log4cpp_INCLUDE_DIR} src/cc)

# link directories
link_directories(${Boost_LIBRARY_DIRS})

IF(EXISTS "${Boost_LIBRARY_DIR}/libboost_thread.a")
  set (BOOST_LIB "boost_thread")
ELSEIF(EXISTS "${Boost_LIBRARY_DIR}/libboost_thread-mt.a")
  set (BOOST_LIB "boost_thread-mt")
ELSE(EXISTS "${Boost_LIBRARY_DIR}/libboost_thread-gcc41-mt.a")
  set (BOOST_LIB "boost_thread-gcc41-mt")
ENDIF(EXISTS "${Boost_LIBRARY_DIR}/libboost_thread.a")

IF (DOXYGEN_FOUND)
   configure_file(${HYPERTABLE_SOURCE_DIR}/doc/Doxyfile ${HYPERTABLE_BINARY_DIR}/Doxyfile)
   add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${HYPERTABLE_BINARY_DIR}/Doxyfile)
ENDIF (DOXYGEN_FOUND)

# Recurse into the "Hello" and "Demo" subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.
add_subdirectory (src/cc/AsyncComm)
add_subdirectory (src/cc/Common)
add_subdirectory (src/cc/Hyperspace)
add_subdirectory (src/cc/Hypertable/Master)
add_subdirectory (src/cc/Hypertable/RangeServer)
add_subdirectory (src/cc/Hypertable/Lib)
add_subdirectory (src/cc/Tools/dfsclient)
add_subdirectory (src/cc/Tools/hyperspace)
add_subdirectory (src/cc/Tools/hypertable)
add_subdirectory (src/cc/Tools/rsclient)
add_subdirectory (src/cc/Tools/rsdump)
add_subdirectory (src/cc/Tools/serverup)
add_subdirectory (src/cc/DfsBroker/Lib)
add_subdirectory (src/cc/DfsBroker/local)
#add_subdirectory (src/cc/DfsBroker/kosmos)

install (FILES demo/metadata-test.xml DESTINATION demo)
install (FILES demo/testdata.txt.bz2 DESTINATION demo)
install (FILES demo/Test.xml DESTINATION demo)
install (FILES demo/Test-schema.xml DESTINATION demo)
install (FILES conf/hypertable.cfg DESTINATION conf)
install (FILES conf/METADATA.xml DESTINATION conf)
install (FILES lib/commons-logging-1.0.4.jar DESTINATION java/lib)
install (FILES lib/hadoop-0.14.4-core.jar DESTINATION java/lib)
install (FILES lib/junit-4.3.1.jar DESTINATION java/lib)
install (FILES lib/log4j-1.2.13.jar DESTINATION java/lib)
install (FILES build/hypertable.jar DESTINATION java/lib)
install (FILES bin/jrun 
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	 DESTINATION bin)
install (FILES test/start-servers.sh
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	 DESTINATION test)
install (FILES test/stop-servers.sh
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	 DESTINATION test)
install (FILES test/Test1-create.hql DESTINATION test)
install (FILES test/Test2-create.hql DESTINATION test)
install (FILES test/Test3-create.hql DESTINATION test)
install (FILES test/metadata.orig DESTINATION test)

