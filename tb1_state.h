typedef struct {
   int level;                     /* Current Level   */
   int shields;                   /* Current Shields */
   int score;                     /* Current Score   */
   int begin_score;               /* Score at Beginning of Level (for save) */
   int begin_shields;             /* Shields at Beginning (for save)        */
   int checkpoints_passed;        /* For possible future expansion          */
   vmwVisual *virtual_1;          /* 320x200x8 */
   vmwVisual *virtual_2;          /* 320x400x8 */
   vmwVisual *virtual_3;          /* 320x200x8 */
   vmwSVMWGraphState *graph_state; /* Graphics state */
   int sound_possible;            /* Sound even possible? */
   int sound_enabled;             /* Sound on or off */
   int music_enabled;             /* Music on or off */
   int sound_volume;              /* Sound volume */
   int music_volume;              /* Music volume */
   int default_double_size;       /* Start in Double-Size Mode */
   int default_fullscreen;        /* Start in Full Screen Mode */
   char path_to_data[BUFSIZ];     /* Where our data is */
} tb1_state;
