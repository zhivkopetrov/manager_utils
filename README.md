# manager_utils

**A static C++ library for 2D hardware accelerated widget rendering, input handling, sound playback and system timers**

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
- The library is optimized for fast compilation.

Examples:
```
class TileButton : public ButtonBase {
public:
  void init(uint64_t buttonRsrcId, uint64_t clickSoundRsrcId) {
    ButtonBase::create(buttonRsrcId);
    mClickSound.create(clickSoundRsrcId);
  }

  void handleEvent(const InputEvent& e) {
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

**Usage from plain CMake**
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


**Usage as part of ROS(catkin) / ROS2(colcon) meta-build systems**
- Consume directly with find_package(manager_utils) in the package CMakeLists.txt
- Link agains your target
- The library automatically exposes and install it's artifacts following ROS/ROS2 structure
- Example usage project: https://github.com/zhivkopetrov/robotics_v1


**Dependencies**
- cmake_helpers - https://github.com/zhivkopetrov/cmake_helpers.git
- The provided library CMakeLists.txt assumes the helpers.cmake from cmake_helpers repo have already been included
- sdl_utils - https://github.com/zhivkopetrov/sdl_utils_utils


**Platform support**
- Tested on Unix, Windows
- Never tested on Mac