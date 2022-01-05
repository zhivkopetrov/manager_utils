// Corresponding header
#include "manager_utils/sound/Music.h"

// C system headers

// C++ system headers
#include <utility>

// Other libraries headers

// Own components headers
#include "manager_utils/managers_base/SoundMgrBase.h"

#include "utils/Log.h"

Music::~Music() {
  // attempt to destroy SoundWidget only if it
  // was first created and not destroyed
  if (true == _isCreated && false == _isDestroyed) {
    destroy();
  }
}

// move constructor
Music::Music(Music&& movedOther) : SoundWidget(std::move(movedOther)) {}

// move assignment operator implementation
Music& Music::operator=(Music&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // implicitly invoke SoundWidget move assignment operator
    SoundWidget::operator=(std::move(movedOther));
  }

  return *this;
}

void Music::destroy() {
  if (_isDestroyed) {
    LOGERR(
        "Warning, trying to destroy already destroyed Music with "
        "_rsrcId: %#16lX",
        _rsrcId);
    return;
  }

  // sanity check
  if (nullptr != gSoundMgrBase) {
    // check for auto-closing of music
    gSoundMgrBase->trySelfUnloadMusic(_rsrcId, _soundLevel);
  }

  // invoke base destroy
  SoundWidget::destroy();
}

void Music::setVolume(const SoundLevel soundLevel) {
  if (SoundLevel::UNKNOWN == soundLevel) {
    LOGERR(
        "Error, UNKNOWN soundLevel value detected. "
        "Will not change volume value.");
    return;
  } else  // it is valid SoundLevel value
  {
    _soundLevel = soundLevel;
    gSoundMgrBase->setMusicVolume(_soundLevel);
  }
}

void Music::play(const int32_t loops) {
  gSoundMgrBase->playLoadedMusic(loops, _endCb);
}

bool Music::isPlaying() const { return gSoundMgrBase->isMusicPlaying(); }

void Music::stop() { gSoundMgrBase->stopLoadedMusic(); }

bool Music::isPaused() const { return gSoundMgrBase->isMusicPaused(); }

void Music::pause() { gSoundMgrBase->pauseMusic(); }

void Music::resume() { gSoundMgrBase->resumeMusic(); }

void Music::loadMusic() { gSoundMgrBase->loadMusic(_rsrcId); }

void Music::unloadMusic() {
  gSoundMgrBase->trySelfUnloadMusic(_rsrcId, _soundLevel);
}

void Music::rewind() { gSoundMgrBase->rewindMusic(); }
