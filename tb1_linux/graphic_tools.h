void coolbox(int x1,int y1,int x2,int y2,int fill,vmwVisual *target);
void shadowrite(char *st,int x5,int y5,int forecol,int backcol,
		vmwFont *tb1_font,vmwVisual *target);
char *check_for_tb1_directory(tb1_state *game_state,int try_to_create);
char *vmwGrInput(tb1_state *game_state,int x,int y,int howlong,int forecol,int backcol,
		                  vmwFont *tb1_font,vmwVisual *vis);
   
