struct vmwSprite {
   int xsize;
   int ysize;
   unsigned char *spritedata;
};

struct vmwSprite *vmwGetSprite(int x, int y, int xsize, int ysize, unsigned char *source);
void vmwPutSprite(struct vmwSprite *sprite,int x,int y,int shield_color,
		                    unsigned char *screen);
   
