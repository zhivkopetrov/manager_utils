# manager_utils

## A static C++ library for 2D hardware accelerated widget rendering, animations, input handling, sound playback and system timers
The library is optimized for fast compilation.

It supports:
- 2D widget rendering - Images, Sprites, Frame buffer objects
- 2D animations - Frame, Position, Rotation and Pulse
- Run-time multithreading load & unload of assets
- 2D widget buttons and event handling
- Pausable, resumable and speed-adjustment timers
- 2D music streams & small sound chunks playback in a dedicated audio thread

Library specifics:
- Utilization of the flyweight design pattern, by providing a handle based API frontend.
- The backend is realized under the sdl_utils library, which can be found here: https://github.com/zhivkopetrov/sdl_utils
- The raw API is provided under several singleton implementations.
Those implementation should never be directly used in derived code.
Instead use the library provided wrapper classes.
- Rendeder thread must operate on the main thread to guarantee cross-platform behaviour.
- Simply spawn additional thread and keep the update loop there

Examples:
```
class TileButton final : public ButtonBase {
public:
  void init(uint64_t buttonRsrcId, uint64_t clickSoundRsrcId) {
    ButtonBase::create(buttonRsrcId);
    mClickSound.create(clickSoundRsrcId);
  }

  void handleEvent(const InputEvent& e) override {
    if (TouchEvent::TOUCH_RELEASE == e.type) {
      mClickSound.play();
    }
  }
    
private:
  Sound mClickSound;  
}

class Map {
public:
  void init() {
    mTileButton.create(DevBatttleGui::TILE_BUTTON, DevBatttleGui::CLICK_SOUND);
    mTileSurface.create(DevBatttleGui::TILE_SURFACE);
  }

  void draw() const {
    mTileSurface.draw();
    mTileButton.draw();
  }
  
  void handleEvent(const InputEvent& e) {
    if (mTileButton.isInputUnlocked() && mTileButton.containsEvent(e) {
      mTileButton.handleEvent(e);
    }
  }

private:
  Image mTileSurface;
  TileButton mTileButton;
};

```

## Usage from plain CMake
- Consume directly with find_package(manager_utils) in a CMakeLists.txt
- Link against your target with suitable access modifier
```
target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
        manager_utils::manager_utils
)
```
- Example usage project: https://github.com/zhivkopetrov/dev_battle.git

## Usage as part of ROS1(catkin) / ROS2(colcon) meta-build systems
- Consume directly with find_package(manager_utils) in the package CMakeLists.txt
- Link agains your target
- The library automatically exposes and install it's artifacts following ROS1/ROS2 structure
- Example usage project: https://github.com/zhivkopetrov/robotics_v1

## Dependencies
- cmake_helpers - https://github.com/zhivkopetrov/cmake_helpers.git
- utils - https://github.com/zhivkopetrov/utils
- resource_utils - https://github.com/zhivkopetrov/resource_utils
- sdl_utils - https://github.com/zhivkopetrov/sdl_utils

## Supported Platforms
Linux:
  - g++ (9.3 and above)
    - Tested up to g++ 12.1
  - clang++ (10 and above)
    - Tested up to clang++ 14.0
  - Emscripten (3.1.28 and above)
    - emcc/em++
  - Robot Operating System 2 (ROS2)
    - Through colcon meta-build system (CMake based)
  - Robot Operating System 1 (ROS1)
    - Through catkin meta-build system (CMake based)
      - Due to soon ROS1 end-of-life catkin builds are not actively supported

Windows:
  - MSVC++ (14.20 and above) Visual Studio 2019
    - Tested up to 17.30 Visual Studio 2022
  - Emscripten (3.1.28 and above)
    - emcc/em++
    - NOTE: non-MSVC CMake build generator is needed
      - For example: ninja
  - ROS1/ROS2
    - Although the code is ROS compatible, actual ROS functionalities have not been tested on Windows