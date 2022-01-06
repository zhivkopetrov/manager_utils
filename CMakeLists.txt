#author Zhivko Petrov

cmake_minimum_required(VERSION 3.10.2)

project(manager_utils)
   
set(_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/manager_utils)       
set(_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(
    ${PROJECT_NAME} 
    STATIC
        ${_INC_DIR}/drawing/CreditRotation.h
        ${_INC_DIR}/drawing/DynamicImage.h
        ${_INC_DIR}/drawing/Image.h
        ${_INC_DIR}/drawing/Sprite.h
        ${_INC_DIR}/drawing/SpriteBuffer.h
        ${_INC_DIR}/drawing/Text.h
        ${_INC_DIR}/drawing/Widget.h
        ${_INC_DIR}/drawing/animation/AnimationBase.h
        ${_INC_DIR}/drawing/animation/AnimationEndCb.hpp
        ${_INC_DIR}/drawing/animation/FrameAnimation.h
        ${_INC_DIR}/drawing/animation/PositionAnimation.h
        ${_INC_DIR}/drawing/animation/PulseAnimation.h
        ${_INC_DIR}/drawing/animation/RotationAnimation.h
        ${_INC_DIR}/drawing/animation/defines/AnimationDefines.h
        ${_INC_DIR}/drawing/animation/defines/CommonAnimationStructs.h
        ${_INC_DIR}/input/ButtonBase.h
        ${_INC_DIR}/input/Scroller.h
        ${_INC_DIR}/managers/config/ManagerHandlerConfig.hpp
        ${_INC_DIR}/managers/config/DrawMgrConfig.hpp
        ${_INC_DIR}/managers/ManagerHandler.h
        ${_INC_DIR}/managers/MgrBase.h
        ${_INC_DIR}/managers/DrawMgr.h
        ${_INC_DIR}/managers/RsrcMgr.h
        ${_INC_DIR}/managers/SoundMgr.h
        ${_INC_DIR}/managers/TimerMgr.h
        ${_INC_DIR}/sound/Music.h
        ${_INC_DIR}/sound/Sound.h
        ${_INC_DIR}/sound/SoundWidget.h
        ${_INC_DIR}/sound/SoundWidgetEndCb.hpp
        ${_INC_DIR}/time/TimerClient.h
        ${_INC_DIR}/time/TimerClientSpeedAdjustable.h
        ${_INC_DIR}/time/UserTimerClient.h
        ${_INC_DIR}/time/UserTimerClient.h
        ${_INC_DIR}/time/defines/TimerClientDefines.h
    
        ${_SRC_DIR}/drawing/CreditRotation.cpp
        ${_SRC_DIR}/drawing/DynamicImage.cpp
        ${_SRC_DIR}/drawing/Image.cpp
        ${_SRC_DIR}/drawing/Sprite.cpp
        ${_SRC_DIR}/drawing/SpriteBuffer.cpp
        ${_SRC_DIR}/drawing/Text.cpp
        ${_SRC_DIR}/drawing/Widget.cpp
        ${_SRC_DIR}/drawing/animation/AnimationBase.cpp
        ${_SRC_DIR}/drawing/animation/FrameAnimation.cpp
        ${_SRC_DIR}/drawing/animation/PositionAnimation.cpp
        ${_SRC_DIR}/drawing/animation/PulseAnimation.cpp
        ${_SRC_DIR}/drawing/animation/RotationAnimation.cpp
        ${_SRC_DIR}/drawing/animation/defines/CommonAnimationStructs.cpp
        ${_SRC_DIR}/input/ButtonBase.cpp
        ${_SRC_DIR}/input/Scroller.cpp
        ${_SRC_DIR}/managers/ManagerHandler.cpp
        ${_SRC_DIR}/managers/DrawMgr.cpp
        ${_SRC_DIR}/managers/RsrcMgr.cpp
        ${_SRC_DIR}/managers/SoundMgr.cpp
        ${_SRC_DIR}/managers/TimerMgr.cpp
        ${_SRC_DIR}/sound/Music.cpp
        ${_SRC_DIR}/sound/Sound.cpp
        ${_SRC_DIR}/sound/SoundWidget.cpp
        ${_SRC_DIR}/time/TimerClient.cpp
        ${_SRC_DIR}/time/TimerClientSpeedAdjustable.cpp
        ${_SRC_DIR}/time/UserTimerClient.cpp
)

set_target_cpp_standard(${PROJECT_NAME} 20)
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

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        sdl_utils
)

#invoke include-what-you-use (if it's enabled) for the current project
if(USE_IWYU)
    enable_target_include_what_you_use(${PROJECT_NAME})
endif(USE_IWYU)

if(BUILD_LIKE_POSITION_INDEPENDANT_CODE)
    enable_target_position_independent_code(${PROJECT_NAME})
endif()  




          



