#pragma once

#include "../../snuffbox/fmod/fmod_sound_system.h"

namespace snuffbox
{
  class Sound
  {
  public:
    /**
    * @brief Construct a sound from a path
    * @param[in] path (std::string) The relative working directory path
    */
    Sound(std::string path);

    /// Default destructor
    ~Sound();

    /// Returns the actual sound
    FMOD::Sound* get();

  private:
    FMOD::Sound* sound_; //!< The actual sound
  };
}