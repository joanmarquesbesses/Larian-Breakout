#include "AudioEngine.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <filesystem>

static ma_engine s_AudioEngine;
static bool s_Initialized = false;
static ma_sound s_Music;
static bool s_MusicLoaded = false;
static std::string s_CurrentMusicPath = "";

void AudioEngine::Init()
{
	ma_result result;

	result = ma_engine_init(NULL, &s_AudioEngine);

	if (result != MA_SUCCESS) {
		return;
	}

	s_Initialized = true;
	SetVolume(0.1f);
}

void AudioEngine::Shutdown()
{
	if (s_Initialized)
	{
		ma_engine_uninit(&s_AudioEngine);
		s_Initialized = false;
	}
}

void AudioEngine::Play(const std::string& filepath)
{
	if (!s_Initialized) return;

	if (!std::filesystem::exists(filepath))
	{
		return;
	}

	ma_result result = ma_engine_play_sound(&s_AudioEngine, filepath.c_str(), NULL);
}

void AudioEngine::PlayMusic(const std::string& filepath, bool loop)
{
	if (!s_Initialized) return;
	if (!std::filesystem::exists(filepath)) return;

	if (s_MusicLoaded && s_CurrentMusicPath == filepath) {
		if (ma_sound_is_playing(&s_Music)) {
			return;
		}
	}

	if (s_MusicLoaded) {
		ma_sound_uninit(&s_Music);
		s_MusicLoaded = false;
	}

	ma_result result = ma_sound_init_from_file(&s_AudioEngine, filepath.c_str(), 0, NULL, NULL, &s_Music);
	if (result == MA_SUCCESS) {
		ma_sound_set_looping(&s_Music, loop ? MA_TRUE : MA_FALSE);
		ma_sound_start(&s_Music);
		s_MusicLoaded = true;
		s_CurrentMusicPath = filepath; // Guardem el nom de la nova cançó
	}
}

void AudioEngine::StopMusic()
{
	if (s_MusicLoaded) {
		ma_sound_stop(&s_Music);
		s_CurrentMusicPath = "";
	}
}

void AudioEngine::SetVolume(float volume)
{
	if (!s_Initialized) return;
	volume = std::clamp(volume, 0.0f, 1.0f);
	ma_engine_set_volume(&s_AudioEngine, volume);
}

float AudioEngine::GetVolume()
{
	if (!s_Initialized) return 0.0f;
	return ma_engine_get_volume(&s_AudioEngine);
}