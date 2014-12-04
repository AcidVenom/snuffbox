#include "../../snuffbox/fmod/fmod_sound.h"
#include "../../snuffbox/environment.h"

namespace snuffbox
{
  //---------------------------------------------------------------
  Sound::Sound(std::string path)
  {
    sound_ = environment::sound_system().Load(path);
  }

  //---------------------------------------------------------------
  FMOD::Sound* Sound::get()
  {
    return sound_;
  }

  //---------------------------------------------------------------
  Sound::~Sound()
  {
    if (environment::has_sound_system() == true)
    {
      sound_->release();
    }
  }
}