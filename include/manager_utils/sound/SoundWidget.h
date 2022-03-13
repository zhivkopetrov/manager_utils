#ifndef MANAGER_UTILS_SOUNDWIDGET_H_
#define MANAGER_UTILS_SOUNDWIDGET_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/class/NonCopyable.h"

// Own components headers
#include "resource_utils/defines/SoundDefines.h"

// Forward declarations
class SoundWidgetEndCb;

/* Common class for Audio sounds .
 * All Sound related classes must inherit from SoundWidget */
class SoundWidget : public NonCopyable {
 public:
  SoundWidget();
  virtual ~SoundWidget() noexcept = default;

  SoundWidget(SoundWidget&& movedOther);
  SoundWidget& operator=(SoundWidget&& movedOther);

  /** @brief used to create sound resource. This function must be called
   *         in order to operate will the resource. re
   *   This function does not return error code for performance reasons
   *
   *  @param const uint64_t     - unique resource ID
   *  @param SoundWidgetEndCb * - user defined sound/music end callback
   * */
  void create(const uint64_t rsrcId, SoundWidgetEndCb* endCb = nullptr);

  virtual void destroy() = 0;

  virtual void play(const int32_t loops) = 0;

  virtual bool isPlaying() const = 0;

  virtual bool isPaused() const = 0;

  virtual void pause() = 0;

  virtual void resume() = 0;

  virtual void stop() = 0;

  virtual void setVolume(const SoundLevel soundLevel) = 0;

 protected:
  /** @brief used to reset internal variables
   * */
  void reset();

  SoundWidgetEndCb* _endCb;

  uint64_t _rsrcId;

  SoundType _soundType;

  SoundLevel _soundLevel;

  bool _isCreated;

  bool _isDestroyed;
};

#endif /* MANAGER_UTILS_SOUNDWIDGET_H_ */
