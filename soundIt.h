
/* SoundIt library 0.04

   Copyright 1994 Brad Pitzel  pitzel@cs.sfu.ca

   Adapted to Solaris/SunOS by Peter Ekberg.

   Feel free to use/distribute/modify as long as proper credits
   are included.
*/
 
/* Designed for digital sound effects in interactive apps (games, drum
   machines, digital organs, ???)

   Will mix channels of mono 8-bit raw samples, & play back in "real-time".
   Each channel can only play one sample at a time, but all
   channels can play a different sample simultaneously.                 
   
   If you have sox, use the ' -t .ub ' option to make samples
   that this library will play properly.
*/

#ifndef SOUNDIT_VERS
#define SOUNDIT_VERS "0.04"

#include <stdlib.h>
#include <stdio.h>

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

/* 00002 = 2 fragments   */
/* 00007 = means each fragment is 2^7 or 128 bytes */
/* See voxware docs (in /usr/src/linux/drivers/sound) for more info */
#define FRAG_SPEC 0x00020007

/*==========================================================================*/
typedef struct {
  unsigned char *data;	/* unsigned 8-bit raw samples */
  int len;    /* length of sample in bytes  */
  int loop;   /* loop=0 : play sample once, */
              /* loop=1 : loop sample       */
} Sample;

void dump_snd_list(void);

/*==========================================================================*/
/* given the name of a .raw sound file, load it into the Sample struct */ 
/* pointed to by 'sample'                                              */
int
Snd_loadRawSample(const char *file, Sample *sample, int loop);

/*==========================================================================*/
/* init sound device, etc..                                                 */
/* num_snd  = the number of samples in the sample array *sa                 */
/* sa       = the sample array						    */
/* freq     = the rate (Hz) to play back the samples                        */
/* channels = # of channels to mix                                          */
/* sound_device = a char string for the sound device, eg, "/dev/dsp"        */
/* returns: 0=success, -1=failure.*/
int
Snd_init(int num_snd, const Sample *sa, int freq, 
	 int channels, const char *sound_device);


/* shutdown sample player, free mem, etc/etc..*/
int
Snd_restore(void);


/* play a sound effect in the given channel 1..n*/
/* volume = integers from 0 (off) to 100 (full volume)*/
int
Snd_effect(int nr, int channel);


/* stop a channel (1..n) from playing*/
/*void
Snd_reset(enum snd_channel channel);*/
	

/* stop all channels from playing*/
/*void
Snd_reset(void);*/
	
	
#endif
