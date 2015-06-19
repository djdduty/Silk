file(GLOB ss_top_SOURCES src/*.*)

file(GLOB ss_rast_SOURCES src/Raster/*.*)
file(GLOB ss_gl_SOURCES src/Raster/OpenGL/*.*)
file(GLOB ss_rend_SOURCES src/Renderer/*.*)
file(GLOB ss_util_SOURCES src/Utilities/*.*)

file(GLOB ss_math_SOURCES src/Math/*.*)
file(GLOB ss_sys_SOURCES src/System/*.*)

if(BUILD_STATIC)
    set(BUILD_MODE STATIC)
else()
    set(BUILD_MODE SHARED)
endif()

add_library(Silk ${BUILD_MODE} ${ss_top_SOURCES} ${ss_sys_SOURCES} ${ss_gl_SOURCES} ${ss_rast_SOURCES} ${ss_rend_SOURCES} ${ss_util_SOURCES} ${ss_math_SOURCES})

target_link_libraries(Silk Glew ${OPENGL_LIBRARIES})

source_group("Silk" FILES ${ss_top_SOURCES})
source_group("Silk\\System" FILES ${ss_sys_SOURCES})
source_group("Silk\\Renderer" FILES ${ss_rend_SOURCES})
source_group("Silk\\Utilities" FILES ${ss_util_SOURCES})

source_group("Silk\\Raster" FILES ${ss_rast_SOURCES})
source_group("Silk\\Raster\\OpenGL" FILES ${ss_gl_SOURCES})
source_group("Silk\\Renderer" FILES ${ss_rend_SOURCES})

source_group("Silk\\Math" FILES ${ss_math_SOURCES})

#install(TARGETS Silk RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
