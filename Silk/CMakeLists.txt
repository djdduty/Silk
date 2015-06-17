file(GLOB ss_top_SOURCES src/*.*)

file(GLOB ss_rast_SOURCES src/Raster/*.*)

file(GLOB ss_math_SOURCES src/Math/*.*)

if(BUILD_STATIC)
    set(BUILD_MODE STATIC)
else()
    set(BUILD_MODE SHARED)
endif()

add_library(Silk ${BUILD_MODE} ${ss_top_SOURCES} ${ss_rast_SOURCES} ${ss_math_SOURCES})

target_link_libraries(Silk Glew Glfw ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})

source_group("Silk" FILES ${ss_top_SOURCES})

source_group("Silk\\Rasterization" FILES ${ss_rast_SOURCES})

source_group("Silk\\Math" FILES ${ss_math_SOURCES})

#install(TARGETS Silk RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
