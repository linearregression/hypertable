#
# Copyright (C) 2007 Doug Judd (Zvents, Inc.)
# 
# This file is part of Hypertable.
# 
# Hypertable is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or any later version.
# 
# Hypertable is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# 

set ( SRC_DIR "${HYPERTABLE_SOURCE_DIR}/src/cc/Hypertable/RangeServer" )
set ( DST_DIR "${HYPERTABLE_BINARY_DIR}/src/cc/Hypertable/RangeServer" )

set ( TEST_DEPENDENCIES ${DST_DIR}/shakespeare.txt ${DST_DIR}/words ${DST_DIR}/urls.txt ${DST_DIR}/tests )

set ( RangeServer_SRCS
AccessGroup.cc
BlockCompressionHeaderCellStore.cc
CellCache.cc
CellCacheScanner.cc
CellStoreScannerV0.cc
CellStoreTrailerV0.cc
CellStoreV0.cc
ConnectionHandler.cc
EventHandlerMasterConnection.cc
FileBlockCache.cc
FillScanBlock.cc
Global.cc
HyperspaceSessionHandler.cc
MaintenanceThread.cc
MergeScanner.cc
Range.cc
RangeServer.cc
RequestHandlerCompact.cc
RequestHandlerCreateScanner.cc
RequestHandlerDumpStats.cc
RequestHandlerFetchScanblock.cc
RequestHandlerLoadRange.cc
RequestHandlerStatus.cc
RequestHandlerUpdate.cc
ResponseCallbackCreateScanner.cc
ResponseCallbackFetchScanblock.cc
ResponseCallbackUpdate.cc
ScanContext.cc
ScannerMap.cc
ScannerTimestampController.cc
TableInfo.cc
main.cc
)

# RangeServer
add_executable (Hypertable.RangeServer ${RangeServer_SRCS})
target_link_libraries (Hypertable.RangeServer pthread ${BOOST_LIB} Hypertable Hyperspace DfsBroker AsyncComm Common log4cpp expat ${ZLIB_LIBRARIES} ${Lzo2_LIBRARIES} ${Tcmalloc_LIBRARIES})

# csdump
add_executable (csdump CellStoreV0.cc CellStoreTrailerV0.cc BlockCompressionHeaderCellStore.cc csdump.cc CellStoreScannerV0.cc FileBlockCache.cc Global.cc ScanContext.cc)
target_link_libraries (csdump pthread ${BOOST_LIB} Hypertable DfsBroker AsyncComm Common log4cpp ${ZLIB_LIBRARIES} ${Lzo2_LIBRARIES} ${Tcmalloc_LIBRARIES})

#
# Get test environment setup
#
add_custom_command (SOURCE ${SRC_DIR}/tests/urls.txt.gz
                    COMMAND cp ARGS ${SRC_DIR}/tests/urls.txt.gz ${DST_DIR}
                    COMMAND gzip ARGS -d ${DST_DIR}/urls.txt.gz
                    OUTPUT ${DST_DIR}/urls.txt)
add_custom_command (SOURCE ${SRC_DIR}/tests/words.gz
                    COMMAND cp ARGS ${SRC_DIR}/tests/words.gz ${DST_DIR}
                    COMMAND gzip ARGS -d ${DST_DIR}/words.gz
                    OUTPUT ${DST_DIR}/words)
add_custom_command (SOURCE ${SRC_DIR}/tests/shakespeare.txt.gz
                    COMMAND cp ARGS ${SRC_DIR}/tests/shakespeare.txt.gz ${DST_DIR}
                    COMMAND gzip ARGS -d ${DST_DIR}/shakespeare.txt.gz
                    OUTPUT ${DST_DIR}/shakespeare.txt)
add_custom_command (SOURCE ${SRC_DIR}/tests
                    COMMAND ln ARGS -s ${SRC_DIR}/tests ${DST_DIR}/tests
                    OUTPUT ${DST_DIR}/tests)

install (TARGETS Hypertable.RangeServer csdump
	 RUNTIME DESTINATION bin)
install (FILES tests/urls.txt.gz tests/words.gz tests/shakespeare.txt.gz DESTINATION demo)

