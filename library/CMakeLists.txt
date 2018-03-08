#[[project(library)

# sdl
include_directories(${SDL2_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}/external/SDL/include)
# glm
include_directories(${CMAKE_SOURCE_DIR}/external/glm)

file(GLOB_RECURSE ALL_FILES *.cpp *.h)
add_library(${PROJECT_NAME} ${LIB_TYPE} ${ALL_FILES})
set(DEPENDS SDL2-static gl3w glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})]]



project(library)

# sdl
include_directories(${SDL2_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}/external/SDL/include)

file(GLOB_RECURSE ALL_FILES *.cpp *.h)
add_library(${PROJECT_NAME} ${LIB_TYPE} ${ALL_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2-static)
#target_link_libraries(${PROJECT_NAME} PUBLIC gl3w)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})