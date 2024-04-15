#include "AudioDB.h"

void AudioDB::Initialize()
{
	AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	AudioHelper::Mix_AllocateChannels498(50);
}

Mix_Chunk* AudioDB::LoadClip(std::string clip_name)
{
	std::string clip_path = "resources/audio/" + clip_name;
	if (!(std::filesystem::exists(clip_path + ".wav") || std::filesystem::exists(clip_path + ".ogg"))) {
		std::cout << "error: failed to play audio clip " << clip_name;
		exit(0);
	}

	clip_path += std::filesystem::exists(clip_path + ".wav") ? ".wav" : ".ogg";

	if (ClipDB.find(clip_name) == ClipDB.end()) {
		ClipDB[clip_name] = AudioHelper::Mix_LoadWAV498(clip_path.c_str());
	}

	return ClipDB[clip_name];
}

void AudioDB::PlayAudio(int channel, std::string clip_name, bool does_loop)
{
	Mix_Chunk* clip = LoadClip(clip_name);

	int param = does_loop ? -1 : 0;

	AudioHelper::Mix_PlayChannel498(channel, clip, param);
}

void AudioDB::HaltChannel(int channel)
{
	AudioHelper::Mix_HaltChannel498(channel);
}

void AudioDB::SetVolume(int channel, float volume)
{
	AudioHelper::Mix_Volume498(channel, volume);
}
