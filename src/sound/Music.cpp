// Corresponding header
#include "manager_utils/sound/Music.h"

// System headers
#include <utility>

// Other libraries headers
#include "utils/Log.h"

// Own components headers
#include "manager_utils/managers/SoundMgr.h"

Music::~Music() noexcept {
  // attempt to destroy SoundWidget only if it
  // was first created and not destroyed
  if (true == _isCreated && false == _isDestroyed) {
    destroy();
  }
}

Music::Music(Music &&movedOther)
    : SoundWidget(std::move(movedOther)) {
}

Music& Music::operator=(Music &&movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // implicitly invoke SoundWidget move assignment operator
    SoundWidget::operator=(std::move(movedOther));
  }

  return *this;
}

void Music::destroy() {
  if (_isDestroyed) {
    LOGERR("Warning, trying to destroy already destroyed Music with "
           "rsrcId: %#16lX", _rsrcId);
    return;
  }

  // sanity check
  if (nullptr != gSoundMgr) {
    // check for auto-closing of music
    gSoundMgr->trySelfUnloadMusic(_rsrcId, _soundLevel);
  }

  // invoke base destroy
  SoundWidget::destroy();
}

void Music::setVolume(const SoundLevel soundLevel) {
  if (SoundLevel::UNKNOWN == soundLevel) {
    LOGERR("Error, UNKNOWN soundLevel value detected. "
           "Will not change volume value.");
    return;
  }

  _soundLevel = soundLevel;
  gSoundMgr->setMusicVolume(_soundLevel);
}

void Music::play(const int32_t loops) {
  gSoundMgr->playLoadedMusic(loops, _endCb);
}

bool Music::isPlaying() const {
  return gSoundMgr->isMusicPlaying();
}

void Music::stop() {
  gSoundMgr->stopLoadedMusic();
}

bool Music::isPaused() const {
  return gSoundMgr->isMusicPaused();
}

void Music::pause() {
  gSoundMgr->pauseMusic();
}

void Music::resume() {
  gSoundMgr->resumeMusic();
}

void Music::loadMusic() {
  gSoundMgr->loadMusic(_rsrcId);
}

void Music::unloadMusic() {
  gSoundMgr->trySelfUnloadMusic(_rsrcId, _soundLevel);
}

void Music::rewind() {
  gSoundMgr->rewindMusic();
}
