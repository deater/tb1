#define TB_ESC   27
#define TB_ENTER 1024
#define TB_F1    1025
#define TB_F2    1026
#define TB_UP    1027
#define TB_DOWN  1028
#define TB_LEFT  1029
#define TB_RIGHT 1030
#define TB_PGUP  1031
#define TB_PGDOWN 1032

int get_input();
void coolbox(int x1,int y1,int x2,int y2,int fill,ggi_visual_t page);
int close_graphics();
int quit();
void clear_keyboard_buffer();
void shadowrite(char *st,int x,int y,int forecol,int backcol,ggi_visual_t vis);
void pauseawhile(int howlong);

