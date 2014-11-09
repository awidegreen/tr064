include_directories("${TR064_SOURCE_DIR}/include")
include_directories("${TR064_SOURCE_DIR}/contrib")

add_executable(tr064tool tr064.cc)
add_executable(tr064export tr064export.cc)

# tr064 is the internal library
target_link_libraries(tr064tool tr064 pthread)

target_link_libraries(tr064export tr064 pthread)

install(TARGETS tr064tool DESTINATION bin)
install(TARGETS tr064export DESTINATION bin)

