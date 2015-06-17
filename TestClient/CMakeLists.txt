file(GLOB tc_top_SOURCES src/*.*)

include_directories(src/)

add_executable(TestClient ${tc_top_SOURCES})

source_group("TestClient\\" FILES ${tc_top_SOURCES})

target_link_libraries(TestClient Silk)
