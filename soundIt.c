
/* SoundIt library 0.04

   Copyright 1994 Brad Pitzel  pitzel@cs.sfu.ca

   Adapted to Solaris/SunOS and HPUX by Peter Ekberg.

   Feel free to use/distribute/modify as long as proper credits
   are included.
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#if defined(HAVE_LINUX_SOUND)
# include <linux/soundcard.h>

#elif defined(HAVE_SUN_SOUND)
# if defined(HAVE_SYS_AUDIOIO_H)
#  include <sys/audioio.h>
# elif defined(HAVE_SUN_AUDIOIO_H)
#  include <sun/audioio.h>
# endif

#elif defined(HAVE_HP_SOUND)
# include <sys/audio.h>

#endif

#include "soundIt.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

/*==========================================================================*/
/* the mix buff, where the channels are mixed into. The mix buffer is then
   dumped to the sound device (/dev/dsp). Samples are mixed in
   Vunclipbuf (buffer of ints), then the values in Vunclipbuf are clipped to
   values between 0 and 255, and stored into Vclippedbuf (buffer of unsigned
   chars).
*/

typedef struct {
#if defined(HAVE_LINUX_SOUND)
  unsigned char *Vclippedbuf;
#elif defined(HAVE_16BIT_SOUND)
  short *Vclippedbuf;
#endif
  int *Vunclipbuf;
  int Vsize;
} Mix;

/*==========================================================================*/
typedef struct {
  unsigned char *Vstart, *Vcurrent;     /* ptr's into a playing sample  */
  int Vlen;                             /* length of sample in bytes    */
  int Vleft;                            /* bytes left of sample to play */
  int Vloop;                            /* loop=0 : play sample once    */
                                        /* loop=1 : loop sample         */
} Channel;

/*==========================================================================*/

/* variables prefixed with S_ are static */
/* 0 if mixer isn't initialized or init failed, 1 if mixer is good */
static int S_mixerStatus = 0;
	
static const Sample *S_sounds = NULL; /* ptr to array of samples */
static int S_num_sounds = 0;	      /* size of 'sounds' array above */
static int S_fd_snddev = -1;	      /* file # for sound device once open */
static int S_fd_pipe[2] = { -1, -1 }; /* pipe to talk to child process */
static int S_son_pid = -1;	      /* process ID for the forked sound mixer */
static const char *S_snddev = NULL;   /* char string for device, ie "/dev/dsp" */
static int S_num_channels = 6;	      /* number of channels to mix */
static int S_playback_freq = 0;	      /* playback frequency (in Hz) */
/*==========================================================================*/
/* non-public functions, used only within this file*/

int Snd_init_dev(void);
int Snd_restore_dev(void);

void Chan_reset(Channel *chan);	/* init channel structure */

	/* start a sample playing on a channel */
void Chan_assign(Channel *chan, const Sample *snd);

	/* mix all channels together into the 'mix' structure */
int Chan_mixAll(Mix *mix, Channel *ch);

	/* used by Chan_mixAll to mix the 1st channel */
int Chan_copyIn(Channel *chan, Mix *mix);

	/* used by Chan_mixAll to mix a loop in the 1st channel */
int Chan_copyIn_loop(Channel *chan, Mix *mix);

	/* used by Chan_mixAll to mix the middle channels */
int Chan_mixIn(Channel *chan, Mix *mix);

	/* used by Chan_mixAll to mix the last channel */
int Chan_finalMixIn(Channel *chan, Mix *mix);


/* alloc mem for mix buffer, and deallocate function */
/* The sound channels are mixed together into the mix buffer   */
/* then the mix buffer data is sent directly to the sound device */
void Mix_alloc(Mix *mix, int size);
void Mix_dealloc(Mix *mix);

/*==========================================================================*/
/* justing for testing, normally not called */
void
dump_snd_list(void)
{
  int i=0;

  for(i=0; i<S_num_sounds; i++) {
    printf("snd %d: len = %d \n", i, S_sounds[i].len );
  }
}
	
/*==========================================================================*/
int
Snd_init(int num_snd, const Sample *sa, int frequency, 
	 int channels, const char *dev)
{
  int result;

  S_num_sounds     = num_snd;
  S_sounds         = sa;	/* array of sound samples*/
  S_playback_freq  = frequency;
  S_num_channels   = channels; 
  S_snddev= dev;	/* sound device, eg /dev/dsp*/

  if (S_sounds==NULL)
    return EXIT_FAILURE;

  result=Snd_init_dev();

  if (result==EXIT_SUCCESS) {
    S_mixerStatus=1;
  }
  else {
    S_mixerStatus=0;
  }

  return result;
}

/*==========================================================================*/
int
Snd_restore(void)
{
  int result;

  if(!S_mixerStatus)
    return EXIT_FAILURE;

  result=Snd_restore_dev();

  if (result==EXIT_SUCCESS) {
    S_mixerStatus=0;
  }
  else {
    S_mixerStatus=0;
  }

  return result;
}

/*==========================================================================*/
/* volume control not implemented yet.*/
int
Snd_effect(int sound_num, int channel)
{
  if(!S_mixerStatus)
    return EXIT_FAILURE;

  /* make sure a valid sound # was passed in */
  if(sound_num<0 || sound_num>=S_num_sounds)
    return EXIT_FAILURE;

  /* make sure a valid channel # was passed in */
  if(channel<0 || channel>=S_num_channels)
    return EXIT_FAILURE;

  if(S_sounds[sound_num].data != NULL) {	
    write(S_fd_pipe[1], &sound_num, sizeof(sound_num));
    write(S_fd_pipe[1], &channel, sizeof(channel));
  }
  else {
    fprintf(stderr,"Referencing NULL sound entry\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/*============================================================================*/
int
Snd_init_dev(void)
{
  int whoami;
  S_fd_snddev = -1;

  S_son_pid = 0;

  if(access(S_snddev,W_OK) != 0) {
    perror("no access to sound device");
    return EXIT_FAILURE;
  }

  S_fd_snddev = open(S_snddev,O_WRONLY);

  if(S_fd_snddev < 0) {	
    perror("cannot open sound device");
    return EXIT_FAILURE;
  }

  close(S_fd_snddev);

  if(pipe(S_fd_pipe) < 0) {	
    perror("cannot create pipe for sound control");
    return EXIT_FAILURE;
  }

  /* now setup 2nd process for writing the data... */
  if((whoami = fork()) < 0) {	
    perror("cannot fork sound driver\n");
    return EXIT_FAILURE;
  }

  if(whoami != 0) {	/* successfully created son */
    close(S_fd_pipe[0]);	/* close end for reading */
    S_son_pid = whoami;
    return EXIT_SUCCESS;
  }

  /* Here is the code for the son... */
  {
    int sound_num,ch,i;
    struct timeval tval = {0L,0L};
    fd_set readfds,dsp;
    Mix mix;
    int fragsize;
    Channel *chan;

    chan = (Channel *)malloc(sizeof(Channel) * S_num_channels);

    for(i=0; i<S_num_channels; i++)
      Chan_reset( chan+i );

    S_fd_snddev = open(S_snddev,O_WRONLY);
    if(S_fd_snddev < 0) {	
      perror("Cannot open sound device");
      exit(1);
    }

#if defined(HAVE_LINUX_SOUND)
    {
      int frag;
      frag = FRAG_SPEC; /*defined in soundIt.h */

      ioctl(S_fd_snddev, SNDCTL_DSP_SETFRAGMENT, &frag);

      if(ioctl(S_fd_snddev,SNDCTL_DSP_SPEED, &S_playback_freq)==-1)
	perror("Sound driver ioctl");

      fragsize=0;
      if(ioctl(S_fd_snddev,SNDCTL_DSP_GETBLKSIZE, &fragsize)==-1)
	perror("Sound driver ioctl");
    }
#elif defined(HAVE_SUN_SOUND)
    {
      audio_info_t ainfo;

      AUDIO_INITINFO(&ainfo);
      ainfo.play.precision = 16;
      ainfo.play.sample_rate = S_playback_freq;
      ainfo.play.encoding = AUDIO_ENCODING_LINEAR;
      ainfo.play.channels = 1;
      ainfo.play.port = AUDIO_LINE_OUT|AUDIO_HEADPHONE|AUDIO_SPEAKER;
      if(ioctl(S_fd_snddev,AUDIO_SETINFO, &ainfo)==-1)
	perror("Sound driver ioctl");

      fragsize=512;
    }
#elif defined(HAVE_HP_SOUND)
    {
      struct audio_describe adescribe;
      struct audio_gain again;
      struct audio_limits alimits;
      int i;
      int best_rate;
      int temp;

      if(ioctl(S_fd_snddev, AUDIO_DESCRIBE, &adescribe)==-1)
	perror("Sound driver ioctl: Audio Describe");

      /*
      printf("Audio Device: ");
      switch(adescribe.audio_id) {
      case AUDIO_ID_PSB2160:
	printf("PSB 2160\n");
	break;
      case AUDIO_ID_CS4215:
	printf("CS 4215\n");
	break;
      default:
	printf("Unknown\n");
	break;
      }
      printf("Samplerates:");
      for(i=0; i<adescribe.nrates; i++)
	printf(" %d", adescribe.sample_rate[i]);
      printf("\nMax bits per sample: %d\n", adescribe.max_bits_per_sample);
      printf("Channels: %d\n", adescribe.nchannels);
      */

      if(adescribe.max_bits_per_sample < 16)
	printf("Not enough bits\n");

      temp = AUDIO_FORMAT_LINEAR16BIT;
      if(ioctl(S_fd_snddev, AUDIO_SET_DATA_FORMAT, temp)==-1)
	perror("Sound driver ioctl: Audio Set Data Format");

      best_rate = 0;
      for(i=0; i<adescribe.nrates; i++)
	if(abs(adescribe.sample_rate[i]-S_playback_freq)
	   < abs(best_rate-S_playback_freq))
	  best_rate = adescribe.sample_rate[i];
      if(abs(best_rate-S_playback_freq) > 1050)
	printf("No good sample rate\n");
      if(ioctl(S_fd_snddev, AUDIO_SET_SAMPLE_RATE, best_rate)==-1)
	perror("Sound driver ioctl: Audio Set Sample Rate");

      /* AUDIO_OUT_LINE | AUDIO_OUT_INTERNAL | AUDIO_OUT_EXTERNAL */
      if(ioctl(S_fd_snddev, AUDIO_SET_OUTPUT, AUDIO_OUT_EXTERNAL)==-1)
	perror("Sound driver ioctl: Audio Set Output");

      again.cgain[0].transmit_gain = AUDIO_MAX_GAIN;
      again.cgain[0].monitor_gain =  AUDIO_OFF_GAIN;
      again.cgain[1].transmit_gain = AUDIO_MAX_GAIN;
      again.cgain[1].monitor_gain =  AUDIO_OFF_GAIN;
      again.channel_mask = AUDIO_CHANNEL_LEFT | AUDIO_CHANNEL_RIGHT;
      if(ioctl(S_fd_snddev, AUDIO_SET_GAINS, &again)==-1)
	perror("Sound driver ioctl: Audio Set Gain");

      /*
      alimits.max_transmit_buffer_size = 512;
      alimits.max_receive_buffer_size = 512;
      if(ioctl(S_fd_snddev, AUDIO_SET_LIMITS, &alimits)==-1)
	perror("Sound driver ioctl: Audio Set Limits");

      if(ioctl(S_fd_snddev, AUDIO_GET_LIMITS, &alimits)==-1)
	perror("Sound driver ioctl: Audio Get Limits");

      fragsize=alimits.max_transmit_buffer_size;
      */
      fragsize = 1024;
    }
#endif

    /*printf("after: block size: %d \n", fragsize);*/

    /* init mixer object*/
    Mix_alloc(&mix, fragsize);

    close(S_fd_pipe[1]);	/* close end for writing */

    FD_ZERO(&dsp); 
    FD_SET(S_fd_snddev, &dsp);

    FD_ZERO(&readfds); 
    FD_SET(S_fd_pipe[0], &readfds);

    /*  printf("soundIt library v"SOUNDIT_VERS" initialized.\n");*/
		
    for(;;) {
      FD_SET(S_fd_pipe[0], &readfds);
      tval.tv_sec=0L;
      tval.tv_usec=0L;
      select(S_fd_pipe[0]+1, &readfds,NULL,NULL,&tval);

      if(FD_ISSET(S_fd_pipe[0], &readfds)) {
	if(read(S_fd_pipe[0], &sound_num, sizeof(int))==0)
	  break;

	read(S_fd_pipe[0], &ch, sizeof(int));

	/*printf("chan=%d snd=%d len=%d\n",
	       ch, sound_num, S_sounds[sound_num].len);*/
	Chan_assign(&(chan[ch]), &(S_sounds[sound_num]));
      }

      Chan_mixAll(&mix,chan);

#if defined(HAVE_LINUX_SOUND)
      write(S_fd_snddev, mix.Vclippedbuf, fragsize);
#elif defined(HAVE_SUN_SOUND)
      {
	int stat;
	audio_info_t ainfo;
	static unsigned int samplecount=0;

	do {
	  stat = ioctl(S_fd_snddev, AUDIO_GETINFO, &ainfo);
	  if(samplecount-ainfo.play.samples<fragsize)
	    stat = 1;
	} while(!stat);
	write(S_fd_snddev, (char *)mix.Vclippedbuf, sizeof(short)*fragsize);
	samplecount+=fragsize;
      }
#elif defined(HAVE_HP_SOUND)
      {
	struct audio_status astatus;
	int stat;

	do {
	  stat = ioctl(S_fd_snddev, AUDIO_GET_STATUS, &astatus);
	  if(astatus.transmit_exact_count<2*fragsize)
	    stat = 1;
	} while(!stat);

	write(S_fd_snddev, (char *)mix.Vclippedbuf, sizeof(short)*fragsize);
      }
#endif
    }

    Mix_dealloc( &mix );			
    /* printf("soundIt process exiting.\n");*/
    close(S_fd_pipe[0]);
    close(S_fd_pipe[1]);
    exit (0);
  } /* end of child process */
}


/*==========================================================================*/
int
Snd_restore_dev(void)
{
  close(S_fd_pipe[0]);
  close(S_fd_pipe[1]);

  /* wait for child process to die*/
  wait(NULL);
  return EXIT_SUCCESS;
}

/*==========================================================================*/
/*   CHANNEL MIXING FUNCTIONS						    */
/*==========================================================================*/
void
Chan_reset(Channel *chan)
{
  chan->Vstart=NULL;
  chan->Vcurrent=NULL;
  chan->Vlen=0;
  chan->Vleft=0;
  chan->Vloop=0;
}

/*==========================================================================*/
void
Chan_assign(Channel *chan, const Sample *snd)
{
  chan->Vstart  = snd->data;
  chan->Vcurrent= chan->Vstart;
  chan->Vlen    = snd->len;
  chan->Vleft   = snd->len;
  chan->Vloop   = snd->loop;
} 

/*==========================================================================*/
int
Chan_copyIn(Channel *chan, Mix *mix)
{
  int i, *p = mix->Vunclipbuf, result, min;

  result = (chan->Vleft>0) ? 1 : 0;
  min = (chan->Vleft < mix->Vsize) ? chan->Vleft : mix->Vsize;

  for(i=0; i<min; i++) {
    *p++ = (int)*chan->Vcurrent++;
  }
  chan->Vleft -= i;

  /* fill the remaining (if any) part of the mix buffer with silence */
  while(i<mix->Vsize) {
    *p++ = 128;
    i++;
  }
  return result;
}

/*==========================================================================*/
int
Chan_copyIn_loop(Channel *chan, Mix *mix)
{
  int i=0;
  int *p = mix->Vunclipbuf;
  int left_mix_vsize=mix->Vsize;

  if (chan->Vleft==0)
    chan->Vleft=chan->Vlen;

  if(left_mix_vsize > chan->Vleft) {
      
    /* fill the rest of the sample first */
    for(i=0; i<chan->Vleft; i++)
      *p++ = (int)*chan->Vcurrent++;

    left_mix_vsize -= chan->Vleft;
    chan->Vleft = chan->Vlen;
      
    /* loop the sample by writing it repeatedly */
    while(left_mix_vsize > chan->Vlen) {
      chan->Vcurrent=chan->Vstart;
      for(i=0; i<chan->Vleft; i++)
	*p++ = (int) *chan->Vcurrent++;

      left_mix_vsize-=chan->Vlen;
    }

    /* Return to the start of the Sample */
    chan->Vcurrent=chan->Vstart;

    /* If there is some space left, then fill it up :-) */
    for(i=0;i<left_mix_vsize;i++)
      *p++=(int)*chan->Vcurrent++;

    chan->Vleft=chan->Vlen - left_mix_vsize;             
  } 
  else {

    /* fill in all that fits, no looping to be done ! */
    for(i=0; i<mix->Vsize; i++)
      *p++ = (int) *chan->Vcurrent++;

    chan->Vleft -= i;
  }
  return 1;
}

/*==========================================================================*/
int
Chan_mixIn(Channel *chan, Mix *mix)
{
  int i,*p = mix->Vunclipbuf, result, min;

  result = (chan->Vleft>0) ? 1 : 0;
  min = (chan->Vleft < mix->Vsize) ? chan->Vleft : mix->Vsize;

  for(i=0; i<min; i++)
    *p++ += (int)(*chan->Vcurrent++) - 128;

  chan->Vleft -= i;
  return result;
}

/*========================================================================*/
/* clip an int to a value between 0 and 255 */
#ifdef HAVE_16BIT_SOUND
/*
static inline short
clip(int i)
{
  return ((i<0) ? 0 : ( (i>255) ? 255 : i )) * 257 - 32768;
}
*/
#define clip(i) ((short)((((i)<0) ? 0 : ( ((i)>255) ? 255 : (i) )) * 257 - 128*257))
#else
static inline unsigned char
clip(int i)
{
  return (i<0) ? 0 : ( (i>255) ? 255 : i );
}
#endif

/*==========================================================================*/
int
Chan_finalMixIn(Channel *chan, Mix *mix)
{
  int i;
  int *p = mix->Vunclipbuf, result, min;
#ifdef HAVE_16BIT_SOUND
  short *final = mix->Vclippedbuf;
#else
  unsigned char *final = mix->Vclippedbuf;
#endif

  result = (chan->Vleft>0) ? 1 : 0;
  min = (chan->Vleft < mix->Vsize) ? chan->Vleft : mix->Vsize;

  for(i=0; i<min; i++) {
    *p += (int)(*chan->Vcurrent++) - 128;
    *final++ = clip(*p);
    p++;
  }
  chan->Vleft -= i;

  /* copy rest of Vunclipbuf over to Vclippedbuf */
  while (i<mix->Vsize) {
    *final++ = clip(*p);
    p++;
    i++;
  }

  return result;
}


/*==========================================================================*/
void
Mix_alloc(Mix *mix, int size)
{
#ifdef HAVE_16BIT_SOUND
  mix->Vclippedbuf = (short *)calloc( sizeof(short), size);
#else
  mix->Vclippedbuf = (unsigned char *)calloc( sizeof(char), size);
#endif
  mix->Vunclipbuf = (int *)calloc( sizeof(int), size);
  mix->Vsize  = size;

  if((mix->Vclippedbuf==NULL) || (mix->Vunclipbuf==NULL)) {
    printf("Unable to allocate memory for mixer buffer.\n");
    exit(-1);
  }
}

/*==========================================================================*/
void
Mix_dealloc(Mix *mix)
{ 
  if(mix->Vclippedbuf)
    free(mix->Vclippedbuf); 
  if(mix->Vunclipbuf)
    free(mix->Vunclipbuf); 
}

/*==========================================================================*/
/* Mixes together the channels into one sound.
   Returns # of channels currently playing *any* sound
   Therefore, return 0 means to channels have a sample, therefore no
   sound is playing
*/ 
int
Chan_mixAll(Mix *mix, Channel *chan)
{
  int result=0, i=0;

  /* Loop in the first channel ? */
  if (chan->Vloop==0)
    result = Chan_copyIn(chan, mix);
  else
    result = Chan_copyIn_loop(chan, mix);

  /* we want to loop for S_num_channels-2 */
  for(i=2; i<S_num_channels; i++)
    result += Chan_mixIn(++chan, mix);

  result += Chan_finalMixIn(++chan, mix);

  return result;
}

/*==========================================================================*/
/* given the name of a .raw sound file, load it into the Sample struct */ 
/* pointed to by 'sample'                                              */
/* Returns -1 couldn't open/read file				       */
/*         -2 couldn't alloc memory)                                   */
int
Snd_loadRawSample(const char *file, Sample *sample, int loop)
{
  FILE *fp;

  sample->data = NULL;
  sample->len  = 0;
  sample->loop = loop;

  fp = fopen(file,"r");   

  if(fp==NULL)
    return -1;

  /* get length of the file */
  sample->len = lseek(fileno(fp), 0, SEEK_END);
   
  /* go back to beginning of file */
  lseek(fileno(fp), 0, SEEK_SET);

  /* alloc memory for sample */
  sample->data = (unsigned char *)malloc(sample->len);

  if(sample->data==NULL) {
    fclose(fp);
    return -2;
  }

  fread(sample->data, 1, sample->len, fp);

  fclose(fp);

  return 0;
}
