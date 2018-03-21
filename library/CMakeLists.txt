project(library)

# sdl
include_directories(${SDL2_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}/external/SDL/include)
# assimp
include_directories(${CMAKE_BINARY_DIR}/external/assimp/include ${CMAKE_SOURCE_DIR}/external/assimp/include)

file(GLOB_RECURSE ALL_FILES *.cpp *.h)
add_library(${PROJECT_NAME} ${LIB_TYPE} ${ALL_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2-static)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC stb)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})