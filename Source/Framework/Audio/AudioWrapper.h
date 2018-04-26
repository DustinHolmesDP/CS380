#pragma once

class AudioWrapper
{
public:
    AudioWrapper();

    bool initialize();
    void shutdown();

    void update();
    void suspend();
    void resume();
    void new_device_connected();

    /* should be the first part of the sound file name, "example" if "example.wav"
    returns true if the sound was/is loaded */
    bool load_sound(const char *soundName);

    /* returns an instance of the sound, volume/pitch/panning can be modified
    during playback, returns nullptr if sound not loaded*/
    std::unique_ptr<DirectX::SoundEffectInstance> play_sound(const char *soundName);

    static bool listen_for_new_devices();
    static void stop_listening_for_new_devices();
private:
    std::unique_ptr<DirectX::AudioEngine> engine;
    std::unordered_map<const char *, std::unique_ptr<DirectX::SoundEffect>> sounds;
    bool retry;
};