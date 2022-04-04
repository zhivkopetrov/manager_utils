include(CMakeFindDependencyMacro)

#find_dependency will correctly forward REQUIRED or QUIET args to the consumer
#find_package is only for internal use
find_dependency(cmake_helpers REQUIRED)
find_dependency(sdl_utils REQUIRED)

if(NOT TARGET manager_utils::manager_utils)
  include(${CMAKE_CURRENT_LIST_DIR}/manager_utilsTargets.cmake)
endif()

# This is for catkin compatibility.
set(manager_utils_LIBRARIES manager_utils::manager_utils)

get_target_property(
    manager_utils_INCLUDE_DIRS
    manager_utils::manager_utils
    INTERFACE_INCLUDE_DIRECTORIES
)

