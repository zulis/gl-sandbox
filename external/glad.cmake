add_library(
        glad
        STATIC
        ${CMAKE_CURRENT_LIST_DIR}/glad/src/glad.c
)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_LIST_DIR}/glad/include)
set_target_properties(glad PROPERTIES FOLDER "glad")
