#pragma once
#include <string>
class AudioSample{
   public:
  void * data;
  int * ref_cnt;
  AudioSample(char * data, int n_samples, int bitdepth, int n_channels, int start_sample = 0, float gain = 1.0);
  AudioSample(const char * path);
  AudioSample();
  AudioSample(const AudioSample &other);
  ~AudioSample();
  AudioSample & operator=(const AudioSample & other);
  void set_position(int off);
};

class Music{
   public:
  void * data;
  int * ref_cnt;
  std::string path;
  bool do_loop;
  Music(std::string path, bool do_loop = false);
  Music(const Music & other);
  Music & operator=(const Music & other);
  Music();
  ~Music();
  AudioSample get_chunk(int n_samples);
  void set_position(int pos, bool relative = false);

};

void init_audio(int samplerate, int bitdepth, int nchannels);
int play_audio_sample(AudioSample *,int channel = -1); //Returns the channel index
void play_music(Music );
void stop_music(); //Returns the current music object
