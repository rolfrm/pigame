
#include "audio.h"
#include <SFML/Audio.hpp>
#include <iostream>
#include <string.h>
#define MAX_CHANNELS 8
AudioSample * channels[MAX_CHANNELS];
sf::Sound sounds[MAX_CHANNELS];

template<typename T>
void copy(const T * original, T * out){
  memcpy(out,original,sizeof(T));
}

sf::Music * sf_music;
AudioSample::AudioSample(char * data, int n_samples, int bitdepth, int n_channels, int start_sample, float gain){
  if(bitdepth != 2){
    std::cout << "Unsupported bitdepth: " << bitdepth << "Audio must be unsigned integers, bitdepth: 2\n";
  }
  sf::SoundBuffer * buffer = new sf::SoundBuffer();
   if(!buffer->LoadFromSamples((short *) data,n_samples,n_channels,44100)){
    std::cout << "Error loading audiosamples";
  }
  this->data = (void * ) buffer;
  ref_cnt = new int;
  *ref_cnt = 1;
}


AudioSample::AudioSample(const char * path){
  sf::SoundBuffer * buffer = new sf::SoundBuffer();
  if(!buffer->LoadFromFile(path)){
    std::cout << "Error loading audiosamples";
  }
  data = (void * ) buffer;
  ref_cnt = new int;
  *ref_cnt = 1;
}

void AudioSample::set_position(int off){
  sf::SoundBuffer * buffer = (sf::SoundBuffer *) data;
}

AudioSample::AudioSample(){
  data = NULL;
  ref_cnt = NULL;
}
AudioSample::AudioSample(const AudioSample &other){
  copy<AudioSample>(&other,this);
  if(data != NULL){
    *ref_cnt +=1;
  }
}
AudioSample::~AudioSample(){
  *ref_cnt -=1;
  if(*ref_cnt <= 0){
    delete ref_cnt;
    delete (sf::SoundBuffer *) data;
  }
}
AudioSample & AudioSample::operator=(const AudioSample & other){
  if(data != NULL){
    *ref_cnt -=1;
    if(*ref_cnt <= 0){
      delete ref_cnt;
      delete (sf::SoundBuffer *) data;
    }
  }


  *this = other;
  if(data != NULL){
    *ref_cnt +=1;
  }
  return *this;
}
  
/*Music::Music(){
    ref_cnt = NULL;
    data = NULL;
    }*/

Music::Music(std::string path, bool do_loop){
  this->path = path;
  this->do_loop = do_loop;
  sf::Music *  self_data = new sf::Music();
  self_data->OpenFromFile(path);
  data =(void *) self_data;
  ref_cnt = new int;
  *ref_cnt = 1;
}

Music::Music(){
  data = NULL;
  ref_cnt =  NULL;
}
Music::Music(const Music & other){
  copy<Music>(&other,this);//*this = other;
  ref_cnt +=1;
}

Music & Music::operator=(const Music & other){
  if(data != NULL){
    *ref_cnt--;
    if((*ref_cnt) <= 0){
      std::cout << *ref_cnt <<" " << data << " " << path <<"\n";
      delete ref_cnt;
      delete (sf::Music *) data;
    }
  }
  copy<Music>(&other,this);
  //*this = other;
  *ref_cnt++;
}

  Music::~Music(){
    if(data != NULL){
      *ref_cnt--;
      if(*ref_cnt <= 0){
	delete ref_cnt;
	delete (sf::Music *) data;
      }
    }
  }

AudioSample Music::get_chunk(int n_samples){

}

void Music::set_position(int pos, bool relative){

}


void init_audio(int samplerate, int bitdepth, int nchannels){
  for(int i = 0; i < MAX_CHANNELS;i++){
    channels[i] = NULL;
  }
  sf_music = NULL;
}

int play_audio_sample(AudioSample * as,int channel){ //Returns the channel index
    sf::SoundBuffer * buffer = (sf::SoundBuffer *) as->data;
    if(channel == -1){
      for(int i = 0; i < MAX_CHANNELS;i++){
	if(channels[i] == NULL){
	  channels[i] = as;
	  sounds[i].SetBuffer(*buffer);
	  sounds[i].Play();
	  return i;
	}
      }
    }else if(channel < MAX_CHANNELS){
      if(channels[channel] != NULL){
	sounds[channel].Stop();
      }
      channels[channel] = as;
      sounds[channel].SetBuffer(*buffer);
      //sounds[channel] = buffer;
      sounds[channel].Play();

    }
}

void play_music(Music ms){
  sf::Music * new_music = (sf::Music * ) ms.data;
  if(sf_music == new_music){
    sf_music->Play();
    return;
  }
  if(sf_music != NULL){
    sf_music->Stop();
  }
  sf_music = new_music;
  sf_music->Play();
}

void stop_music(){ //Returns the current music object
  if(sf_music != NULL){
    sf_music->Stop();
  }
}
