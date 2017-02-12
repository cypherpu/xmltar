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

set(LIBS cairo cryptopp boost_date_time boost_unit_test_framework boost_random boost_filesystem boost_system)

include_directories(
/usr/include/mysql++
/usr/include/mysql
src/
src/common/
src/common/Include
)

file(GLOB SOURCES 
src/common/*.[ch]pp
src/common/Xmltar/*.[ch]pp
)

file(GLOB TEST_SOURCES 
src/common/Test.[ch]pp
src/common/Tests/Test.*.[ch]pp
)

add_executable(xmltar src/xmltar.cpp ${SOURCES})
set_target_properties( xmltar PROPERTIES RUNTIME_OUTPUT_DIRECTORY .. )
target_link_libraries (xmltar ${LIBS})
