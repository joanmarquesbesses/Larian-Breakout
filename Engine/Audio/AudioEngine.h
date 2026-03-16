#pragma once

#include <string>

class AudioEngine
{
public:
	static void Init();
	static void Shutdown();

	static void Play(const std::string& filepath);
	static void PlayMusic(const std::string& filepath, bool loop = true);
	static void StopMusic();

	static void SetVolume(float volume);
	static float GetVolume();
};