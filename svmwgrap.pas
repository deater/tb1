unit SVmwGraph;  {Super Vmw Graphics Unit}
{$X+}{$G+}
{$L graph32.obj}

INTERFACE

CONST VGA = $a000;

var unfadered:array[0..255] of byte;
    unfadeblue:array[0..255] of byte;
    unfadegreen:array[0..255] of byte;
    fontline:array[0..255] of byte;
    charset:array[0..255,0..15] of byte;
    charheight:byte;
    fontseg,fontoff:word;





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
IMPLEMENTATION

uses nvmwgraph;


function Mode13SavePicPacked(x1,y1,x2,y2,numcolors:integer;where:word;filename:string):byte;

{**** Saves an image, with corners x1,y1 and x2,y2
      and number of colors NUMCOLORS, and at segment
      where ($A000 for vga) to the paintpro file
      filename ****}
    {x2 y2 should be plus one (320,200 not 319,199}

var buffer:array[0..2] of byte;
    header:string[15];
    f:file;
    col,x,y,xsize,ysize,i,oldcol,numacross:integer;
    byte1,byte2,byte3,temp:byte;
    r,g,b:byte;
    second1,zz:integer;

procedure integer23bytes(inter1,inter2:integer);
var temp1,temp2,temp3:integer;
    temp:byte;
begin
  {asm
    mov ax,inter1
    shl ax,1
      shl ax,1        this assembly will accomplish
      shl ax,1        what the below will.  Basically
      shl ax,1        it takes two, 16 bit integers,
    mov temp1,ah      strips off the top 4 bits of each,
    sub ah,ah         (makin two, 12 bit integers) and
    shr ax,1          joins these into three 8 bit bytes
      shr ax,1
      shr ax,1         for this assmebly to work, however
      shr ax,1        temp1,temp2, etc must be byte, not
    mov temp,al       integer
    mov ax,inter2
    mov temp3,al
    shr ax,1
      shr ax,1
      shr ax,1
      shr ax,1
    mov ah,temp
    shl ax,1
      shl ax,1        in the below, shl is shift left
      shl ax,1           (in C the equivelant is <<)
      shl ax,1        and shr is shift right ( >> )
    mov temp2,ah
  end;

  }
  temp1:=(inter1  AND $7FFF);
  temp1:=temp1 shr 4;
  temp3:=(inter2 AND $7FFF);
  temp2:= ((inter1 and $000F) shl 4)+ ((inter2 AND $0F00) shr 8);

    buffer[0]:=temp1;
    buffer[1]:=temp2;
    buffer[2]:=temp3;
    blockwrite(f,buffer,1);
end;


begin
  Mode13SavePicPacked:=0;    {*Clear the errors}
  assign(f,filename);        {*Open the file}
  {$I-}
   rewrite(f,3);             {*Turn off i/o and open file}
  {$I+}
   if Ioresult<>0 then Mode13SavePicPacked:=1;
   if Ioresult<>0 then exit;    {*If error, file not found}
   header:='PAINTPROV6.0';
  for i:=0 to 3 do begin        {*Write header}
      buffer[0]:=ord(header[(i*3)+1]);
      buffer[1]:=ord(header[(i*3)+2]);
      buffer[2]:=ord(header[(i*3)+3]);
      blockwrite(f,buffer,1);
  end;

  xsize:=abs(x2-x1);             {*Write xsize and ysize}
  ysize:=abs(y2-y1);
  integer23bytes(xsize,ysize);
  integer23bytes(numcolors,0);   {*Write number of colors and a blank}

  for i:=0 to (numcolors-1) do    {*Save the pallete (24 bit, rgb)}
  begin
      getpal(i,buffer[0],buffer[1],buffer[2]);
      buffer[0]:=buffer[0]* 4;    {*Multiplied by 4 because}
      buffer[1]:=buffer[1] * 4;   {mode 13h is only 18 bit}
      buffer[2]:=buffer[2] * 4;
      blockwrite(f,buffer,1);
  end;
  zz:=(y2-y1);
  y:=y1;
  numacross:=0;                          {this does RLE encoding}
  second1:=0;                            {see doc file}
  oldcol:=getpixel(x1,y,where);
  while y<=y2 do begin
        x:=x1;
        while x<=x2 do begin
              col:=getpixel(x,y,where);
              if (col=oldcol) and (numacross<2046) then inc(numacross)
              else begin                        {*compresses 1 color stretches}
                   if numacross=1 then begin
                      if second1=0 then begin
                         second1:=oldcol+2048;
                      end
                      else begin
                           integer23bytes(second1,oldcol+2048);
                           second1:=0;
                      end

                   end
                   else begin
                        if second1<>0 then
                        integer23bytes(second1-2048,1);
                        integer23bytes(oldcol,numacross);
                        second1:=0;
                   end;
                   numacross:=1;
              end;
              inc(x);
              oldcol:=col;
        end;
        inc(y);
  end;
      integer23bytes(col,numacross);

  close(f);
end;






function Mode13LoadPicPacked(x1,y1:integer;where:word;LoadPal,LoadPic:boolean;FileName:string):byte;

{*Loads a paintpro image, filename, at location x1,y1
  to offset where (vga=$A000) and loadspal if LOADPAL=true
  Loadsapicture if Loadpic=true and returns error}

var buffer:array[0..299] of byte;
    result,buffpointer:integer;
    f:file;
    col,x,y,xsize,ysize,i,oldcol,
    numcolors,tempint1,tempint2,numacross:integer;
    header:string;
    lastread:boolean;
    tempbyte,r,g,b:byte;


procedure sixbytes2twoint;

var temp1,temp2,temp3:byte;
    int1,int2:integer;

begin
  temp1:=buffer[buffpointer];
  temp2:=buffer[buffpointer+1];
  temp3:=buffer[buffpointer+2];
  inc(buffpointer,3);
  if (buffpointer>=result) then
     if (result=300) then begin
        blockread(f,buffer,300,result);
        buffpointer:=0;
     end
     else lastread:=true;
  asm
    mov ah,temp1          {splits 3 bytes (24 bits)}
    mov al,temp2          {into two 12 bit integers}
    shr ax,4              {and then pads them into}
    mov int1,ax          {two 16 bit integers}
    mov ah,temp2
    mov al,temp3
    shl ah,4
    shr ah,4
     mov int2,ax
  end;
  tempint1:=int1;
  tempint2:=int2;
end;



begin
  lastread:=false;
  Mode13LoadPicPacked:=0;     {*Clear errors}
  header:='';
  buffpointer:=0;
  assign(f,filename);
  {$I-}
    reset(f,1);             {*Check that file is there}
  {$I+}
  if IOresult<>0 then begin       {*Error if not}
     Mode13LoadPicPacked:=1;
  end
  else begin

  blockread(f,buffer,300,result);

  for i:=1 to 9 do header:=concat(header,chr(buffer[i-1]));


  if header<> 'PAINTPROV' then Mode13LoadPicPacked:=2;
  if header<> 'PAINTPROV' then exit;    {*Error if not Paintpro header}

  header:='';

  header:=concat(header,chr(buffer[9]),chr(buffer[10]),chr(buffer[11]));

  if header<>'6.0' then Mode13LoadPicPacked:=3;   {*Error if wron version}
  if header<>'6.0' then exit;
  buffpointer:=12;

  sixbytes2twoint;
  xsize:=tempint1;
  ysize:=tempint2;
  sixbytes2twoint;
  numcolors:=tempint1;



  for i:=0 to (numcolors-1) do begin
      r:=(buffer[buffpointer] div 4);
      g:=(buffer[buffpointer+1] div 4);
      b:=(buffer[buffpointer+2] div 4);
  inc(buffpointer,3);
  if (buffpointer>=result) then
     if (result=300) then begin
        blockread(f,buffer,300,result);
        buffpointer:=0;
     end
     else

     lastread:=true;
  if loadpal then pal(i,r,g,b);
  end;



  


  x:=x1;
  y:=y1;
if loadpic=true then begin
   while (lastread=false) do begin
     sixbytes2twoint;

      if tempint1>2047 then begin
         putpixel(x,y,(tempint1-2048),where);
         inc(x);
         if x>=xsize+x1+1 then begin
            x:=x1; inc(y); end;
         putpixel(x,y,(tempint2-2048),where);
         inc(x);
         if x>=xsize+x1+1 then begin
            x:=x1; inc(y); end;
      end

      else begin
          col:=tempint1;
          numacross:=tempint2;
     while ((x+numacross)>=(xsize+x1+1)) do begin
           horizontalline(x,x1+xsize,y,col,where);
           numacross:=numacross-((xsize+1)-x);
           x:=x1;
           inc(y);
     end;
     if numacross<>0 then horizontalline(x,x+numacross,y,col,where);
       x:=x+numacross;

   end;
   end;
end;
close(f);
end;
end;

procedure cls32(col:byte;where:word);EXTERNAL;
procedure flipd320(source,dest:word);EXTERNAL;
procedure flipd240(hm,va,va2:word);EXTERNAL;
procedure flipd50(fromwhere,off1,whereto,off2:word);EXTERNAL;




Procedure SetMCGA;  { This procedure gets you into 320x200x256 mode. }
BEGIN
  asm
     mov        ax,0013h
     int        10h
  end;
END;

Procedure SetText;  { This procedure returns you to text mode.  }
BEGIN
  asm
     mov        ax,0003h
     int        10h
  end;
END;

Procedure LoadFont(namest:string);
var buff:array[0..15] of byte;
    f:file;
    i,fonty:integer;

begin
  assign(f,namest);
  {$I-}
   reset(f,16);
  {$I+}
   if Ioresult<>0 then writeln(#7);
   if Ioresult<>0 then exit;

  fonty:=0;
  while (not eof(F)) and (fonty<4097) do
     begin
       blockread(f,buff,1);
       for i:=0 to 15 do
           charset[fonty div 16,i]:=buff[i];
       inc(fonty,16);
     end;
  close(f);
end;


Procedure Putpixel (X,Y : Integer; Col : Byte; where:word);
  { This puts a pixel on the screen by writing directly to memory. }
BEGIN
  Asm
    push    ds
    push    es
    mov     ax,[where]
    mov     es,ax
    mov     bx,[X]
    mov     dx,[Y]
    push    bx                      {; and this again for later}
    mov     bx, dx                  {; bx = dx}
    mov     dh, dl                  {; dx = dx * 256}
    xor     dl, dl
    shl     bx, 1
    shl     bx, 1
    shl     bx, 1
    shl     bx, 1
    shl     bx, 1
    shl     bx, 1                   {; bx = bx * 64}
    add     dx, bx                  {; dx = dx + bx (ie y*320)}
    pop     bx                      {; get back our x}
    add     bx, dx                  {; finalise location}
    mov     di, bx
    {; es:di = where to go}
    xor     al,al
    mov     ah, [Col]
    mov     es:[di],ah
    pop     es
    pop     ds
  End;
END;

Procedure Putpixel240 (X,Y : Integer; Col : Byte; where:word);
BEGIN
  Asm
    push    ds
    push    es
    mov     ax,[where]
    mov     es,ax
    mov     bx,[y]
    shl     bx,4
    mov     dx,bx                      {; and this again for later}
    shl     bx, 1                  {; bx = dx}
    add     dx, bx                  {; dx = dx * 256}
    shl     bx,1
    add     dx,bx
    shl     bx, 1
    add     dx, bx
    add     dx,[x]                    {; bx = bx * 64}
                          {; dx = dx + bx (ie y*320)}
                          {; get back our x}
                          {; finalise location}
    mov     di, dx
    {; es:di = where to go}
    xor     al,al
    mov     ah, [Col]
    mov     es:[di],ah
    pop     es
    pop     ds
  End;
END;

function getpixel(x,y,where:word):byte;
begin
  getpixel:=mem[where:x+(y*320)];
end;

Procedure Pal(Col,R,G,B : Byte);
  { This sets the Red, Green and Blue values of a certain color }
Begin
   asm
      mov    dx,3c8h
      mov    al,[col]
      out    dx,al
      inc    dx
      mov    al,[r]
      out    dx,al
      mov    al,[g]
      out    dx,al
      mov    al,[b]
      out    dx,al
   end;
End;

procedure WaitRetrace; assembler;
  {  This waits for a vertical retrace to reduce snow on the screen }
label
  l1, l2;
asm
    mov dx,3DAh
l1:
    in al,dx
    and al,08h
    jnz l1
l2:
    in al,dx
    and al,08h
    jz  l2
end;

Procedure GetPal(Col : Byte; Var R,G,B : Byte);
  { This gets the Red, Green and Blue values of a certain color }
Var
   rr,gg,bb : Byte;
Begin
   asm
      mov    dx,3c7h
      mov    al,col
      out    dx,al

      add    dx,2

      in     al,dx
      mov    [rr],al
      in     al,dx
      mov    [gg],al
      in     al,dx
      mov    [bb],al
   end;
   r := rr;
   g := gg;
   b := bb;
end;




procedure outtextlineover(st:string;x,y:integer;col,background:byte;line:integer;where:word);
label gus1,gus4,gus5,gus7,gus8,gus3;
var i,len:word;

begin
  len:=length(St);
  for i:=1 to (len div 2)+1 do begin
      fontline[(i-1)*2]:=charset[ord(st[(i*2)]),line];
      fontline[((i-1)*2)+1]:=charset[ord(st[(i*2)-1]),line];
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
   cmp bx,02
   jnge gus4
   mov cx,16
   lodsw
   jmp gus5
gus4:
   mov cx,8
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



procedure outtextline(st:string;x,y:integer;col,background:byte;line:integer;where:word);
label gus1,gus4,gus6,gus5,gus7,gus8,gus3;
var i,len:word;

begin
  len:=length(St);
  for i:=1 to (len div 2)+1 do begin
      fontline[(i-1)*2]:=charset[ord(st[(i*2)]),line];
      fontline[((i-1)*2)+1]:=charset[ord(st[(i*2)-1]),line];
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

   cmp bx,02

   jnge gus4
   mov cx,16
   lodsw
   jmp gus5
gus4:
   mov cx,8
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


procedure outtextxy(st:string;x,y,col,background:integer;where:word;overwrite:boolean);
label l0105;
var l,len,i,xadd,yadd,lineon:integer;
    n,k,o,min,max,qwerty:byte;
begin
  for lineon:=0 to (charheight-1) do
      if overwrite then
         outtextlineover(st,x,y+lineon,col,background,lineon,where)
      else
         outtextline(st,x,y+lineon,col,background,lineon,where);

end;



procedure OutSmallTextLineOver(st:string;x,y:integer;col,background:byte;line:integer;where:word);
label gus1,gus4,gus5,gus7,gus8,gus3;
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
