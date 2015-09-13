file(GLOB tc_top_SOURCES src/*.*)
file(GLOB tc_t4_SOURCES src/T4/*.*)
file(GLOB tc_ui_SOURCES src/UIElements/*.*)
file(GLOB tc_soil_SOURCES src/T4/SOIL/*.*)

include_directories(src/)

add_executable(TestClient ${tc_top_SOURCES} ${tc_t4_SOURCES} ${tc_soil_SOURCES} ${tc_ui_SOURCES})

source_group("TestClient" FILES ${tc_top_SOURCES})
source_group("TestClient\\T4" FILES ${tc_t4_SOURCES})
source_group("TestClient\\T4\\SOIL" FILES ${tc_soil_SOURCES})
source_group("TestClient\\UIElements" FILES ${tc_ui_SOURCES})

target_link_libraries(TestClient Silk Glfw ${GLFW_LIBRARIES})
