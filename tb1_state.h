struct tb1_state {
   int level;
   int shields;
   int shield_color;
   int score;
   unsigned char *virtual_1;
   unsigned char *virtual_2;
   unsigned char *virtual_3;
   SDL_Surface   *sdl_screen;
   int sound_enabled;
   vmw_font *tb1_font;
   char path_to_data[BUFSIZ];
};
