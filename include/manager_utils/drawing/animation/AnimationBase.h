#ifndef MANAGER_UTILS_ANIMATIONBASE_H_
#define MANAGER_UTILS_ANIMATIONBASE_H_

/*
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

// System headers

// Other libraries headers
#include "sdl_utils/drawing/defines/DrawConstants.h"
#include "utils/ErrorCode.h"

// Own components headers
#include "manager_utils/time/TimerClient.h"
#include "manager_utils/drawing/animation/defines/CommonAnimationStructs.h"
#include "manager_utils/drawing/Image.h"

// Forward Declarations
class AnimationEndCb;
class Fbo;

class AnimationBase : public TimerClient {
 public:
  AnimationBase();
  virtual ~AnimationBase() noexcept;

  AnimationBase(AnimationBase&& movedOther);
  AnimationBase& operator=(AnimationBase&& movedOther);

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
  void setEndHandler(AnimationEndCb* endCb) { _endCb = endCb; }

  /** @brief used to hide animation - so it will not be drawn
   * */
  void hideAnimation() { _isVisible = false; }

  /** @brief used to hide animation - so it will not be drawn
   * */
  void showAnimation() { _isVisible = true; }

  /** @brief used to check whether animation is visible or not
   *
   *  @return bool - is animation active or not
   * */
  bool isVisible() const { return _isVisible; }

  /**
   * @brief change the opacity of the animating image
   *
   * @param const int32_t opacity -
   */
  void setAnimOpacity(const int32_t opacity);

  /** @brief used to check whether the current animation is active
   *         Note: function polls if the animations timer is active.
   *
   *  @return bool - is animation active or not
   * */
  bool isAnimationActive() const;

  /** @brief used to make draw calls
   * */
  void draw() const;

  /** @brief used to make draw calls on top of Fbo
   *
   *  @param Fbo & - frame buffer object instance
   * */
  void drawOnFbo(Fbo& fbo) const;

  /**
   * @brief function to allow the animation fading in and out effect
   */
  void activateAnimationAlphaModulation();

  int32_t getWidth() const { return _img->getFrameWidth(); }

  int32_t getHeight() const { return _img->getFrameHeight(); }

  int32_t getX() const { return _img->getX(); }

  int32_t getY() const { return _img->getY(); }

  Point getPosition() const { return _img->getPosition(); }

  void setPosition(const int32_t x, const int32_t y);

  void setX(const int32_t x);

  void setY(const int32_t y);

  void setPosition(const Point& pos);

  void moveRight(const int32_t dX);

  void moveLeft(const int32_t dX);

  void moveUp(const int32_t dY);

  void moveDown(const int32_t dY);

  void setFrame(const int32_t frameIdx);

  int32_t getFrame() const { return _img->getFrame(); }

  int32_t getFrameCount() const { return _img->getFrameCount(); }

  /** @brief used to set external crop rectangle for the underlying
   *         animation images
   *
   *  @param const Rectangle & - image crop rectangle
   * */
  void setAnimCropRectangle(const Rectangle& cropRect);

 protected:
  /** @brief used to configure animation before you need to start it.
   *         NOTE: you can reset the animation internals by calling
   *         configure again.
   *
   *  @param const AnimBaseConfig& - animation configuration
   *  @param AnimationEndCb*       - animation End callback
   *
   *  @return ErrorCode            - error code
   * */
  ErrorCode configureInternal(const AnimBaseConfig& cfg, AnimationEndCb* endCb);

  /** @brief used to completely reset AnimationBase internal variables
   * */
  void resetConfigInternal();

  /** Base configuration for all animations */
  AnimBaseConfig _cfg;

  /** Common image for all animations */
  Image* _img;

  /** Base end callback for all animations */
  AnimationEndCb* _endCb;

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
