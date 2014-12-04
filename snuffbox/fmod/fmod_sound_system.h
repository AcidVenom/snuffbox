#pragma once

#include <fmod.hpp>

#include "../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
  class Sound;

  /**
  * @class snuffbox::SoundSystem
  * @brief The sound system that handles all sound events, sound banks, etc. of FMOD
  * @author Daniël Konings
  */
  class SoundSystem : public JSObject
  {
  public:

    /// Default constructor
    SoundSystem();

    /// Default destructor
    ~SoundSystem();

    /// Updates the sound system
    void Update();

    /// Loads a sound from a path and returns it
    FMOD::Sound* Load(std::string path);

    /// Plays a sound
    void Play(std::string path, std::string channelGroup, bool loop = false);

    /// Adds a channel
    void AddChannelGroup(std::string channelGroup);

    /// Sets the volume of a channel group
    void SetChannelGroupVolume(std::string channelGroup, float volume);

    /// Stops a channel group
    void Stop(std::string channelGroup);

    /// Pauses a channel group
    void Pause(std::string channelGroup);

    /// Resumes the sound of a channel group
    void Resume(std::string channelGroup);

    /// Is this channel group paused?
    bool IsPaused(std::string channelGroup);

  private:
    FMOD::System* fmod_system_; //!< The actual FMOD system
    std::map<std::string, FMOD::ChannelGroup*> channel_groups_; //!< All channel groups

  public:
    JS_NAME(SoundSystem);

    static void RegisterJS(JS_TEMPLATE);
    static void JSPlay(JS_ARGS);
    static void JSPause(JS_ARGS);
    static void JSIsPaused(JS_ARGS);
    static void JSResume(JS_ARGS);
    static void JSStop(JS_ARGS);
    static void JSAddChannelGroup(JS_ARGS);
    static void JSSetChannelGroupVolume(JS_ARGS);
  };
}