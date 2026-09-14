cmake_minimum_required(VERSION 3.30)
project(LangulusProfiler
   VERSION         1.0.0
   DESCRIPTION     "Langulus profiler utility"
   HOMEPAGE_URL    https://langulus.com
)

# Check if this project is built as standalone, or a part of something else 
if (PROJECT_IS_TOP_LEVEL OR NOT LANGULUS)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
   set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
   set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
   include(LangulusUtilities.cmake)
   fetch_langulus_module(Logger GIT_TAG wip)
endif()

# Build and install Tester library											
add_langulus_library(LangulusProfiler
   SOURCES  $<$<BOOL:${LANGULUS_OPTION_LOGGING}>:$<TARGET_OBJECTS:LangulusLogger>>
            source/Profiler.cpp
)

target_compile_definitions(LangulusProfiler
   PRIVATE  LANGULUS_EXPORT_ALL
)

target_include_directories(LangulusProfiler
   PUBLIC   $<$<BOOL:${LANGULUS_OPTION_LOGGING}>:$<TARGET_PROPERTY:LangulusLogger,INTERFACE_INCLUDE_DIRECTORIES>>
            include
)

target_link_libraries(LangulusProfiler
   PUBLIC   LangulusCore
            $<$<BOOL:${LANGULUS_OPTION_LOGGING}>:fmt>
)