#include "../../snuffbox/fmod/fmod_sound_system.h"
#include "../../snuffbox/fmod/fmod_sound.h"
#include "../../snuffbox/content/content_manager.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/js/js_wrapper.h"

namespace snuffbox
{
  namespace environment
  {
    namespace
    {
      SoundSystem* globalInstance = nullptr;
    }

    SoundSystem& sound_system()
    {
      SNUFF_ASSERT_NOTNULL(globalInstance);
      return *globalInstance;
    }

    bool has_sound_system()
    {
      return globalInstance != nullptr;
    }
  }
}

namespace snuffbox
{
  //---------------------------------------------------------------
  SoundSystem::SoundSystem() :
    fmod_system_(nullptr)
  {
    FMOD_RESULT result;

    result = FMOD::System_Create(&fmod_system_);

    SNUFF_XASSERT(result == FMOD_OK, "Failed creating the FMOD sound system!");

    result = fmod_system_->init(512, FMOD_INIT_NORMAL, 0);

    SNUFF_XASSERT(result == FMOD_OK, "Failed initializing the FMOD sound system!");

    unsigned int version = 0;
    result = fmod_system_->getVersion(&version);
    SNUFF_XASSERT(result == FMOD_OK, "Failed retrieving the FMOD version!");

    SNUFF_LOG_INFO(std::string("Succesfully initialized FMOD v" + std::to_string(version)).c_str());

    environment::globalInstance = this;
  }

  //---------------------------------------------------------------
  void SoundSystem::AddChannelGroup(std::string channel)
  {
    if (channel_groups_.find(channel) != channel_groups_.end())
    {
      SNUFF_LOG_ERROR(std::string("Channel with name " + channel + " already exists!").c_str());
      return;
    }

    FMOD_RESULT result;

    FMOD::ChannelGroup* channelGroup;
    result = fmod_system_->createChannelGroup(channel.c_str(), &channelGroup);

    channel_groups_.emplace(channel, channelGroup);
  }

  //---------------------------------------------------------------
  void SoundSystem::Update()
  {
    fmod_system_->update();
  }

  //---------------------------------------------------------------
  FMOD::Sound* SoundSystem::Load(std::string path)
  {
    FMOD::Sound* snd = nullptr;
    FMOD_RESULT result;

    result = fmod_system_->createSound(std::string(environment::js_state_wrapper().path() + "/" + path).c_str(), FMOD_DEFAULT, 0, &snd);
    SNUFF_XASSERT(result == FMOD_OK, "Failed creating sound " + path + "!");

    return snd;
  }

  //---------------------------------------------------------------
  void SoundSystem::Play(std::string path, std::string channelGroup, bool loop)
  {
    Sound* sound = environment::content_manager().Get<Sound>(path).get();

    auto it = channel_groups_.find(channelGroup);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR(std::string("Channel group attempted to play sound on does not exist! Channel: " + channelGroup).c_str());
      return;
    }
    loop == true ? sound->get()->setMode(FMOD_LOOP_NORMAL) : sound->get()->setMode(FMOD_LOOP_OFF);
    fmod_system_->playSound(sound->get(), it->second, false, 0);
  }

  //---------------------------------------------------------------
  void SoundSystem::SetChannelGroupVolume(std::string channelGroup, float volume)
  {
    auto it = channel_groups_.find(channelGroup);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR(std::string("Channel group attempted to play sound on does not exist! Channel: " + channelGroup).c_str());
      return;
    }

    if (volume < 0.0f || volume > 1.0f)
    {
      SNUFF_LOG_WARNING("Volume should be in a 0.0 to 1.0 range, clamping");
      if (volume < 0.0f)
      {
        volume = 0.0f;
      }
      else if (volume > 1.0f)
      {
        volume = 1.0f;
      }
    }

    it->second->setVolume(volume);
  }

  //---------------------------------------------------------------
  void SoundSystem::Stop(std::string channelGroup)
  {
    auto it = channel_groups_.find(channelGroup);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR(std::string("Channel group attempted to play sound on does not exist! Channel: " + channelGroup).c_str());
      return;
    }

    it->second->stop();
  }

  //---------------------------------------------------------------
  void SoundSystem::Pause(std::string channelGroup)
  {
    auto it = channel_groups_.find(channelGroup);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR(std::string("Channel group attempted to play sound on does not exist! Channel: " + channelGroup).c_str());
      return;
    }

    it->second->setPaused(true);
  }

  //---------------------------------------------------------------
  void SoundSystem::Resume(std::string channelGroup)
  {
    auto it = channel_groups_.find(channelGroup);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR(std::string("Channel group attempted to play sound on does not exist! Channel: " + channelGroup).c_str());
      return;
    }

    it->second->setPaused(false);
  }

  //---------------------------------------------------------------
  bool SoundSystem::IsPaused(std::string channelGroup)
  {
    auto it = channel_groups_.find(channelGroup);

    if (it == channel_groups_.end())
    {
      SNUFF_LOG_ERROR(std::string("Channel group attempted to play sound on does not exist! Channel: " + channelGroup).c_str());
      return false;
    }

    bool paused = false;
    it->second->getPaused(&paused);
    return paused;
  }

  //---------------------------------------------------------------
  void SoundSystem::JSResume(JS_ARGS)
  {
    JS_CHECK_PARAMS("S");

    if (!validParams)
    {
      return;
    }

    environment::sound_system().Resume(wrapper.GetString(0));
  }

  //---------------------------------------------------------------
  void SoundSystem::JSIsPaused(JS_ARGS)
  {
    JS_CHECK_PARAMS("S");

    if (!validParams)
    {
      return;
    }

    wrapper.ReturnBool(environment::sound_system().IsPaused(wrapper.GetString(0)));
  }

  //---------------------------------------------------------------
  void SoundSystem::JSStop(JS_ARGS)
  {
    JS_CHECK_PARAMS("S");

    if (!validParams)
    {
      return;
    }

    environment::sound_system().Stop(wrapper.GetString(0));
  }

  //---------------------------------------------------------------
  void SoundSystem::JSPause(JS_ARGS)
  {
    JS_CHECK_PARAMS("S");

    if (!validParams)
    {
      return;
    }

    environment::sound_system().Pause(wrapper.GetString(0));
  }

  //---------------------------------------------------------------
  void SoundSystem::JSPlay(JS_ARGS)
  {
    JS_CHECK_PARAMS("SSB");

    if (!validParams)
    {
      return;
    }

    environment::sound_system().Play(wrapper.GetString(0), wrapper.GetString(1), wrapper.GetBool(2));
  }

  //---------------------------------------------------------------
  void SoundSystem::JSAddChannelGroup(JS_ARGS)
  {
    JS_CHECK_PARAMS("S");

    if (!validParams)
    {
      return;
    }

    environment::sound_system().AddChannelGroup(wrapper.GetString(0));
  }

  //---------------------------------------------------------------
  void SoundSystem::JSSetChannelGroupVolume(JS_ARGS)
  {
    JS_CHECK_PARAMS("SN");

    if (!validParams)
    {
      return;
    }

    environment::sound_system().SetChannelGroupVolume(wrapper.GetString(0), wrapper.GetNumber<float>(1));
  }

  //---------------------------------------------------------------
  void SoundSystem::RegisterJS(JS_TEMPLATE)
  {
    JSFunctionRegister funcs[] = {
      JSFunctionRegister("play", JSPlay),
      JSFunctionRegister("pause", JSPause),
      JSFunctionRegister("stop", JSStop),
      JSFunctionRegister("resume", JSResume),
      JSFunctionRegister("isPaused", JSIsPaused),
      JSFunctionRegister("addChannelGroup", JSAddChannelGroup),
      JSFunctionRegister("setChannelGroupVolume", JSSetChannelGroupVolume)
    };

    JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
  }

  //---------------------------------------------------------------
  SoundSystem::~SoundSystem()
  {
    fmod_system_->release();

    environment::globalInstance = nullptr;
  }
}