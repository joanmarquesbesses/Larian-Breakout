#include "AudioEngine.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <filesystem>

static ma_engine s_AudioEngine;
static bool s_Initialized = false;

void AudioEngine::Init()
{
	ma_result result;

	result = ma_engine_init(NULL, &s_AudioEngine);

	if (result != MA_SUCCESS) {
		return;
	}

	s_Initialized = true;
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