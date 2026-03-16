#pragma once

#include <string>

class AudioEngine
{
public:
	static void Init();
	static void Shutdown();

	static void Play(const std::string& filepath);

	static void SetVolume(float volume);
	static float GetVolume();
};