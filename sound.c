/* "borrowed" from gltron */

#ifdef SDL_MIXER_SOUND
#include <SDL.h>
#include <SDL_mixer.h>
#endif

#include "sound.h"
#include <stdlib.h>

  /* Function definition */
char *tb1_data_file(char *name,char *path);

/* linux only, at the moment */

#define NUM_GAME_FX 8

#ifdef SDL_MIXER_SOUND
static Mix_Music *music;

static Mix_Chunk *game_fx[NUM_GAME_FX];

#endif

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
#ifdef SDL_MIXER_SOUND
  int i;
  char *path;

  for(i = 0; i < NUM_GAME_FX; i++) {
    path=tb1_data_file(game_fx_names[i],path_to_data);
    if(path) {
      game_fx[i] = Mix_LoadWAV(path);
    }
  }
#endif
}
 
int initSound(char *path_to_data) {
  /* open the audio device */
#ifdef SDL_MIXER_SOUND   

           int audio_rate;
           Uint16 audio_format;
           int audio_channels;
   
   
           /* Initialize variables */
           audio_rate = MIX_DEFAULT_FREQUENCY;
           audio_format = MIX_DEFAULT_FORMAT;
           audio_channels = 2;
   
   
         /* Initialize the SDL library */
    if ( SDL_Init(SDL_INIT_AUDIO) < 0 ) {	
       fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
      return(255);
    }
   
         /* Open the audio device */
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, 4096) < 0) {	
       fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
       return(2);
    } else {
   
       Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
       printf("  + Opened audio at %d Hz %d bit %s\n", audio_rate,
			                          (audio_format&0xFF),
			  (audio_channels > 1) ? "stereo" : "mono");
   }
   
/*


   
  if(Mix_OpenAudio(22050, AUDIO_U16, 1, 1024) < 0) {
    fprintf(stderr, "can't open audio: %s\n", SDL_GetError());
    return -1;
  }
 */
  loadFX(path_to_data);
#endif
  return 0;
}


void shutdownSound() {
#ifdef SDL_MIXER_SOUND
  Mix_CloseAudio();
#endif
}
  

int loadSound(char *name) {
#ifdef SDL_MIXER_SOUND
  music = Mix_LoadMUS(name);
#endif
  return 0;
}

int playSound() {
#ifdef SDL_MIXER_SOUND
  if( ! Mix_PlayingMusic() )
    Mix_PlayMusic(music, -1);
  /* todo: remove the following once the bug in SDL_mixer is fixed */
  /* we don't want too many references to game objects here */
//  setMusicVolume(game->settings->musicVolume);
#endif
  return 0;
}

int stopSound() {
#ifdef SDL_MIXER_SOUND
  if( Mix_PlayingMusic() )
    Mix_HaltMusic();
#endif
  return 0;
}

void soundIdle() {
  /* sdl_mixer uses pthreads, so no work here */
  return;
}

void playGameFX(int fx) {
#ifdef SDL_MIXER_SOUND
  Mix_PlayChannel(-1, game_fx[fx], 0); 
//  fprintf(stderr, "fx on channel %d\n", Mix_PlayChannel(-1, game_fx[fx], 0));
#endif
}


void setMusicVolume(float volume) {
#ifdef SDL_MIXER_SOUND
  if(volume > 1) volume = 1;
  if(volume < 0) volume = 0;

  Mix_VolumeMusic((int)(volume * 128));
#endif
}

void setFxVolume(float volume) {
#ifdef SDL_MIXER_SOUND
  if(volume > 1) volume = 1;
  if(volume < 0) volume = 0;

  Mix_Volume(-1, (int)(volume * 128));
#endif
}
