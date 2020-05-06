#ifndef MANAGER_UTILS_ANIMATIONBASE_H_
#define MANAGER_UTILS_ANIMATIONBASE_H_

/*
 * AnimationBase.h
 *
 *
 *  Brief: A common class that every type of animation should inherit from.
 *
 *         NOTE: Animations could be reconfigured and used again without the
 *               need to destroy the animation object;
 *
 *         Example:
 *               FrameAnimation anim;
 *               anim.configure(someCfg, &someAnimationEndCb);
 *               anim.start();
 *
 *               anim.stop();
 *
 *               anim.configure(someOtherCfg, &someOtherAnimationEndCg);
 *               anim.start();
 */

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "manager_utils/time/TimerClient.h"
#include "manager_utils/drawing/animation/defines/CommonAnimationStructs.h"
#include "manager_utils/drawing/Image.h"
#include "sdl_utils/drawing/defines/DrawConstants.h"

// Forward Declarations
class AnimationEndCb;
class SpriteBuffer;

class AnimationBase : public TimerClient {
 public:
  AnimationBase();
  virtual ~AnimationBase() = default;

  // move constructor needed for STL containers empalce_back/push_back
  AnimationBase(AnimationBase&& movedOther);

  // move assignment operator implementation
  AnimationBase& operator=(AnimationBase&& movedOther);

  // forbid the copy constructor and copy assignment operator
  AnimationBase(const AnimationBase& other) = delete;
  AnimationBase& operator=(const AnimationBase& other) = delete;

  /** @brief used to start the loaded/configured animation.
   * */
  virtual void start() = 0;

  /** @brief used to manually stop a loaded/configured animation.
   * */
  virtual void stop() = 0;

  /** @brief used to reset a loaded/configured animation internals so it
   *         can be started again from the start without the need of
   *                                                         reconfigure.
   * */
  virtual void reset() = 0;

  /** @brief change the endhandler of the animation
   *
   *  @param AnimationEndCb* - the new end handler of the animation
   * */
  inline void setEndHandler(AnimationEndCb* endCb) { _endCb = endCb; }

  /** @brief used to hide animation - so it will not be drawn
   * */
  inline void hideAnimation() { _isVisible = false; }

  /** @brief used to hide animation - so it will not be drawn
   * */
  inline void showAnimation() { _isVisible = true; }

  /** @brief used to check whether animation is visible or not
   *
   *  @return bool - is animation active or not
   * */
  inline bool isVisible() const { return _isVisible; }

  /**
   * @brief change the opacity of the animating image
   *
   * @param const int32_t opacity -
   */
  inline void setAnimOpacity(const int32_t opacity) {
    _img.setOpacity(opacity);
  }

  /** @brief used to check whether the current animation is active
   *         Note: function polls if the animations timer is active.
   *
   *  @return bool - is animation active or not
   * */
  bool isAnimationActive() const;

  /** @brief used to make draw calls
   * */
  void draw();

  /** @brief used to make draw calls on top of SpriteBuffer
   *
   *  @param SpriteBuffer & - sprite buffer instance
   * */
  void drawOnSpriteBuffer(SpriteBuffer& spriteBuffer);

  /**
   * @brief function to allow the animation fading in and out effect
   */
  inline void activateAnimationAlphaModulation() {
    _img.activateAlphaModulation();
  }

  inline int32_t getWidth() const { return _img.getFrameWidth(); }

  inline int32_t getHeight() const { return _img.getFrameHeight(); }

  inline int32_t getX() const { return _img.getX(); }

  inline int32_t getY() const { return _img.getY(); }

  inline Point getPosition() const { return _img.getPosition(); }

  inline void setPosition(const int32_t x, const int32_t y) {
    _img.setPosition(x, y);
    _cfg.startPos.x = x;
    _cfg.startPos.y = y;
  }

  inline void setX(const int32_t x) {
    _img.setX(x);
    _cfg.startPos.x = x;
  }

  inline void setY(const int32_t y) {
    _img.setY(y);
    _cfg.startPos.y = y;
  }

  inline void setPosition(const Point& pos) {
    _img.setPosition(pos);
    _cfg.startPos = pos;
  }

  inline void moveRight(const int32_t dX) {
    _img.moveRight(dX);
    _cfg.startPos.x += dX;
  }

  inline void moveLeft(const int32_t dX) {
    _img.moveLeft(dX);
    _cfg.startPos.x -= dX;
  }

  inline void moveUp(const int32_t dY) {
    _img.moveUp(dY);
    _cfg.startPos.y -= dY;
  }

  inline void moveDown(const int32_t dY) {
    _img.moveDown(dY);
    _cfg.startPos.y += dY;
  }

  inline void setFrame(const int32_t frameIdx) { _img.setFrame(frameIdx); }

  inline int32_t getFrame() const { return _img.getFrame(); }

  inline int32_t getFrameCount() const { return _img.getFrameCount(); }

  /** @brief used to set external crop rectangle for the underlying
   *         animation images
   *
   *  @param const Rectangle & - image crop rectangle
   * */
  inline void setAnimCropRectangle(const Rectangle& cropRect) {
    _img.setCropRect(cropRect);
  }

 protected:
  /** @brief used to configure animation before you need to start it.
   *         NOTE: you can reset the animation internals by calling
   *         configure again.
   *
   *  @param const AnimBaseConfig& - animation configuration
   *  @param AnimationEndCb*       - animation End callback
   *
   *  @return int32_t              - error code
   * */
  int32_t configureInternal(const AnimBaseConfig& cfg, AnimationEndCb* endCb);

  /** @brief used to completely reset AnimationBase internal variables
   * */
  void resetConfigInternal();

  /** Base configuration for all animations */
  AnimBaseConfig _cfg;

  /** Base end callback for all animations */
  AnimationEndCb* _endCb;

  /** Common image for all animations */
  Image _img;

  /** Holds active/disabled state of animation - so no unnecessary
   *  renderer draw calls are made
   * */
  bool _isVisible;

  /** Holds the status of complete configuration. Unconfigured animation
   *  should not be able to be started.
   * */
  bool _isCfgComplete;
};

#endif /* MANAGER_UTILS_ANIMATIONBASE_H_ */
