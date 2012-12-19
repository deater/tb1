unit NVMWgraph;  {the new vmw graph unit}
{$X+} {$G+}
INTERFACE

CONST VGA = $a000;

var unfadered:array[0..255] of byte;
    unfadeblue:array[0..255] of byte;
    unfadegreen:array[0..255] of byte;
    fontline:array[0..255] of byte;
    charset:array[0..255,0..15] of byte;
    charheight:byte;
    fontseg,fontoff:word;

Procedure SetMCGA;
Procedure SetText;  { This procedure returns you to text mode.  }
Procedure LoadFont(namest:string);
Procedure Cls (Col : Byte; Where:word);
Procedure Putpixel (X,Y : Integer; Col : Byte; where:word);
Procedure Putpixel240 (X,Y : Integer; Col : Byte; where:word);
function getpixel(x,y,where:word):byte;
Procedure Pal(Col,R,G,B : Byte);
Procedure GetPal(Col : Byte; Var R,G,B : Byte);
procedure WaitRetrace;
procedure flip(source,dest:Word);
procedure outtextlineover(st:string;x,y:integer;col,background:byte;line:integer;where:word);
procedure outtextline(st:string;x,y:integer;col,background:byte;line:integer;where:word);
procedure outtextxy(st:string;x,y,col,background:integer;where:word;overwrite:boolean);
procedure outsmalltextlineover(st:string;x,y:integer;col,background:byte;line:integer;where:word);
procedure outsmalltextline(st:string;x,y:integer;col,background:byte;line:integer;where:word);
procedure outsmalltextxy(st:string;x,y,col,background:integer;where:word;overwrite:boolean);
function sgn(a:real):integer;
procedure line(a,b,c,d,col:integer;where:word);
procedure horizontalline(FromX1,ToX2,AtY,col:integer;where:word);
procedure verticalline(FromY1,ToY2,AtX,col:integer;where:word);
procedure fade;
procedure unfade;
function SavePicSuperPacked(x1,y1,x2,y2,maxcolors:integer;where:word;filename:string):byte;
function LoadPicSuperPacked(x1,y1:integer;where:word;LoadPal,LoadPic:boolean;FileName:string):byte;
function oldloadpicsuperpacked(x1,y1:integer;where:word;fil:string):byte;
procedure box(x1,y1,x2,y2,col:integer;where:word);
procedure vdelay(howlong:integer);
procedure setupvmwgraph;
procedure setuptb1;

{Errors for save and load:
   0=No error
   1=File Not Found
   2=Improper file type (No PAINTPROVX.XX header)
   3=Improper version
   4=Too Many Colors
   5=Picture Too Big
 }


IMPLEMENTATION

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

Procedure Cls (Col : Byte; Where:word);
   { This clears the screen to the specified color }
BEGIN
     asm
        push    es
        mov     cx, 32000;
        mov     es,[where]
        xor     di,di
        mov     al,[col]
        mov     ah,al
        rep     stosw
        pop     es
     End;
END;

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




procedure flip(source,dest:Word);
  { This copies the entire screen at "source" to destination }
begin
  asm
    push    ds
    mov     ax, [Dest]
    mov     es, ax
    mov     ax, [Source]
    mov     ds, ax
    xor     si, si
    xor     di, di
    mov     cx, 32000
    rep     movsw
    pop     ds
  end;
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


function SavePicSuperPacked(x1,y1,x2,y2,maxcolors:integer;where:word;filename:string):byte;

var buffer:array[0..2] of byte;
    f:file;
    col,x,y,xsize,ysize,i,oldcol,numacross:integer;
    header:string;
    byte1,byte2,byte3,temp:byte;
    r,g,b:byte;

procedure integer23bytes(inter1,inter2:integer);
var temp1,temp2,temp3,temp:byte;
begin
  asm
    mov ax,inter1
    shl ax,1
      shl ax,1
      shl ax,1
      shl ax,1
    mov temp1,ah
    sub ah,ah
    shr ax,1
      shr ax,1
      shr ax,1
      shr ax,1
    mov temp,al
    mov ax,inter2
    mov temp3,al
    shr ax,1
      shr ax,1
      shr ax,1
      shr ax,1
    mov ah,temp
    shl ax,1
      shl ax,1
      shl ax,1
      shl ax,1
    mov temp2,ah
  end;
    buffer[0]:=temp1;
    buffer[1]:=temp2;
    buffer[2]:=temp3;
    blockwrite(f,buffer,1);
end;

begin
  header:='PAINTPROV5.0';
  savepicsuperpacked:=0;
  assign(f,filename);
  {$I-}
   rewrite(f,3);
  {$I+}
   if Ioresult<>0 then savepicsuperpacked:=1;
   if Ioresult<>0 then exit;
  for i:=0 to 3 do begin
      buffer[0]:=ord(header[(i*3)+1]);
      buffer[1]:=ord(header[(i*3)+2]);
      buffer[2]:=ord(header[(i*3)+3]);
      blockwrite(f,buffer,1);
  end;

  for i:=0 to (maxcolors-1) do
  begin
      getpal(i,buffer[0],buffer[1],buffer[2]);
      blockwrite(f,buffer,1);
  end;

  xsize:=abs(x2-x1);
  ysize:=abs(y2-y1);
  integer23bytes(xsize,ysize);
  integer23bytes(maxcolors,0);

  for y:=y1 to y2 do begin
      oldcol:=getpixel(x1,y,where);  numacross:=1;
      for x:=(x1+1) to x2 do begin
          col:=getpixel(x,y,where);
          if col=oldcol then inc(numacross)
          else begin
             integer23bytes(oldcol,numacross);
             numacross:=1;
          end;
          oldcol:=col;
      end;
      integer23bytes(col,numacross);
  end;
  close(f);
end;

function LoadPicSuperPacked(x1,y1:integer;where:word;LoadPal,LoadPic:boolean;FileName:string):byte;

var buffer:array[0..299] of byte;
    result,buffpointer:integer;
    f:file;
    col,x,y,xsize,ysize,i,oldcol,
    maxcolors,tempint1,tempint2,numacross:integer;
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
    mov ah,temp1
    mov al,temp2
    shr ax,4
    mov int1,ax
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
  loadpicsuperpacked:=0;
  header:='';
  buffpointer:=0;
  assign(f,filename);
  {$I-}
    reset(f,1);
  {$I+}
  if IOresult<>0 then LoadPicSuperPacked:=1;
  if IOresult<>0 then exit;

  blockread(f,buffer,300,result);

  for i:=1 to 9 do header:=concat(header,chr(buffer[i-1]));


  if header<> 'PAINTPROV' then LoadPicSuperPacked:=2;
  if header<> 'PAINTPROV' then exit;

  header:='';

  header:=concat(header,chr(buffer[9]),chr(buffer[10]),chr(buffer[11]));

  if header<>'5.0' then LoadPicSuperPacked:=3;
  if header<>'5.0' then exit;
  buffpointer:=12;

  for i:=0 to 255 do begin
      r:=buffer[buffpointer];
      g:=buffer[buffpointer+1];
      b:=buffer[buffpointer+2];
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



  sixbytes2twoint;
  xsize:=tempint1;
  ysize:=tempint2;
  sixbytes2twoint;
  maxcolors:=tempint1;

  {if maxcolors>256 then error=4}

  x:=x1;
  y:=y1;
if loadpic=true then begin
   while (lastread=false) do begin
     sixbytes2twoint;
     col:=tempint1;
     numacross:=tempint2;
     horizontalline(x,x+numacross,y,col,where);
     {  for i:=x to (x+numacross) do putpixel(i,y,col,where);}
     if (x+numacross)>xsize+x1 then begin
        inc(y);
        x:=x1;
        numacross:=0;
     end;
     x:=x+numacross;
   end;
end;
close(f);
end;

function oldloadpicsuperpacked(x1,y1:integer;where:word;fil:string):byte;

var buffer:array[0..255] of byte;
    f:file;
    col,x,y,xsize,ysize,buffpointer,i,oldcol,
    tempint1,tempint2,numacross:integer;
    header:string;
    lastread,done:boolean;
    temp7:string;

procedure readbuffer;
begin
  blockread(f,buffer,1);
  if buffer[255]=90 then lastread:=true;
end;

procedure sixbytes2twoint;
var temp1,temp2,temp3:byte;
    int1,int2:integer;
begin
  if buffpointer>=253 then begin
     readbuffer;
     buffpointer:=0;
  end;
  if not done then begin
     temp1:=buffer[buffpointer];
     temp2:=buffer[buffpointer+1];
     temp3:=buffer[buffpointer+2];
     if (temp1=255) and (temp2=255) and (temp3=255) then
        done:=true;
     inc(buffpointer,3);
  asm
    mov ah,temp1
    mov al,temp2
    shr ax,1
      shr ax,1
      shr ax,1
      shr ax,1
    mov int1,ax
    mov ah,temp2
    mov al,temp3
      shl ah,4
      shr ah,4
    mov int2,ax
  end;
  tempint1:=int1;
  tempint2:=int2;
end;
end;
label ender;
begin
  done:=false;
  header:='PAINTPROV4.00';
  assign(f,fil);
  reset(f,256);
  readbuffer;
  buffpointer:=0;
  temp7:=header;
  for i:=1 to 13 do temp7[i]:=chr(buffer[i-1]);
  if temp7<>header then begin
     oldloadpicsuperpacked:=01;
     exit;
  end;
  buffpointer:=13;
  sixbytes2twoint;
  xsize:=tempint1;
  ysize:=tempint2;
  sixbytes2twoint;

  x:=x1;
  y:=y1;
repeat
  sixbytes2twoint;
  if done then goto ender;
  col:=tempint1;
  numacross:=tempint2;
  for i:=x to (x+numacross) do putpixel(i,y,col,where);
  if (x+numacross)>xsize+x1 then begin
     inc(y);
     x:=x1;
     numacross:=0;
  end;
  x:=x+numacross;
  ender:
until done=true;
close(f);
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
