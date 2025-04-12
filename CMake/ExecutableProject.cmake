MACRO(CreateExecutableProject ProjectName)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    file(GLOB_RECURSE SOURCES "Source/*.cpp")
    file(GLOB_RECURSE HEADERS "Source/*.h")

    add_executable(${ProjectName} ${SOURCES} ${HEADERS})

    target_link_libraries(${ProjectName} PRIVATE Shared)
ENDMACRO()