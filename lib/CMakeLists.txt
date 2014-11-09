file(GLOB SOURCES
  "*.cc"
  "*.hh"
  "net/*.cc"
  "net/*.hh"
  "serialize/*.cc"
  "serialize/*.hh"
  "soap/*.cc"
  "soap/*.hh"
  )
add_library(tr064 SHARED $<TARGET_OBJECTS:PugiXml> ${SOURCES})
#add_library(tr064 ${SOURCES})

include_directories("${TR064_SOURCE_DIR}/include")
include_directories("${TR064_SOURCE_DIR}/contrib")
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

if (OPENSSL_LIBRARIES)
  message(STATUS "OPENSSL_LIBRARIES defined.")
else (OPENSSL_LIBRARIES)
  message(FATAL_ERROR "OPENSSL_LIBRARIES not defined!")
endif()

if (OPENSSL_INCLUDE_DIR)
  message(STATUS "OPENSSL_INCLUDE_DIR defined.")
else ()
  message(FATAL_ERROR "OPENSSL_INCLUDE_DIR not defined!")
endif()

target_link_libraries(tr064 ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

install(TARGETS tr064 DESTINATION lib)
