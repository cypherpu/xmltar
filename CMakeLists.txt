project	(WebSchedule2 CXX)

cmake_minimum_required (VERSION 2.6)

set (CMAKE_BUILD_TYPE Debug)
SET(CMAKE_VERBOSE_MAKEFILE 1)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14")
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

link_directories(/usr/local/lib)
link_directories(/usr/lib64)

set(LIBS cairo cryptopp boost_date_time boost_unit_test_framework boost_random boost_filesystem boost_system xerces-c gtest)

include_directories(
src/
src/common/
src/common/Generated
src/common/Include
src/common/Snapshot
)

file(GLOB SOURCES 
	src/common/*.[ch]pp
	src/common/Generated/*.[ch]pp
	src/common/Snapshot/*.[ch]pp
	src/common/Xmltar/*.[ch]pp
)

file(GLOB TEST_SOURCES 
	src/common/Test.[ch]pp
	src/common/Tests/Test.*.[ch]pp
)

file(GLOB BIDIRECTIONAL_PIPE
	../Compression/Bidirectional_Pipe.[ch]pp
)

ADD_CUSTOM_COMMAND(
   OUTPUT src/common/Generated/*.[hc]pp
   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BIDIRECTIONAL_PIPE} ${CMAKE_CURRENT_SOURCE_DIR}/src/common/Generated
   DEPENDS src/common/Generated
)

ADD_CUSTOM_TARGET(generate_foo DEPENDS src/common/Generated/*.[hc]pp)
 
add_executable(xmltar src/xmltar.cpp ${SOURCES})
ADD_DEPENDENCIES(xmltar generate_foo)
set_target_properties(xmltar PROPERTIES RUNTIME_OUTPUT_DIRECTORY .. )
target_link_libraries (xmltar ${LIBS})

add_executable(Xmltar.Test src/Xmltar.Test.cpp ${SOURCES} ${TEST_SOURCES})
set_target_properties( Xmltar.Test PROPERTIES RUNTIME_OUTPUT_DIRECTORY .. )
target_link_libraries (Xmltar.Test ${LIBS})

