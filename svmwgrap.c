#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ggi/libggi.h>


/*var unfadered:array[0..255] of byte;
    unfadeblue:array[0..255] of byte;
    unfadegreen:array[0..255] of byte;
    fontline:array[0..255] of byte;
    charset:array[0..255,0..15] of byte;
    charheight:byte;
    fontseg,fontoff:word;
*/


typedef struct {
   char *font_data;
   int width;
   int height;
   int numchars;
} vmw_font;


int vmwCrossBlit(char *dest,char *src,int stride,int ysize)
{
   
  int y;

  for (y = 0; y < ysize; y++) {
      memcpy (dest, src, stride);
      src += stride;
      dest += stride;
  }
  return 0;   
}

int vmwCrossBlit240(char *dest,char *src,int stride,int stride_factor,
		    int ysize)
{
   
     int y;
   
     for (y = 0; y < ysize; y++) {
	      memcpy (dest, src, (240*stride_factor));
	      src += stride;
	      dest += stride;
     }
     return 0;
}

int vmwArbitraryCrossBlit(char *src,int x1,int y1,int w,int h,
			  char *dest,int x2,int y2,int stride,
			  int stride_factor)
{
   int y;
   src+=(stride*y1);
   dest+=(stride*y2);
   
   for(y=0;y<h;y++) {
      memcpy ((dest+x2),(src+x1),(w*stride_factor));
      src+=stride;
      dest+=stride;
   }
   return 0;   
    
}

int vmwPutSprite(int *src,int w,int h,int stride_factor,
		 char *dest,int x,int y,int dest_stride)
{
    int xx,yy;
    dest+=(dest_stride*y);
    for(yy=0;yy<h;yy++){ 
       for(xx=0;xx<w;xx++) 
          if(*(src+xx)) memcpy(dest+(stride_factor*(xx+x)),(src+xx),stride_factor);
	 /**(dest+xx+x)=15;*/
   src+=w;
   dest+=dest_stride;
    }
  return 0;
   
}

int vmwPutPartialSprite(int *src,int w,int h,int stride_factor,
		        char *dest,int x,int y,int dest_stride,
			int x_start,int x_stop,int y_start,int y_stop)
{                          /* x_start/stop not implemented yet */
    int xx,yy;
    dest+=(dest_stride*y);
    for(yy=0;yy<h;yy++){ 
       for(xx=0;xx<w;xx++) 
          if ((*(src+xx)) && ((yy>=y_start) && (yy<=y_stop)) )
	     memcpy(dest+(stride_factor*(xx+x)),(src+xx),stride_factor);
	 /**(dest+xx+x)=15;*/
   src+=w;
   dest+=dest_stride;
    }
  return 0;
   
}


int vmwGetSprite(ggi_visual_t visual,int x,int y,int w,int h,
		                  int *sprite)
{
       int width,xx;
   
       xx=x;
       while(h--) {
	           x=xx;width=w;
	           while(width--) ggiGetPixel(visual,x++,y,sprite++);
	           y++;
       }
       return 0;

}
 

/*
int vmwPutSprite(ggi_visual_t visual,int x,int y,int w,int h,
		                  int transparent_color,int *sprite)
{
       int width,xx;
   
       xx=x;
       while(h--) {
	         x=xx; width=w;
	         while(width--){
		             if ( *(sprite)!=transparent_color )
		                  ggiPutPixel(visual,x,y,*(sprite));
		             x++;
		             sprite++;
		 }
	         y++;
       }
       return 0;
}
*/


/*
function Mode13SavePicPacked(x1,y1,x2,y2,numcolors:integer;where:word;filename:string):byte;
function Mode13LoadPicPacked(x1,y1:integer;where:word;LoadPal,LoadPic:boolean;FileName:string):byte;
procedure cls32(col:byte;where:word);
procedure flipd320(source,dest:word);
procedure flipd240(hm,va,va2:word);
procedure flipd50(fromwhere,off1,whereto,off2:word);

Procedure SetMCGA;
Procedure SetText;  { This procedure returns you to text mode.  }
Procedure LoadFont(namest:string);

Procedure Putpixel (X,Y : Integer; Col : Byte; where:word);
Procedure Putpixel240 (X,Y : Integer; Col : Byte; where:word);
function getpixel(x,y,where:word):byte;
Procedure Pal(Col,R,G,B : Byte);
Procedure GetPal(Col : Byte; Var R,G,B : Byte);
procedure WaitRetrace;
procedure outtextlineover(st:string;x,y:integer;col,background:byte;line:integer;where:word);
procedure outtextline(st:string;x,y:integer;col,background:byte;line:integer;where:word);
procedure outtextxy(st:string;x,y,col,background:integer;where:word;overwrite:boolean);
procedure outsmalltextlineover(st:string;x,y:integer;col,background:byte;line:integer;where:word);
procedure outsmalltextline(st:string;x,y:integer;col,background:byte;line:integer;where:word);
procedure outsmalltextxy(st:string;x,y,col,background:integer;where:word;overwrite:boolean);
function  sgn(a:real):integer;
procedure line(a,b,c,d,col:integer;where:word);
procedure horizontalline(FromX1,ToX2,AtY,col:integer;where:word);
procedure verticalline(FromY1,ToY2,AtX,col:integer;where:word);
procedure fade;
procedure unfade;
procedure box(x1,y1,x2,y2,col:integer;where:word);
procedure vdelay(howlong:integer);
procedure setupvmwgraph;
procedure setuptb1;






{Errors:  0=No Errors
          1=File Not Found
          2=Not a Paintpro File (no PAINTPROVX.XX header)
          3=Improper Version (less then 6)
}
*/

int GGILoadPicPacked(int x1,int y1,ggi_visual_t vis
		     ,int LoadPal,int LoadPic,char *FileName,
		     ggi_color *eight_bit_pal,ggi_pixel *tb1_pal,
		     int color_depth)


/*Loads a paintpro image, filename, at location x1,y1      *\
\*to offset where (vga=$A000) and loadspal if LOADPAL=true */
/*Loadsapicture if Loadpic=true and returns error}         */

{
    unsigned char temp1,temp2,temp3;
    int errorlev;
    int int1,int2,i,x,y;
    char buffer[300];
    int lastread=0;
    int buffpointer=0;
    int picwidth,picheight;
    FILE *fff;
    char header[10];
    int xsize;
    int ysize;
    int numcolors;
    int col,numacross;
   
       /* Open the file */                  
    fff=fopen(FileName,"rb");
    if (fff==NULL){
       printf("PPRO error... File \"%s\" not found.\n",FileName);
       exit(1);
    }
    errorlev=fread(buffer,1,300,fff);
       /* Check to see if its really a Paintpro File */
    for (i=0;i<9;i++) {
       header[i]=buffer[i];
    }
    header[9]='\0';
    if (strcmp(header,"PAINTPROV")) {
       printf("PPRO error... %s is NOT a paintpro file!\n",FileName);
       exit(2);
    }
       /*printf("Paintpro Confirmed\n");*/
       /* Check to see if it is the proper version (currently 6.0) */
    header[0]=buffer[9];
    header[1]=buffer[10];
    header[2]=buffer[11];
    header[3]='\0';       
    if (strcmp(header,"6.0")) {
       printf("PPRO error... Version %s unsupported, must be >6\n",header);
       exit(3);
    }
    buffpointer=12;
       /* Read X and Y sizes */
    temp1=buffer[buffpointer];
    temp2=buffer[buffpointer+1];
    temp3=buffer[buffpointer+2];
    buffpointer+=3;
    if (buffpointer>=errorlev) {
       printf("PPRO error... Early end of file.\n");
       exit(3);
    }
    xsize=(temp2>>4)+(temp1<<4);
    ysize=((temp2-((temp2>>4)<<4))<<16)+temp3;
       /* being old 6 bytes 2 two int*/
    temp1=buffer[buffpointer];
    temp2=buffer[buffpointer+1];
    temp3=buffer[buffpointer+2];
    buffpointer+=3;
    if (buffpointer>=errorlev) {
       printf("PPRO error... Early end of file.\n");
       exit(3);
    }
    numcolors=(temp2>>4)+(temp1<<4);
    /*int2=((temp2-((temp2>>4)<<4))*256)+temp3;*/
    picwidth=xsize+1;
    picheight=ysize+1;
       /*Load Palette*/
    if (numcolors!=256) printf("%d colors is not supported yet.\n",numcolors);
    for (i=0;i<256;i++) {
       eight_bit_pal[i].r=(buffer[buffpointer]*0x100);
       eight_bit_pal[i].g=(buffer[buffpointer+1]*0x100);
       eight_bit_pal[i].b=(buffer[buffpointer+2]*0x100); 
       buffpointer+=3;
       if (buffpointer>=errorlev) {
	  if (errorlev==300) {
	     errorlev=fread(buffer,1,300,fff);
	     buffpointer=0;
	  }
	  else {
	     lastread=1;
	  }
       }    
    }
    if(LoadPal) {
       if(color_depth!=8) {
	  for(i=0;i<256;i++) 
	     tb1_pal[i]=ggiMapColor(vis,&eight_bit_pal[i]);
       }
       else {
	 for(i=0;i<256;i++) tb1_pal[i]=(ggi_pixel)i;
	 ggiSetPaletteVec(vis,0,256,eight_bit_pal);
       }
    }
    x=x1;
    y=y1;
    while ((!lastread)&&(LoadPic)) {
       temp1=buffer[buffpointer];
       temp2=buffer[buffpointer+1];
       temp3=buffer[buffpointer+2];
       buffpointer+=3;
       if (buffpointer>=errorlev) {
	  if (errorlev==300) {
	     errorlev=fread(buffer,1,300,fff);
	     buffpointer=0;
	  }
	  else lastread=1;
       }
       int1=(temp2>>4)+(temp1<<4);
       int2=((temp2-((temp2>>4)<<4))*256)+temp3;       
       if (int1>2047) {
          ggiPutPixel(vis,x,y,tb1_pal[int1-2048]);
          x++;
          if (x>xsize+x1) {
	     x=x1; y++;
	  }
          ggiPutPixel(vis,x,y,tb1_pal[int2-2048]);
          x++;
          if (x>xsize+x1){
	     x=x1;y++;
	  }
       }
       else {
          col=int1;
          numacross=int2;
          while ((x+numacross)>(xsize+x1)) {
	     ggiSetGCForeground(vis,tb1_pal[col]);
	     ggiDrawHLine(vis,x,y,((xsize+x1)-x));
             numacross=numacross-((xsize+1)-x);
	     x=x1;
	     y++;
          }
          if (numacross!=0) {
	     ggiSetGCForeground(vis,tb1_pal[col]);
	     ggiDrawHLine(vis,x,y,numacross);
          }
          x=x+numacross;
       }
    }
    ggiFlush(vis);
    if (fff!=NULL) fclose(fff);
    return 0;
}
 


vmw_font *LoadVMWFont(char *namest,int xsize,int ysize,int numchars)
{
    unsigned char buff[16];
    FILE *f;
    int i,fonty,numloop;
    vmw_font *font;
    char *data;

    font=(vmw_font *)malloc(sizeof(vmw_font));
    data=(char *)calloc(numchars*ysize,(sizeof(char)));
   
    f=fopen(namest,"r");
    if (f==NULL) {
       printf("ERROR loading font file %s.\n",namest);
       return NULL;
    }
    numloop=(numchars*ysize);
    
    font->width=xsize;
    font->height=ysize;
    font->numchars=numchars;
    font->font_data=data;
    fonty=0;
    while ( (!feof(f))&&(fonty<=numloop)) {
       fread(buff,1,16,f);
       for(i=0;i<16;i++) font->font_data[fonty+i]=buff[i];
       fonty+=16;
    }
    fclose(f);
    return font;
}

/*int bit(unsigned char mree,int whichbit)
{
   return mree&(128>>whichbit);
}
 */

void VMWtextlineover(char *st,int x,int y,int col,int background,int line,
		     vmw_font *font,ggi_visual_t vis)
{
    int i,xx,len;
    len=strlen(st);
    for(i=0;i<len;i++)
       for(xx=0;xx<8;xx++){
          if ( (font->font_data[(st[i]*font->height)+line])&(128>>xx) ) 
	     ggiPutPixel(vis,(x+(i*8)+xx),y,col);
          else 
	     ggiPutPixel(vis,(x+(i*8)+xx),y,background);
       }
}


void VMWtextline(char *st,int x,int y,int col,int background,int line,
		 vmw_font *font,ggi_visual_t vis)
{
    int i,len,xx;
    len=strlen(st);
    for(i=0;i<len;i++)
       for(xx=0;xx<8;xx++)
	  if( ((unsigned char) (font->font_data[(st[i]*16)+line]))
	      &(128>>xx) ) {
	         ggiPutPixel(vis,x+(i*8)+xx,y,col);
              }
}
   
void VMWtextxy(char *st,int x,int y,int col,int background,int overwrite,
	       vmw_font *font,ggi_visual_t vis)
{
    int lineon,i,xx,len;

    len=strlen(st);
    for(lineon=0;lineon<8;lineon++){
       for(i=0;i<len;i++)
	  for(xx=0;xx<8;xx++){
	     if ( (font->font_data[(st[i]*font->height)+lineon])&(128>>xx) )
		ggiPutPixel(vis,(x+(i*8)+xx),y+lineon,col);
	     else
	        if(overwrite) ggiPutPixel(vis,(x+(i*8)+xx),y+lineon,background);
	  }
     } 
       /*VMWtextlineover(st,x,y+lineon,col,background,lineon,font,vis);*/
       /* VMWtextline(st,x,y+lineon,col,background,lineon,font,vis);*/
}
   

void VMWsmalltextxy(char *st,int x,int y,int col,int background,int overwrite,
		    vmw_font *font,ggi_visual_t vis)
{
    int lineon,i,xx,len;

     /* This will print out a small 4x5 font located in chars */
     /* 128-255 in a normal VGA font.  These are custom made  */
     /* fonts they are non-standard */
   
    len=strlen(st);
    for(lineon=0;lineon<5;lineon++) {
       for(i=0;i<len;i++)
	  for(xx=0;xx<5;xx++) {
	     if ( (font->font_data[((st[i]+128)*font->height)+lineon])&(128>>xx) )
	        ggiPutPixel(vis,(x+(i*5)+xx),y+lineon,col);
             else
	       if(overwrite) ggiPutPixel(vis,(x+(i*5)+xx),y+lineon,background);
	  }
    }
}

/*
var i,len:word;

begin
  len:=length(St);
  for i:=1 to (len div 2)+1 do begin
      fontline[(i-1)*2]:=charset[(ord(st[(i*2)]))+128,line];
      fontline[((i-1)*2)+1]:=charset[(ord(st[(i*2)-1]))+128,line];
  end;
  asm
    push ds
    push si
    push es
    push di
      mov ax,[fontseg]
      mov ds,ax
      mov ax,[fontoff]
      mov si,ax
      mov dh,[col]
      mov dl,[background]
          mov ax,[y]     {multiplies y value by 320}
          shl ax,5
          mov cx,ax
          shl ax,1
          shl cx,3
          add ax,cx
          add ax,[x]        {adds the x-value to get offset}
      mov di,ax
      mov ax,[where]
      mov es,ax
   mov bx,[len]
gus1:
   xor ax,ax
   cmp bx,02
   jnge gus4
   mov cx,10
   lodsb
   push bx
   mov bl,al
   xor bh,bh
   lodsb
   shl ax,5
   add ax,bx
   shl ax,3
   pop bx
   jmp gus5
gus4:
   mov cx,5
   lodsw
gus5:
   shl ax,1
   push ax
   jc gus7
   mov al,dl
   jmp gus8
gus7:
   mov al,dh
gus8:
  xor ah,ah
  stosb
  pop ax
  loop gus5
  cmp bx,2
  jng gus3
  dec bx
  dec bx
   jmp gus1
gus3:
    pop di
    pop es
    pop si
    pop ds
end;
end;

procedure outsmalltextline(st:string;x,y:integer;col,background:byte;line:integer;where:word);
label gus1,gus4,gus6,gus5,gus7,gus8,gus3;
var i,len:word;

begin
  len:=length(St);
  for i:=1 to (len div 2)+1 do begin
      fontline[(i-1)*2]:=charset[(ord(st[(i*2)]))+128,line];
      fontline[((i-1)*2)+1]:=charset[(ord(st[(i*2)-1]))+128,line];
  end;
  asm
    push ds
    push si
    push es
    push di
      mov ax,[fontseg]
      mov ds,ax
      mov ax,[fontoff]
      mov si,ax
      mov dh,[col]
      mov bl,[background]
          mov ax,[y]     {multiplies y value by 320}
          shl ax,5
          mov cx,ax
          shl ax,1
          shl cx,3
          add ax,cx
          add ax,[x]        {adds the x-value to get offset}
      mov di,ax
      mov ax,[where]
      mov es,ax
   mov bx,[len]

gus1:
   xor ax,ax
   cmp bx,02
   jnge gus4
   mov cx,10
   lodsb
   push bx
   mov bl,al
   xor bh,bh
   lodsb
   shl ax,5
   add ax,bx
   shl ax,3
   pop bx
   jmp gus5
gus4:
   mov cx,5
   lodsw
gus5:
   shl ax,1
   push ax
   jc gus7
   inc di
   jmp gus8
gus7:
   mov al,dh
   xor ah,ah
   stosb
gus8:
  pop ax
  loop gus5
  cmp bx,2
  jng gus3
  dec bx
  dec bx
  jmp gus1
gus3:
    pop di
    pop es
    pop si
    pop ds
end;
end;


procedure outsmalltextxy(st:string;x,y,col,background:integer;where:word;overwrite:boolean);
label l0105;
var l,len,i,xadd,yadd,lineon:integer;
    n,k,o,min,max,qwerty:byte;
begin
  for lineon:=0 to 4 do
      if overwrite then
         outsmalltextlineover(st,x,y+lineon,col,background,lineon,where)
      else
         outsmalltextline(st,x,y+lineon,col,background,lineon,where);

end;



function sgn(a:real):integer;
begin
     if a>0 then sgn:=+1;
     if a<0 then sgn:=-1;
     if a=0 then sgn:=0;
end;

procedure line(a,b,c,d,col:integer;where:word);
var u,s,v,d1x,d1y,d2x,d2y,m,n:real;
    i:integer;
begin
     u:= c - a;
     v:= d - b;
     d1x:= SGN(u);
     d1y:= SGN(v);
     d2x:= SGN(u);
     d2y:= 0;
     m:= ABS(u);
     n := ABS(v);
     IF NOT (M>N) then
     BEGIN
          d2x := 0 ;
          d2y := SGN(v);
          m := ABS(v);
          n := ABS(u);
     END;
     s := INT(m / 2);
     FOR i := 0 TO round(m) DO
     BEGIN
          putpixel(a,b,col,where);
          s := s + n;
          IF not (s<m) THEN
          BEGIN
               s := s - m;
               a:= a +round(d1x);
               b := b + round(d1y);
          END
          ELSE
          BEGIN
               a := a + round(d2x);
               b := b + round(d2y);
          END;
     end;
END;

procedure horizontalline(FromX1,ToX2,AtY,col:integer;where:word);

begin
  asm
    push es
    push di
    
          mov ax,[aty]     {multiplies y value by 320}
          shl ax,5
          mov cx,ax
          shl ax,1
          shl cx,3
          add ax,cx
          add ax,[fromx1]        {adds the x-value to get offset}
      mov di,ax
      mov ax,[where]
      mov es,ax

     mov cx,[tox2]
     sub cx,[fromx1]  {how long line is}

   xor ax,ax      {load color}
   mov ax,col
   rep stosb      {draw it}
    pop di
    pop es
end;
end;

procedure verticalline(FromY1,ToY2,AtX,col:integer;where:word);
label mree2,mree3,mree,mree4;
begin
  asm
    push es
    push di
       xor dx,dx
       mov bx,[toy2]                     {this mess saves you}
       mov ax,[fromy1]                   {if you put a y1 value}
                                         {lower than y2.  It is}
     cmp bx,ax                           {probably unnecessary}
     jz mree3
     jl Mree2
        sub bx,ax  {how long line is}
        mov dx,bx
        
        jmp mree3
  mree2:
        sub ax,bx
        mov dx,ax
        mov ax,[toy2]
  mree3:



          {mov ax,[FromY1]}     {multiplies y value by 320}
          shl ax,5
          mov cx,ax
          shl ax,1
          shl cx,3
          add ax,cx
          add ax,[atX]        {adds the x-value to get offset}
      mov di,ax
      mov ax,[where]
      mov es,ax


  mov cx,dx
  inc cx

   xor ax,ax      {load color}
   mov ax,col

Mree:
    stosb      {draw it}
    add di,319
loop mree
mree4:
    pop di
    pop es
end;
end;

procedure fade;
var i,j:integer;
    r,g,b:byte;
begin
  for i:=0 to 255 do begin
      getpal(i,r,g,b);
      unfadered[i]:=r;
      unfadeblue[i]:=b;
      unfadegreen[i]:=g;
  end;
  for i:=0 to 63 do begin
      for j:=0 to 255 do begin
          getpal(j,r,g,b);
          if r>0 then dec(r);
          if g>0 then dec(g);
          if b>0 then dec(b);
          pal(j,r,g,b);
      end;
  end;
end;

procedure unfade;
var i,j:integer;
    r,g,b:byte;
begin
  for i:=0 to 63 do begin
      for j:=0 to 255 do begin
          getpal(j,r,g,b);
          if r<unfadered[j] then inc(r);
          if g<unfadegreen[j] then inc(g);
          if b<unfadeblue[j] then inc(b);
          pal(j,r,g,b);
      end;
  end;
end;




procedure box(x1,y1,x2,y2,col:integer;where:word);
begin
   line(x1,y1,x1,y2,col,where);
   horizontalline(x1,x2,y2,col,where);
   line(x2,y2,x2,y1,col,where);
   horizontalline(x1,x2,y1,col,where);
end;

procedure vdelay(howlong:integer);
var i:integer;
begin
  for i:=1 to howlong do waitretrace;
end;

procedure setupvmwgraph;
begin
  loadfont('8x8font.tb1');
  charheight:=8;
  fontseg:=seg(fontline[0]);
  fontoff:=ofs(fontline[0]);
end;

procedure setuptb1;
begin
  loadfont('tbfont.tb1');
  charheight:=8;
  fontseg:=seg(fontline[0]);
  fontoff:=ofs(fontline[0]);
end;


begin
end.
*/
