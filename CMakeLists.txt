#author Zhivko Petrov

cmake_minimum_required(VERSION 3.10.2)

if(TARGET manager_utils)
  return()
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_helpers/helpers.cmake)

project(manager_utils)
            
set(_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/manager_utils")

add_library(
    ${PROJECT_NAME} 
    STATIC
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

add_subdirectory(sdl_utils)

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




          



