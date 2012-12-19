void pauseawhile(int howlong);
int collision(int x1,int y1,int xsize,int ysize,
	      int x2,int y2,int x2size,int y2size);
char *tb1_data_file(char *name,char *path);
int are_you_sure(tb1_state *game_state,
		                  char *warning_1,
		                  char *warning_2,
		                  char *yes_option,
		                  char *no_option);
   
