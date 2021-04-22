cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 11)

project(sort-cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

if (NOT APPLE)
    # This help to find OpenCV and Eigen, some how cmake on
    # Windows doesn't use ENV variables so I had to re-set it here
    set(OpenCV_DIR $ENV{OpenCV_DIR})
    set(EIGEN3_INCLUDE_DIR $ENV{EIGEN3_INCLUDE_DIR})
endif()

# Try to find OpenCV
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    # If the package has been found, several variables will
    # be set, you can find the full list with descriptions
    # in the OpenCVConfig.cmake file.
    # Print some message showing some of them
    message(STATUS "OpenCV library status:")
    message(STATUS "    version: ${OpenCV_VERSION}")
    message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "Could not locate OpenCV")
endif()

if(APPLE)
    # Trying to include path to Eigen that was installed by brew
    include_directories("/usr/local/include")
elseif(WIN32)
    include_directories(${EIGEN3_INCLUDE_DIR})
endif()
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_JS_INC})

file(GLOB SOURCE_FILES src/*.cpp src/*.cc src/*.h)


add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
# add_executable(${CMAKE_PROJECT_NAME}-bin ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries (
    ${CMAKE_PROJECT_NAME}
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})

# Include Node-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

# define NPI_VERSION
add_definitions(-DNAPI_VERSION=3)