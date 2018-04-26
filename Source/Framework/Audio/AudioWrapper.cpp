#include <pch.h>
#include "AudioWrapper.h"
#include <Dbt.h>
#include <sstream>

//HDEVNOTIFY hNewAudio = nullptr;
//
//AudioWrapper::AudioWrapper() : retry(false)
//{}
//
//bool AudioWrapper::initialize()
//{
//    DirectX::AUDIO_ENGINE_FLAGS flags = DirectX::AudioEngine_Default;
//
//    #ifdef _DEBUG
//        flags = flags | DirectX::AudioEngine_Debug;
//    #endif
//
//    //engine = std::make_unique<DirectX::AudioEngine>(flags);
//
//    return true;
//}
//
//void AudioWrapper::shutdown()
//{
//}
//
//void AudioWrapper::update()
//{
//    if (retry)
//    {
//        retry = false;
//
//        if (engine->Reset())
//        {
//            // TODO: restart looped sounds
//        }
//    }
//    else if (engine->Update() == false)
//    {
//        // update failed, which means device may have been lost/switched
//        retry = true;
//    }
//}
//
//void AudioWrapper::suspend()
//{
//    engine->Suspend();
//}
//
//void AudioWrapper::resume()
//{
//    engine->Resume();
//}
//
//void AudioWrapper::new_device_connected()
//{
//    retry = true;
//}
//
//bool AudioWrapper::load_sound(const char *soundName)
//{
//    std::cout << "Loading sound " << soundName << std::endl;
//
//    // see if the file is already loaded
//    const auto result = sounds.find(soundName);
//    if (result != sounds.end())
//    {
//        // make sure the sound is valid
//        if (result->second != nullptr)
//        {
//            return true;
//        }
//
//        // otherwise, try to load it again
//    }
//
//    std::stringstream narrowStream;
//    narrowStream << "Assets\\" << soundName << ".wav";
//    const std::string narrow = narrowStream.str();
//    const std::wstring wide(narrow.begin(), narrow.end());
//
//    sounds[soundName] = std::make_unique<DirectX::SoundEffect>(engine.get(), wide.c_str());
//
//    return sounds[soundName] != nullptr;
//}
//
//std::unique_ptr<DirectX::SoundEffectInstance> AudioWrapper::play_sound(const char *soundName)
//{
//    // make sure the sound exists
//    const auto result = sounds.find(soundName);
//
//    if (result != sounds.end())
//    {
//        auto sound = result->second->CreateInstance();
//        sound->Play(true);
//
//        return sound;
//    }
//
//    return nullptr;
//}
//
//bool AudioWrapper::listen_for_new_devices()
//{
//    DEV_BROADCAST_DEVICEINTERFACE filter = {};
//    filter.dbcc_size = sizeof(filter);
//    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
//    filter.dbcc_classguid = KSCATEGORY_AUDIO;
//
//    hNewAudio = RegisterDeviceNotification(renderer->get_resources().get_window(), &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
//
//    return hNewAudio != nullptr;
//}
//
//void AudioWrapper::stop_listening_for_new_devices()
//{
//    if (hNewAudio != nullptr)
//    {
//        UnregisterDeviceNotification(hNewAudio);
//    }
//}
