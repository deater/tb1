/* "borrowed" from gltron */

#include "sound.h"
#include <stdlib.h>

  /* Function definition */
char *tb1_data_file(char *name,char *path);


/* linux only, at the moment */

static Mix_Music *music;

#define NUM_GAME_FX 8

static Mix_Chunk *game_fx[NUM_GAME_FX];

static char *game_fx_names[] = {
  "sound/tb_ahh.wav",
  "sound/tb_cc.wav",
  "sound/tb_kapow.wav",
  "sound/tb_scream.wav",
  "sound/tb_bonk.wav",
  "sound/tb_click.wav",
  "sound/tb_ow.wav",
  "sound/tb_zrrp.wav"
};

void loadFX(char *path_to_data) {
  int i;
  char *path;

  for(i = 0; i < NUM_GAME_FX; i++) {
    path=tb1_data_file(game_fx_names[i],path_to_data);
    if(path) {
      game_fx[i] = Mix_LoadWAV(path);
    }
  }
}
 
int initSound(char *path_to_data) {
  /* open the audio device */
  if(Mix_OpenAudio(22050, AUDIO_U16, 1, 1024) < 0) {
    fprintf(stderr, "can't open audio: %s\n", SDL_GetError());
    return -1;
  }
  loadFX(path_to_data);
  return 0;
}


void shutdownSound() {
  Mix_CloseAudio();
}
  

int loadSound(char *name) {
  music = Mix_LoadMUS(name);
  return 0;
}

int playSound() {
  if( ! Mix_PlayingMusic() )
    Mix_PlayMusic(music, -1);
  /* todo: remove the following once the bug in SDL_mixer is fixed */
  /* we don't want too many references to game objects here */
//  setMusicVolume(game->settings->musicVolume);
  return 0;
}

int stopSound() {
  if( Mix_PlayingMusic() )
    Mix_HaltMusic();
  return 0;
}

void soundIdle() {
  /* sdl_mixer uses pthreads, so no work here */
  return;
}

void playGameFX(int fx) {
  Mix_PlayChannel(-1, game_fx[fx], 0); 
//  fprintf(stderr, "fx on channel %d\n", Mix_PlayChannel(-1, game_fx[fx], 0));
}


void setMusicVolume(float volume) {
  if(volume > 1) volume = 1;
  if(volume < 0) volume = 0;

  Mix_VolumeMusic((int)(volume * 128));
}

void setFxVolume(float volume) {
  if(volume > 1) volume = 1;
  if(volume < 0) volume = 0;

  Mix_Volume(-1, (int)(volume * 128));
}
