unit VMWgraph;
{$X+} {$G+}
INTERFACE

const charset:array[0..255,0..7] of byte=
   ((0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
{32' '}(0,0,0,0,0,0,0,0),($78,$78,$78,$30,$30,$00,$30,$00),($34,$34,$00,$00,$00,$00,$00,$00),
{35'#'}($28,$7C,$28,$28,$7C,$28,$28,$00),($10,$7C,$50,$7C,$14,$7C,$10,$00),
{37'$'}($00,$62,$64,$08,$10,$26,$46,$00),($00,$38,$40,$20,$54,$48,$34,$00),
{39'''}($30,$30,$10,$00,$00,$00,$00,$00),($08,$10,$20,$20,$20,$10,$08,$00),
{41')'}($20,$10,$08,$08,$08,$10,$20,$00),($92,$54,$38,$FE,$38,$54,$92,$00),
{43'+'}($00,$10,$10,$7C,$10,$10,$00,$00),($00,$00,$00,$00,$00,$10,$10,$20),
{45'-'}($00,$00,$00,$7C,$00,$00,$00,$00),($00,$00,$00,$00,$00,$30,$30,$00),
{47'/'}($02,$06,$0C,$18,$30,$E0,$C0,$00),($FE,$C6,$CE,$DA,$F2,$E2,$FE,$00),
{49'1'}($30,$70,$B0,$30,$30,$30,$FC,$00),($78,$84,$08,$10,$20,$40,$FC,$00),
{51'3'}($78,$84,$04,$3C,$04,$84,$78,$00),($C4,$C4,$C4,$FC,$04,$04,$04,$00),
{53'5'}($FC,$C0,$C0,$F8,$04,$04,$F8,$00),($7C,$C4,$C0,$F8,$C4,$C4,$F8,$00),
{55'7'}($FC,$84,$08,$10,$10,$20,$20,$00),($78,$C4,$C4,$78,$C4,$C4,$78,$00),
{57'9'}($7C,$C4,$C4,$7C,$04,$04,$04,$00),($00,$00,$30,$30,$00,$30,$30,$00),
{59';'}($00,$00,$30,$30,$00,$10,$10,$20),($18,$30,$60,$C0,$60,$30,$18,$00),
{61'='}($00,$00,$7C,$00,$7C,$00,$00,$00),($C0,$60,$30,$18,$30,$60,$C0,$00),
{63'?'}($78,$CC,$98,$30,$30,$00,$30,$00),($FC,$84,$BC,$A4,$BC,$80,$FC,$00),
{65'A'}($FE,$C2,$C2,$FE,$C2,$C2,$C2,$00),($F8,$C8,$C8,$FC,$C2,$C2,$FC,$00),
{67'C'}($FE,$C2,$C0,$C0,$C0,$C0,$FE,$00),($F8,$C4,$C2,$C2,$C2,$C2,$FC,$00),
{69'E'}($FE,$C0,$C0,$FE,$C0,$C0,$FE,$00),($FE,$C0,$C0,$FE,$C0,$C0,$C0,$00),
{71'G'}($FE,$C2,$C0,$CE,$C2,$C2,$FE,$00),($C2,$C2,$C2,$FE,$C2,$C2,$C2,$00),
{73'I'}($FE,$30,$30,$30,$30,$30,$FE,$00),($0E,$02,$02,$02,$C2,$64,$38,$00),
{75'K'}($C2,$C2,$CC,$F0,$CC,$C2,$C2,$00),($C0,$C0,$C0,$C0,$C0,$C0,$FE,$00),
{77'M'}($C2,$E6,$EE,$DA,$D2,$C2,$C2,$00),($C2,$E2,$F2,$DA,$CA,$C6,$C6,$00),
{79'O'}($FE,$C2,$C2,$C2,$C2,$C2,$FE,$00),($FE,$C2,$C2,$FE,$C0,$C0,$C0,$00),
{81'Q'}($FE,$C2,$C2,$C2,$CA,$C6,$FE,$00),($FE,$C2,$C2,$FE,$C8,$C4,$C2,$00),
{83'S'}($FE,$C0,$C0,$FE,$02,$82,$FE,$00),($FE,$30,$30,$30,$30,$30,$30,$00),
{85'U'}($C2,$C2,$C2,$C2,$C2,$C2,$FE,$00),($C2,$C2,$C4,$64,$68,$38,$30,$00),
{87'W'}($C2,$C2,$C2,$D2,$F2,$FA,$C6,$00),($C2,$C6,$3C,$18,$3C,$C6,$C2,$00),
{89'Y'}($C2,$C2,$64,$68,$30,$30,$30,$00),($FE,$04,$08,$30,$60,$C0,$FE,$00),
{91'['}($F8,$C0,$C0,$C0,$C0,$C0,$F8,$00),($C0,$E0,$30,$18,$0C,$06,$02,$00),
{93']'}($1E,$02,$02,$02,$02,$02,$1E,$00),($10,$28,$44,$00,$00,$00,$00,$00),
{95'_'}($00,$00,$00,$00,$00,$00,$FF,$00),($08,$04,$02,$00,$00,$00,$00,$00),
{97'a'}($00,$00,$7C,$04,$7C,$44,$7E,$00),($C0,$40,$40,$7C,$44,$44,$FC,$00),
{99'c'}($00,$00,$00,$7C,$40,$40,$7C,$00),($06,$04,$04,$7C,$44,$44,$7E,$00),
{101e'}($00,$00,$7C,$44,$7C,$40,$7C,$00),($10,$28,$20,$70,$20,$20,$70,$00),
{103g'}($00,$00,$04,$7C,$44,$7C,$04,$7C),($E0,$40,$40,$7C,$44,$44,$EE,$00),
{105i'}($00,$10,$00,$30,$10,$10,$78,$00),($00,$04,$00,$0C,$04,$04,$44,$3C),
{107k'}($C0,$40,$40,$58,$60,$50,$48,$00),($30,$10,$10,$10,$10,$10,$78,$00),
{109m'}($00,$00,$00,$E8,$54,$54,$54,$00),($00,$00,$00,$F0,$48,$48,$48,$00),
{111o'}($00,$00,$00,$7C,$44,$44,$7C,$00),($00,$00,$00,$FC,$44,$7C,$40,$E0),
{113q'}($00,$00,$00,$7E,$44,$7C,$04,$0E),($00,$00,$00,$28,$34,$20,$70,$00),
{115s'}($00,$00,$7C,$40,$7C,$04,$7C,$00),($00,$20,$70,$20,$20,$28,$38,$00),
{117u'}($00,$00,$00,$C8,$48,$48,$7C,$00),($00,$00,$00,$44,$44,$28,$10,$00),
{119w'}($00,$00,$00,$42,$42,$5A,$66,$00),($00,$00,$66,$34,$18,$34,$66,$00),
{121y'}($00,$00,$00,$44,$44,$7C,$04,$7C),($00,$00,$7C,$08,$10,$20,$7C,$00),
{123|'}($30,$20,$20,$40,$20,$20,$30,$00),($10,$10,$10,$00,$10,$10,$10,$00),
{125~'}($18,$08,$08,$04,$08,$08,$18,$00),($3A,$6C,$00,$00,$00,$00,$00,$00),
{127''}($00,$54,$2A,$54,$2A,$54,$2A,$00),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),
    (0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0));
CONST VGA = $a000;

var unfadered:array[0..255] of byte;
    unfadeblue:array[0..255] of byte;
    unfadegreen:array[0..255] of byte;

Procedure SetMCGA;
Procedure SetText;  { This procedure returns you to text mode.  }
Procedure Cls (Col : Byte; Where:word);
Procedure Putpixel (X,Y : Integer; Col : Byte; where:word);
function getpixel(x,y,where:word):byte;
Procedure Pal(Col,R,G,B : Byte);
Procedure GetPal(Col : Byte; Var R,G,B : Byte);
procedure egaputpix(x,y,col,where:word);
procedure WaitRetrace;
procedure flip(source,dest:Word);
procedure outtext(x,y,rowochar,col,background:integer;num:byte;where:word;overwrite:boolean);
procedure egaouttext(x,y,col:integer;num:byte;where:word;overwrite:boolean);
procedure outtextxy(st:string;x,y,rowochar,col,background:integer;where:word;overwrite:boolean);
procedure egaouttextxy(st:string;x,y,col:integer;where:word;overwrite:boolean);
function sgn(a:real):integer;
procedure line(a,b,c,d,col:integer;where:word);
procedure fade;
procedure unfade;
procedure SavePicSuperPacked(x1,y1,x2,y2,maxcolors:integer;filename:string);
function loadpicsuperpacked(x1,y1:integer;where:word;fil:string):byte;
procedure box(x1,y1,x2,y2,col:integer;where:word);
procedure vdelay(howlong:integer);

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

procedure egaputpix(x,y,col,where:word);

VAR red,blue,green,inte,P1,P2:byte;
    col2:word;

procedure semiput(x,y:word;col,p1,p2:byte;where:word);
var temp:word;
    q3:word;
    old,q1,q2,qz:byte;
    label looper,loop2,loop3,looper2;
begin
  asm
    mov ax,where    {Set Where screen is}
    mov es,ax
    mov ax,y        {The following is 80*y}
    mov dx,y        {which makes (640*y)/8 uneccessary}
    shl ax,6
    shl dx,4
    add ax,dx
    mov temp,ax     {saves temporary address}
    mov bx,x        {gets X DIV 8 and adds it to temp}
    shl bx,5
    mov dl,bh
    mov dh,00
    sub bh,bh
    shr bx,5
    mov q2,bl       {gets X MOD 8 for actual pixel}
    mov ax,temp
    add ax,dx
    mov temp,ax
    {inegaports}
    mov dx,$3Ce     {Sets us bitplane for input}
    mov al,4
    out dx,al
    mov dx,$3Cf
    mov al,p1
    out dx,al
    {old}           {Gets old pixel values}
      mov di,temp
      mov al,es:[di]
      mov ah,00
      mov cl,q2
      mov ch,00
      JCXZ loop2
      looper:
        shl ax,1
      loop looper
      loop2:
      mov dh,ah
      mov ah,00
      shl ax,1
      mov ah,col
      shr ax,1
      mov ah,dh
      mov cl,q2
      mov ch,00
      jcxz loop3
      looper2:
        shr ax,1
      loop looper2
      loop3:
    
    mov qz,al
           {Logically gets old and new}
    {out}
      mov dx,$3C4
      mov al,2
      out dx,al
      mov dx,$3C5
      mov al,p2
      out dx,al        {rewrites pixel}
    {mem}
      mov di,temp
      mov al,qz
      mov es:[di],al

    end;
end;
begin
  asm
    mov bx,col
    shl bx,4
    shl bx,1
    mov inte,bh
    sub bh,bh
    shl bx,1
    mov red,bh
    sub bh,bh
    shl bx,1
    mov green,bh
    sub bh,bh
    shl bx,1
    mov blue,bh
  end;
    p1:=0; p2:=1; col2:=0;
      if blue= 1 then col2:=1;
    semiput(x,y,col2,p1,p2,vga);
    p1:=1; p2:=2; col2:=0;
      if green=1 then col2:=1;
    semiput(x,y,col2,p1,p2,vga);
    p1:=2; p2:=4; col2:=0;
       if red=1 then col2:=1;
    semiput(x,y,col2,p1,p2,vga);
    p1:=3; p2:=8; col2:=0;
      if inte=1 then col2:=1;
    semiput(x,y,col2,p1,p2,vga);

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



procedure outtext(x,y,rowochar,col,background:integer;num:byte;where:word;overwrite:boolean);
label l0105;          {^-15=all, 0-7= that row}
var n,k,l,o,min,max,qwerty:byte;
begin
  if rowochar=15 then begin min:=0; max:=7; end
     else begin min:=rowochar; max:=rowochar; end;
  qwerty:=0;

  for n:=min to max do begin        {Loop through charachter}
      if rowochar=15 then qwerty:=n;
      o:=charset[num,n];            {array in Hex}
      for l:=0 to 7 do begin
        asm                 {Hex to binary decoder}
           mov bl,o             {mov hex into o}
           mov dl,00
           rcl bl,1             {Rotate O left into carry}
           adc dl,00h           {Add the carry into dl}
           mov o,bl             {Save O}
           mov k,dl             {Copy Dl (if carry) into k}

        end;
        if k=1 then putpixel(x+l,y+qwerty,col,where)
           else if overwrite then putpixel(x+l,y+qwerty,background,where);

      end;
  end;
end;
procedure egaouttext(x,y,col:integer;num:byte;where:word;overwrite:boolean);
label l0105;
var n,k,l,o:byte;
begin
  for n:=0 to 7 do begin        {Loop through charachter}
      o:=charset[num,n];            {array in Hex}
      for l:=0 to 7 do begin
        asm                 {Hex to binary decoder}
           mov bl,o             {mov hex into o}
           mov dl,00
           rcl bl,1             {Rotate O left into carry}
           adc dl,00h           {Add the carry into dl}
           mov o,bl             {Save O}
           mov k,dl             {Copy Dl (if carry) into k}
        end;
        if k=1 then egaputpix(x+l,y+n,col,where)
           else if overwrite then egaputpix(x+l,y+n,0,where);

      end;
  end;
end;
procedure outtextxy(st:string;x,y,rowochar,col,background:integer;where:word;overwrite:boolean);
var l,len,i,xadd,yadd:integer;
begin
  xadd:=0; yadd:=y;
  len:=length(St);
  l:=1;
  repeat
    outtext(x+xadd,yadd,rowochar,col,background,ord(st[l]),where,overwrite);
    inc(xadd,8);
    inc(l,1);
    if (xadd>311)  then begin xadd:=0; inc(yadd,8) end;
    if (ord(st[l]))=13 then begin xadd:=0; inc(yadd,8); inc(l,1); end;
  until l>=(len+1);

end;
procedure egaouttextxy(st:string;x,y,col:integer;where:word;overwrite:boolean);
var l,len,i,xadd,yadd:integer;
begin
  xadd:=0; yadd:=y;
  len:=length(St);
  l:=1;
  repeat
    egaouttext(x+xadd,yadd,col,ord(st[l]),where,overwrite);
    inc(xadd,8);
    inc(l,1);
    if (xadd>311)  then begin xadd:=0; inc(yadd,8) end;
    if (ord(st[l]))=13 then begin xadd:=0; inc(yadd,8); inc(l,1); end;
  until l=(len+1);

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
procedure SavePicSuperPacked(x1,y1,x2,y2,maxcolors:integer;filename:string);

var buffer:array[0..255] of byte;
    f:file;
    col,x,y,xsize,ysize,buffpointer,i,oldcol,numacross:integer;
    header:string;
   byte1,byte2,byte3,temp:byte;

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
    byte1:=temp1;
    byte2:=temp2;
    byte3:=temp3;
end;

procedure writebuff2disk;
begin
  blockwrite(f,buffer,1);
  buffpointer:=0;
end;

procedure writetemps2buffer;
begin
  if buffpointer>=253 then writebuff2disk;
  buffer[buffpointer]:=byte1;
  buffer[buffpointer+1]:=byte2;
  buffer[buffpointer+2]:=byte3;
  inc(buffpointer,3);
end;



begin
  header:='PAINTPROV4.00';
  assign(f,filename);
  rewrite(f,256);
  buffpointer:=0;
  for i:=1 to 13 do buffer[i-1]:=ord(header[i]);
  buffpointer:=13;
  xsize:=abs(x2-x1);
  ysize:=abs(y2-y1);
  integer23bytes(xsize,ysize);
  writetemps2buffer;
  integer23bytes(maxcolors,0);
  writetemps2buffer;


  for y:=y1 to y2 do begin
    oldcol:=getpixel(x1,y,vga);  numacross:=1;
      for x:=(x1+1) to x2 do begin
          col:=getpixel(x,y,vga);
          if col=oldcol then inc(numacross)
          else begin
             integer23bytes(oldcol,numacross);
             writetemps2buffer;
             numacross:=1;
          end;
          oldcol:=col;
      end;
      integer23bytes(col,numacross);
      writetemps2buffer;
  end;
  for x:=buffpointer to 254 do buffer[x]:=255;
  buffer[255]:=90;
  writebuff2disk;
  close(f);
end;

function loadpicsuperpacked(x1,y1:integer;where:word;fil:string):byte;

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
    shl ah,1
      shl ah,1
      shl ah,1
      shl ah,1
      shr ah,1
      shr ah,1
      shr ah,1
      shr ah,1
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
     loadpicsuperpacked:=01;
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
   line(x1,y2,x2,y2,col,where);
   line(x2,y2,x2,y1,col,where);
   line(x2,y1,x1,y1,col,where);
end;

procedure vdelay(howlong:integer);
var i:integer;
begin
  for i:=1 to howlong do waitretrace;
end;

begin
end.
