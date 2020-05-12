#author Zhivko Petrov

cmake_minimum_required(VERSION 3.10.2)

if(TARGET manager_utils)
  return()
endif()

if(NOT CMAKE_HELPERS_INCLUDED)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_helpers/helpers.cmake)
endif()

project(manager_utils)
   
set(_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/manager_utils")       
set(_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/manager_utils")

add_library(
    ${PROJECT_NAME} 
    STATIC
        "${_INCLUDE_DIR}/drawing/CreditRotation.h"
        "${_INCLUDE_DIR}/drawing/DynamicImage.h"
        "${_INCLUDE_DIR}/drawing/Image.h"
        "${_INCLUDE_DIR}/drawing/Sprite.h"
        "${_INCLUDE_DIR}/drawing/SpriteBuffer.h"
        "${_INCLUDE_DIR}/drawing/Text.h"
        "${_INCLUDE_DIR}/drawing/Widget.h"
        "${_INCLUDE_DIR}/drawing/animation/AnimationBase.h"
        "${_INCLUDE_DIR}/drawing/animation/AnimationEndCb.hpp"
        "${_INCLUDE_DIR}/drawing/animation/FrameAnimation.h"
        "${_INCLUDE_DIR}/drawing/animation/PositionAnimation.h"
        "${_INCLUDE_DIR}/drawing/animation/PulseAnimation.h"
        "${_INCLUDE_DIR}/drawing/animation/RotationAnimation.h"
        "${_INCLUDE_DIR}/drawing/animation/defines/AnimationDefines.h"
        "${_INCLUDE_DIR}/drawing/animation/defines/CommonAnimationStructs.h"
        "${_INCLUDE_DIR}/input/ButtonBase.h"
        "${_INCLUDE_DIR}/input/Scroller.h"
        "${_INCLUDE_DIR}/managers_base/MgrBase.h"
        "${_INCLUDE_DIR}/managers_base/DrawMgrBase.h"
        "${_INCLUDE_DIR}/managers_base/RsrcMgrBase.h"
        "${_INCLUDE_DIR}/managers_base/SoundMgrBase.h"
        "${_INCLUDE_DIR}/managers_base/TimerMgrBase.h"
        "${_INCLUDE_DIR}/sound/Music.h"
        "${_INCLUDE_DIR}/sound/Sound.h"
        "${_INCLUDE_DIR}/sound/SoundWidget.h"
        "${_INCLUDE_DIR}/sound/SoundWidgetEndCb.hpp"
        "${_INCLUDE_DIR}/time/TimerClient.h"
        "${_INCLUDE_DIR}/time/TimerClientSpeedAdjustable.h"
        "${_INCLUDE_DIR}/time/UserTimerClient.h"
        "${_INCLUDE_DIR}/time/UserTimerClient.h"
        "${_INCLUDE_DIR}/time/defines/TimerClientDefines.h"
    
        "${_SRC_DIR}/drawing/CreditRotation.cpp"
        "${_SRC_DIR}/drawing/DynamicImage.cpp"
        "${_SRC_DIR}/drawing/Image.cpp"
        "${_SRC_DIR}/drawing/Sprite.cpp"
        "${_SRC_DIR}/drawing/SpriteBuffer.cpp"
        "${_SRC_DIR}/drawing/Text.cpp"
        "${_SRC_DIR}/drawing/Widget.cpp"
        "${_SRC_DIR}/drawing/animation/AnimationBase.cpp"
        "${_SRC_DIR}/drawing/animation/FrameAnimation.cpp"
        "${_SRC_DIR}/drawing/animation/PositionAnimation.cpp"
        "${_SRC_DIR}/drawing/animation/PulseAnimation.cpp"
        "${_SRC_DIR}/drawing/animation/RotationAnimation.cpp"
        "${_SRC_DIR}/drawing/animation/defines/CommonAnimationStructs.cpp"
        "${_SRC_DIR}/input/ButtonBase.cpp"
        "${_SRC_DIR}/input/Scroller.cpp"
        "${_SRC_DIR}/managers_base/DrawMgrBase.cpp"
        "${_SRC_DIR}/managers_base/RsrcMgrBase.cpp"
        "${_SRC_DIR}/managers_base/SoundMgrBase.cpp"
        "${_SRC_DIR}/managers_base/TimerMgrBase.cpp"
        "${_SRC_DIR}/sound/Music.cpp"
        "${_SRC_DIR}/sound/Sound.cpp"
        "${_SRC_DIR}/sound/SoundWidget.cpp"
        "${_SRC_DIR}/time/TimerClient.cpp"
        "${_SRC_DIR}/time/TimerClientSpeedAdjustable.cpp"
        "${_SRC_DIR}/time/UserTimerClient.cpp"
)

set_target_cpp_standard(${PROJECT_NAME} 17)
enable_target_warnings(${PROJECT_NAME})
set_target_visibility(${PROJECT_NAME})

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        VERSION 1.0.0
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(NOT TARGET sdl_utils)
    add_subdirectory(sdl_utils)
endif()

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        utils
        resource_utils
        sdl_utils
)

#invoke include-what-you-use (if it's enabled) for the current project
if(USE_IWYU)
    enable_target_include_what_you_use(${PROJECT_NAME})
endif(USE_IWYU)

if(BUILD_LIKE_POSITION_INDEPENDANT_CODE)
    enable_target_position_independent_code(${PROJECT_NAME})
endif()  




          



