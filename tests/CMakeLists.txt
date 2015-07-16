project( tr064_tests )
cmake_minimum_required( VERSION 2.8 )

#option(tr064_build_tests "Build all of tr064's unit tests." OFF)
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_HAS_TR1_TUPLE=0" )

include_directories(
  SYSTEM             
  "${TR064_SOURCE_DIR}/include"
  "${TR064_SOURCE_DIR}/contrib"
  "${TR064_SOURCE_DIR}/lib"
  )

file( GLOB SOURCES *.cc )

configure_file( ./test_data/wanipconnSCPD.xml ${CMAKE_CURRENT_BINARY_DIR} COPYONLY )

add_executable( ${PROJECT_NAME} ${SOURCES} )
target_link_libraries( ${PROJECT_NAME} 
                        tr064
                        pthread 
                        curlcpp 
                        gmock 
                        gmock_main 
                        gtest 
                        gtest_main)

add_test( tr064-tests ${PROJECT_NAME} )

