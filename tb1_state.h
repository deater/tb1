struct tb1_state {
   int level;
   int shields;
   int shield_color;
   int score;
   vmwVisual *virtual_1;
   vmwVisual *virtual_2;
   vmwVisual *virtual_3;
   vmwSVMWGraphState *graph_state;
   int sound_enabled;
   char path_to_data[BUFSIZ];
};
