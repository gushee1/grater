#ifndef AUDIODB_H
#define AUDIODB_H

//STL
#include <filesystem>
#include <iostream>
#include <map>

//Internal
#include "AudioHelper.h"

class AudioDB {
public:
	static void Initialize();

	static Mix_Chunk* LoadClip(std::string clip_name);

	static void PlayAudio(int channel, std::string clip_name, bool does_loop);

	static void HaltChannel(int channel);

	static void SetVolume(int channel, float volume);

	static inline std::unordered_map<std::string, Mix_Chunk*> ClipDB;
};

#endif
