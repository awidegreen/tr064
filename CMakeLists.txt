cmake_minimum_required (VERSION 3.0)
project(TR064)

# The version number.
set (TR064_VERSION_MAJOR 0)
set (TR064_VERSION_MINOR 1)

ADD_DEFINITIONS(
    -std=c++11 
    # Other flags
)
                  

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTR064_DEBUG=1")
#if (CMAKE_BUILD_TYPE matches Debug)
  #set (CMAKE_CXX_FLAGS TR064_DEBUG "1" )


find_package(OpenSSL REQUIRED COMPONENTS system)
if (NOT OPENSSL_FOUND)
  message(FATAL_ERROR "OpenSSL NOT found.")
endif()

find_package(Boost REQUIRED COMPONENTS system)
if (NOT Boost_FOUND)
  message(FATAL_ERROR "Boost NOT found.")
endif()

add_subdirectory(lib) 
add_subdirectory(src) 
add_subdirectory(man) 
add_subdirectory(contrib) 

find_package(GTest)
if (GTEST_FOUND)
  message(STATUS "Found Gtest, including unit tests.")
  enable_testing()
  add_subdirectory(tests) 
  #add_test( AllTr064Tests tr064_tests  )
else()
  message(STATUS "Found NOT Gtest, unit tests not included in build.")
endif()

  
