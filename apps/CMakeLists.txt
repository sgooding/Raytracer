
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.h")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")
add_executable(raytracer ${HEADER_LIST} ${SOURCE_LIST})

target_link_libraries(raytracer PRIVATE ann)
