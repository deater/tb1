program TOM_BOMB_EM_AND_INVASION_OF_INANIMATE_OBJECTS;
        {by Vincent Weaver....21085-4706}

uses nvmwgraph,crt,detect,smix,dos;

{$G+}
const
  {Shape Table Values for Game}
    shape1off  = 8000;
    shape2off  = 8400;
    shape3off  = 8800;
    shape4off  = 9200;
    shape5off  = 9600;
    shape6off  = 10000;
    shape7off  = 10400;
    shape8off  = 10800;
    shape9off  = 11200;
    shape10off = 11600;
    shape11off = 12000;
    shape12off = 12400;
    shape13off = 12800;
    shape14off = 13200;
    shape15off = 13600;
    shape16off = 14000;
    shape17off = 14400;
    shape18off = 14800;
    shape19off = 15200;
    shape20off = 15600;
    bigship1off = 0;
    bigship2off = 1600;
    bigship3off = 3200;
    tbnoff = 0;
    tbeoff = 150;
    tbsoff = 300;
    tbwoff = 450;
    shapearray :
         array [1..20] of word =(8000,8400,8800,9200,
                                 9600,10000,10400,10800,
                                 11200,11600,12000,12400,
                                 12800,13200,13600,14000,
                                 14400,14800,15200,15600);
    shape2array :
         array [0..39] of word =(8000,8200,8400,8600,8800,
                                 9000,9200,9400,9600,9800,
                                 10000,10200,10400,10600,10800,
                                 11000,11200,11400,11600,11800,
                                 12000,12200,12400,12600,12800,
                                 13000,13200,13400,13600,13800,
                                 14000,14200,14400,14600,14800,
                                 15000,15200,15400,15600,15800);
    shape3array :
         array [0..79] of word =(8000,8100,8200,8300,8400,
                                 8500,8600,8700,8800,8900,
                                 9000,9100,9200,9300,9400,
                                 9500,9600,9700,9800,9900,
                                 10000,10100,10200,10300,10400,
                                 10500,10600,10700,10800,10900,
                                 11000,11100,11200,11300,11400,
                                 11500,11600,11700,11800,11900,
                                 12000,12100,12200,12300,12400,
                                 12500,12600,12700,12800,12900,
                                 13000,13100,13200,13300,13400,
                                 13500,13600,13700,13800,13900,
                                 14000,14100,14200,14300,14400,
                                 14500,14600,14700,14800,14900,
                                 15000,15100,15200,15300,15400,
                                 15500,15600,15700,15800,15900);



type ScreenType = array [0..3999] of byte;  {For Ansi Loading}

     Obstruction = record
                   x,y:integer;
                   shooting,dead,exploding:boolean;
                   explodeprogress:byte;
                   howmanyhits:integer;
                   kind,lastshot:byte;
                end;


     EnemyInfo = record
               x,y:integer;
               kind:byte;
               out,exploding,boundarycheck,dead:boolean;
               explodprogress:byte;
               minx,maxx,xspeed,yspeed:integer;
               hitsneeded:integer;
             end;

     Virtual = Array [1..64000] of byte;  { The size of our Virtual Screen }
     VirtPtr = ^Virtual;                  { Pointer to the virtual screen }

     ShapeTable = Array [0..16000] of byte;
     ShapePtr = ^ShapeTable;


var ho,mi,se,s100:word;
    background:array[0..200,0..11] of byte;
    tempsec:integer;
    flames:boolean;
    inte:integer;
    dev,mix,stat,pro,loop : integer;
    md : string;
    howmuchscroll:byte;
    filestuff:searchrec;  {for load game}
    savegamef:text;
    ondisk:string;
    tempch:char;

VAR Virscr,VirScr2,VirScr3 : VirtPtr;   { Our first Virtual screen }
    ShapeTable1:ShapePtr;
    num:byte;
    Vaddr,Vaddr2,Vaddr3 : Word;                      { The segment of our 2nd virt. screen}
    ShapeAddr:word;                     { The segment of the shape table}
    ShapeOfs:word;
    backofs,backseg:word;
    ourpal : Array [0..255,1..3] of byte; { A virtual pallette }
    enemy:array[0..5] of enemyinfo;
    passive:array[0..30] of obstruction;
    numpassive:integer;
    Soundp : Pointer;
    Check : BOOLEAN;

    hiscore,level,shields:integer;
    beginscore,beginshields:integer;
    score:longint;
    axel_und_carmen:boolean;
    tempi,tempj:integer;
    scorest:string[8];
    soundeffects:boolean;
    lowscore:integer;
    it,hiname:string[10];
    shipxadd,shipyadd,shipx,shipy,shipspeed,shipframe:integer;


var grapherror:byte;
    temp:array[1..3] of byte;
    i,j:byte;
    x,y,barpos:integer;
    screen:screentype absolute $B800:0000;
    ch:char;

procedure fillblock(xx1,yy1,xx2,yy2:integer;colo:byte;where:word);
label gus3,gus;
begin
   asm
     push es             {Save Variables}
       push ds
     mov ax,where      {Load where output Goes}
       mov es,ax
     mov ah,colo       {Color To Block}
       mov al,colo
     mov bx,yy1
     mov dx,bx         {Multiply by 320 to find start row}
       shl dx,5
       mov cx,dx
       shl dx,1
       shl cx,3
       add dx,cx
     add dx,xx1       {Add x to get column}
     mov di,dx        {Load into register}

     mov dx,[xx2]     {Calculate width and store in dx}
       sub dx,[xx1]
     mov bx,320       {Calcualte and store skipover}
       sub bx,dx
       push bx
     mov bx,[yy2]     {calculate height and store in bx}
       sub bx,[yy1]

gus3:
     mov cx,dx
     shr cx,1           {Divide width in half}
       jnc gus        {If even, jump}

     stosb            {then store odd byte}
                      {Jump Past}
gus:
      rep stosw       {Its even.. so store words}

    pop cx            {restore and add skipover}
    add di,cx
    push cx
    dec bx            {decrement y line}
    jnz gus3          {if not zero, go back}
    pop cx
    pop ds            {restore registers}
    pop es
   end;
end;

procedure ReadConfigFile;
var ff:text;
begin
(*  {$I-}
     assign(ff,'config.tb1');
     reset(ff);
     close(ff);
  {$I+}
     if IOresult<>0 then begin
        writeln('Warning: Missing or Invalid "config.tb1" file');
        writeln;
        writeln('Please run the "tbsetup" utility included to set up your computer.  Thank You.');
        halt(5);
     end;
     assign(ff,'config.tb1');
     reset(ff);
       readln(ff,Xsoundeffects);
       readln(ff,Xsbsound);
       readln(ff,Xbaseio);
       readln(ff,Xirq);
       readln(ff,Xdma);
     close(ff); *)
end;



function menuread:char;
var chtemp,ch2:char;
begin
  repeat until keypressed;
  ch2:=#0;
  chtemp:=readkey;
  if chtemp=chr(0) then ch2:=readkey;
  chtemp:=upcase(chtemp);
  if (ord(chtemp)<10) and (ord(chtemp)<128) then begin
     if ch2='H' then chtemp:='ß';  {up}
     if ch2='M' then chtemp:='Þ';  {right}
     if ch2='P' then chtemp:='Ü';  {down}
     if ch2='K' then chtemp:='Ý';  {left}
     if ch2=';' then chtemp:='¨';  {f1}
     if ch2='I' then chtemp:='ô';  {pgup}
     if ch2='Q' then chtemp:='õ';  {pgdown}
  end;
  menuread:=chtemp;
end;


procedure coolbox(x1,y1,x2,y2:integer;fill:boolean;page:word);
begin
   for i:=0 to 5 do box(x1+i,y1+i,x2-i,y2-i,31-i,page);
   if fill then for i:=y1+5 to y2-5 do
           horizontalline(x1+5,x2-5,i,7,page);
end;


Procedure ShutDown;
   { This frees the memory used by the virtual screen }
BEGIN
  FreeMem (VirScr,64000);
  FreeMem (VirScr2,64000);
  FreeMem (VirScr3,16800);
  FreeMem (ShapeTable1,16000);
END;



Procedure SetUpVirtual;
   { This sets up the memory needed for the virtual screen }
BEGIN
  GetMem (VirScr,64000);
  vaddr := seg (virscr^);
  GetMem (VirScr2,64000);
  vaddr2 := seg (virscr2^);
  GetMem (VirScr3,16800);
  vaddr3:=seg(virscr3^);
  GetMem (ShapeTable1,16000);
  shapeaddr:= seg (ShapeTable1^);
  shapeofs:=ofs (ShapeTable1^);
  backofs:=ofs(background);
  backseg:=seg(background);
END;

function showhiscore(showchart,savelist:boolean):integer;
var hilist:text;
    testr:string[10];
    place,tempi:integer;
    names:array[0..9] of string[10];
    scores:array[0..9] of longint;
label bob;
begin
   assign(hilist,'HISCORE.TB1');
   reset(hilist);
     for i:=0 to 9 do readln(hilist,names[i]);
     for i:=0 to 9 do readln(hilist,scores[i]);
   close(hilist);
   lowscore:=scores[9];
   showhiscore:=scores[0];
   if showchart then begin
      fillblock(45,40,285,160,7,vga);
      coolbox(45,40,285,160,false,vga);
     for i:=0 to 9 do
         outtextxy(names[i],51,46+(i*10),9,7,vga,true);
     for i:=0 to 9 do begin
         str(scores[i],testr);
         outtextxy(testr,181,46+(i*10),9,7,vga,true);
     end;
     repeat until keypressed;
     ch:=readkey;
  end;
  if savelist then begin
    place:=0;
     for tempi:=0 to 9 do if score>scores[tempi] then inc(place);
     place:=10-place;
     for tempi:=9 downto place do scores[tempi]:=scores[tempi-1];
     for tempi:=9 downto place do names[tempi]:=names[tempi-1];
     scores[place]:=score;
     names[place]:=hiname;
     assign(hilist,'HISCORE.TB1');
     rewrite(hilist);
       for i:=0 to 9 do writeln(hilist,names[i]);
       for i:=0 to 9 do writeln(hilist,scores[i]);
     close(hilist);
  end;
end;

procedure putshape240over(shapeoffset,towhere,xacross,yacross,xx,yy:word);
label bob,bob2,bob3;
begin
  asm
              {saves registers}
     push ds
     mov cx,[yy]     {multiplies y value by 320}
      shl cx,4
      mov ax,cx
      shl cx,1
      add ax,cx
      shl cx,1
      add ax,cx
      shl cx,1
      add ax,cx
      add ax,[xx]        {adds the x-value to get offset}
      mov di,ax

    
    mov bx,[yacross]
    xor dx,dx
      mov ax,[shapeaddr]     {moves segment of array}
      mov ds,ax
      mov ax,[shapeoffset]     {moves offsett if array}
      {add ax,[shapeofs]}
      mov si,ax
      mov ax,towhere        {loads place to move it}

      mov es,ax
      xor cx,cx
      xor dx,dx
      mov ax,240
      sub ax,[xacross]   {xacross?}
    bob:
      mov cx,[xacross]        {how long to loop across (the x-size of array)}
      rep movsb
      cmp dx,bx
      jz bob3
      xor cx,cx
      add di,ax
      inc dx
      jmp bob
  bob3:
                      {restore variables}
    pop ds
  end;
end;

procedure putshape240(shapeoffset,towhere,xacross,yacross,xx,yy:word);
label bob,bob4,bob2,bob3;
begin
  asm
              {saves registers}
     push ds
     mov cx,[yy]     {multiplies y value by 320}
      shl cx,4
      mov ax,cx
      shl cx,1
      add ax,cx
      shl cx,1
      add ax,cx
      shl cx,1
      add ax,cx
      add ax,[xx]        {adds the x-value to get offset}
      mov di,ax

      mov bx,240
      sub bx,[xacross]   {xacross?}
      push bx
    mov bx,[yacross]
    xor dx,dx
      mov ax,[shapeaddr]     {moves segment of array}
      mov ds,ax
      mov ax,[shapeoffset]     {moves offsett if array}
      {add ax,[shapeofs]}
      mov si,ax
      mov ax,towhere        {loads place to move it}

      mov es,ax
      xor ax,ax
      xor cx,cx
      xor dx,dx
    bob:
      mov cx,[xacross]        {how long to loop across (the x-size of array)}
    bob4:
      xor ah,ah
      lodsb
      cmp ax,0               {If zero skip, if not, store value}
      jz bob2
      stosb
      dec di
    bob2:                    {Adds when x-value must be increased}
      inc di
      loop bob4
      cmp dx,bx
      jz bob3
      xor cx,cx
      pop ax
      add di,ax
      push ax
      inc dx
      jmp bob
  bob3:
    pop bx                  {restore variables}
    pop ds
  end;
end;



procedure put10shape240(shapeoffset,towhere,xx,yy:word);
label bob,bob4,bob2,bob3;
begin
  asm
              {saves registers}
     push ds
     mov cx,[yy]     {multiplies y value by 320}
      shl cx,4
      mov ax,cx
      shl cx,1
      add ax,cx
      shl cx,1
      add ax,cx
      shl cx,1
      add ax,cx
      add ax,[xx]        {adds the x-value to get offset}
      mov di,ax

      mov bx,240
      sub bx,10  {xacross?}
      push bx
    mov bx,9
    xor dx,dx
      mov ax,[shapeaddr]     {moves segment of array}
      mov ds,ax
      mov ax,[shapeoffset]     {moves offsett if array}
      {add ax,[shapeofs]}
      mov si,ax
      mov ax,towhere        {loads place to move it}

      mov es,ax
      xor ax,ax
      xor cx,cx
      xor dx,dx
    bob:
      mov cx,10        {how long to loop across (the x-size of array)}
    bob4:
      xor ah,ah
      lodsb
      cmp ax,0               {If zero skip, if not, store value}
      jz bob2
      stosb
      dec di
    bob2:                    {Adds when x-value must be increased}
      inc di
      loop bob4
      cmp dx,bx
      jz bob3
      xor cx,cx
      pop ax
      add di,ax
      push ax
      inc dx
      jmp bob
  bob3:
    pop bx                  {restore variables}
    pop ds
  end;
end;



procedure putshape(shapeoffset,towhere,xacross,yacross,xx,yy:word);
label bob,bob4,bob2,bob3;
begin
  asm
              {saves registers}
     push ds
     mov ax,[yy]     {multiplies y value by 320}
      shl ax,5
      mov cx,ax
      shl ax,1
      shl cx,3
      add ax,cx
      add ax,[xx]        {adds the x-value to get offset}
      mov di,ax
      mov bx,320
      sub bx,[xacross]   {xacross?}
      push bx
    mov bx,[yacross]
    xor dx,dx
      mov ax,[shapeaddr]     {moves segment of array}
      mov ds,ax
      mov ax,[shapeoffset]     {moves offsett if array}
      {add ax,[shapeofs]}
      mov si,ax
      mov ax,[towhere]        {loads place to move it}
      mov es,ax
      xor ax,ax
      xor cx,cx
      xor dx,dx
    bob:
      mov cx,[xacross]        {how long to loop across (the x-size of array)}
    bob4:
      xor ah,ah
      lodsb
      cmp ax,0               {If zero skip, if not, store value}
      jz bob2
      stosb
      dec di
    bob2:                    {Adds when x-value must be increased}
      inc di
      loop bob4
      cmp dx,bx
      jz bob3
      xor cx,cx
      pop ax
      add di,ax
      push ax
      inc dx
      jmp bob
  bob3:
    pop bx                  {restore variables}
    pop ds
  end;
end;

procedure putshapeline(fromwhereo,towhere,xacross,yacross,xx,yy,line:word);
var int,color:integer;
{draws just one line of a shape with same syntax as putshape}
begin
  for int:=0 to xacross do begin
      {does not work. stack overflow}
      (*asm
        push ax
        push bx
        push cx
        mov ax,[line]     {multiplies y value by 320}
        mul [xacross]
        add ax,[int]
        mov bx,ax
        mov ax,[ShapeAddr]     {moves segment of array}
        mov ds,ax
        {mov ax,[ShapeOfs]}     {moves offset of array}
        mov ax,[fromwhereo]
        add ax,bx
        mov si,ax
        xor ax,ax
        lodsb
        mov [color],ax
        pop cx
        pop bx
        pop ax
      end;*)
      color:=ShapeTable1^[fromwhereo+((xacross*line)+int)];
      if color<>0 then putpixel(xx+int,yy+line,color,towhere);
  end;
end;

procedure putshapeover(fromwhereo,towhere,xacross,yacross,xx,yy:word);
label bbob,bbob4,bbob2,bbob3;
begin
  asm
    push ax          {saves registers}
    push dx
    push cx
    push bx
    push ds
     mov ax,[yy]     {multiplies y value by 320}
      shl ax,5
      mov cx,ax
      shl ax,1
      shl cx,3
      add ax,cx
      add ax,[xx]        {adds the x-value to get offset}
      mov di,ax
      mov bx,320
      sub bx,[xacross]   {xacross?}
      push bx
    mov bx,[yacross]
    xor dx,dx
      mov ax,[shapeaddr]     {moves segment of array}
      mov ds,ax
      (*add ax,[shapeofs] *)    {moves offsett if array}
      mov ax,[fromwhereo]
      mov si,ax
      mov ax,[towhere]        {loads place to move it}
      mov es,ax
      xor ax,ax
      xor cx,cx
      xor dx,dx
    bbob:
      mov cx,[xacross]        {how long to loop across (the x-size of array)}
    bbob4:
      rep movsb               {copies, overwriting}
    bbob2:                    {Adds when x-value must be increased}
      cmp dx,bx
      jz bbob3
      xor cx,cx
      pop ax
      add di,ax
      push ax
      inc dx
      jmp bbob
  bbob3:
    pop bx                  {restore variables}
    pop ds
    pop bx
    pop cx
    pop dx
    pop ax
  end;
end;


procedure flip240(hm:integer);
label gus;
  { This copies 240 columns from vaddr2 to vaddr }
begin
  asm
    push    ds
    mov     ax, vaddr
    mov     es, ax
    mov     ax, vaddr2

    mov     bx,[hm]
    mov     cx,bx
    shl     bx,4
    sub     bx,cx
    add     ax,bx

    mov     ds, ax
    xor     si,si
    xor     di, di
  mov ax,200
  gus:
    mov     cx, 120
    rep     movsw
    add     di,80

  dec ax
  jnz gus

    pop     ds
  end;
end;


procedure flip50(fromwhere,off1,whereto,off2:word);
label gus;
  { This copies 240 columns from vaddr2 to vaddr }
begin
  asm
    push    ds
    mov     ax, [whereto]
    add     ax, [off2]
    mov     es, ax
    mov     ax, [fromwhere]
    add     ax,[off1]
    mov     ds, ax
    xor     si,si
    xor     di, di
  mov ax,50
  gus:
    mov     cx, 120
    rep     movsw
  dec ax
  jnz gus

    pop     ds
  end;
end;


procedure help;
begin
  cls(0,vga);
  coolbox(0,0,319,199,false,vga);
  outtextxy('HELP',144,10,4,0,vga,false);
  outtextxy('--------------------------------------',10,20,12,0,vga,false);
  outtextxy('IN THE GAME:',10,30,4,0,vga,false);
  outtextxy('   ARROWS MANUEVER',10,40,4,0,vga,false);
  outtextxy('   SPACE BAR FIRES MISSILES',10,50,4,0,vga,false);
  outtextxy('   F2 SAVES GAME',10,60,4,0,vga,false);
  outtextxy('   P=PAUSE  S=TOGGLE SOUND  ESC=QUIT',10,70,4,0,vga,false);
  outtextxy('--------------------------------------',10,80,12,0,vga,false);
  outtextxy('MISSION: SHOOT THE INANIMATE OBJECTS',10,90,4,0,vga,false);
  outtextxy('         WHY?  WATCH THE STORY!',10,100,4,0,vga,false);
  outtextxy('--------------------------------------',10,110,12,0,vga,false);
  outtextxy('THE SPACE BAR SPEEDS UP MOVIE SCENES',10,120,4,0,vga,false);
  outtextxy('ESC QUITS THEM',10,130,4,0,vga,false);
  outtextxy('--------------------------------------',10,140,12,0,vga,false);
  outtextxy('   SEE "TB1.DOC" FOR MORE HELP/INFO',10,150,4,0,vga,false);
  outtextxy('--------------------------------------',10,160,12,0,vga,false);
  outtextxy('PRESS ANY KEY TO CONTINUE',64,185,4,0,vga,false);
  repeat until keypressed;
  ch:=readkey;
end;




Procedure setupsidebar;
 
VAR i,x,y:integer;
    loop1,loop2,loop3:integer;
    tempaddr:word;

procedure makehole(y:integer);
begin
  fillblock(249,y,314,y+9,0,tempaddr);
  horizontalline(249,314,y,24,tempaddr);
  horizontalline(249,313,y+10,18,tempaddr);
  line(249,y,249,y+9,24,tempaddr);
  line(314,y+1,314,y+10,18,tempaddr);
end;

BEGIN
  tempaddr:=vaddr;
  fillblock(240,0,320,199,19,tempaddr);
   line(240,0,240,199,18,tempaddr);
   line(240,0,319,0,18,tempaddr);
   line(319,0,319,199,24,tempaddr);
   line(241,199,319,199,24,tempaddr);
  outtextxy('SCORE',241,1,127,0,tempaddr,false);
  outtextxy('SCORE',242,2,143,0,tempaddr,false);
    makehole(10);
  str(score,it);
  outtextxy(it,250+(8*(8-length(it))),11,12,0,tempaddr,true);

  outtextxy('HI-SCORE',241,21,127,0,tempaddr,false);
  outtextxy('HI-SCORE',242,22,143,0,tempaddr,false);
  str(hiscore,it);
  makehole(30);
  outtextxy(it,250+(8*(8-length(it))),31,12,0,tempaddr,true);
  
  outtextxy('LEVEL',241,41,127,0,tempaddr,false);
  outtextxy('LEVEL',242,42,143,0,tempaddr,false);
  makehole(50);
  outtextxy('SHIELDS',241,61,127,0,tempaddr,false);
  outtextxy('SHIELDS',242,62,143,0,tempaddr,false);
  makehole(70);
  for i:=0 to ((4*shields)-1) do line(250+i,71,250+i,78,(47-(i div 4)),tempaddr);

  outtextxy('WEAPONS',241,81,127,0,tempaddr,false);
  outtextxy('WEAPONS',242,82,143,0,tempaddr,false);
  makehole(90);

  fillblock(249,111,314,189,0,tempaddr);

  line(249,111,249,189,24,tempaddr);
  line(315,111,315,189,18,tempaddr);
  line(249,111,315,111,24,tempaddr);
  line(249,189,315,189,18,tempaddr);

  outtextxy('  TB1   ',251,114,15,0,tempaddr,false);
  outtextxy('F1-HELP  ',251,124,15,0,tempaddr,false);
  outtextxy('ESC-QUIT',251,135,15,0,tempaddr,false);
  outtextxy('F2-SAVE ',251,145,15,0,tempaddr,false);

  flip (vaddr,vaddr2);
END;

procedure ClearKeyboardBuffer;
begin
  mem[$0040:$001c]:=mem[$0040:$001a];
end;



Procedure blockmove(x1,y1,x2,y2,p1,x3,y3,p2:word);
  { This is a small procedure to copy block x1,y1,x2,y2,
    from page 1 to x3,y3 on page 2}
BEGIN
  asm
    push    ds
    push    es
    mov     ax,p1
    mov     ds,ax
    mov     ax,p2
    mov     es,ax

    mov     bx,[X1]
    mov     dx,[Y1]
    push    bx                      {; and this again for later}
    mov     bx, dx                  {; bx = dx}
    mov     dh, dl                  {; dx = dx * 256}
    xor     dl, dl
    shl     bx, 6                   {; bx = bx * 64}
    add     dx, bx                  {; dx = dx + bx (ie y*320)}
    pop     bx                      {; get back our x}
    add     bx, dx                  {; finalise location}
    mov     si, bx                  {; es:di = where to go}

    mov     bx,[X3]
    mov     dx,[Y3]
    push    bx                      {; and this again for later}
    mov     bx, dx                  {; bx = dx}
    mov     dh, dl                  {; dx = dx * 256}
    xor     dl, dl
    shl     bx, 6                   {; bx = bx * 64}
    add     dx, bx                  {; dx = dx + bx (ie y*320)}
    pop     bx                      {; get back our x}
    add     bx, dx                  {; finalise location}
    mov     di, bx                  {; es:di = where to go}

    mov ax,[y2]
    sub  ax,[y1]

    mov     dx,[x2]
    sub     dx,[x1]
    mov     bx,320
    sub     bx,dx

    
@@1 :
    mov     cx, dx         { Width of block to copy divided by 2 }
    shr     cx,1
    jnc     @@2
    rep     movsw
    movsb
    jmp @@3
@@2:
    rep     movsw
@@3:
    add     di,bx
    add     si,bx
    dec     ax
    jnz     @@1

    pop     es
    pop     ds
  end;
END;


procedure pauseawhile(howlong:word);
var i:word;
begin
  i:=0;
  repeat
    waitretrace;
    inc(i);
    if i>howlong then exit;
  until keypressed;
end;



procedure changescore;
var itemp,jtemp:integer;
begin
   str(score:8,scorest);
   outtextxy(scorest,250,11,12,0,vaddr,true);
   if (score mod 400 =0) and (shields<16) then begin
      {if sbeffects then startsound(sound[2],0,false);}
      inc(shields);
      pal(254,shields*4,0,0);
      for itemp:=0 to shields do
          for jtemp:=71 to 78 do
              horizontalline(250+(itemp*4),
                             254+(itemp*4),jtemp,
                             47-itemp,vaddr);

  end;
end;

function collision(x1,y1,xsize1,ysize1,x2,y2,xsize2,ysize2:integer):boolean;
var bob:byte;
    hit:boolean;
label aat1,aat2,aat3,aat4;
begin
  bob:=0;
    asm
      mov ax,[x1]
      mov bx,[x2]
      mov cx,[xsize1]
      mov dx,[xsize2]
      cmp bx,ax
    jnge aat1
      xchg ax,bx
      xchg cx,dx
    aat1:
      add ax,cx
      add bx,dx

      sub ax,bx
      add cx,dx
      cmp cx,ax
    jnge aat3
      inc [bob]

      mov ax,[y1]
      mov bx,[y2]
      mov cx,[ysize1]
      mov dx,[ysize2]
      cmp bx,ax
    jnge aat2
      xchg ax,bx
      xchg cx,dx
    aat2:
      add ax,cx
      add bx,dx

      sub ax,bx
      add cx,dx
      cmp cx,ax
    jnge aat3
      inc [bob]
    aat3:
      mov [hit],0
      cmp [bob],2
    jnz aat4
      mov [hit],1
    aat4:
  end;
  if hit then collision:=true
         else collision:=false;
end;







{%%}
procedure clearshape;
begin
  asm
    push es
    mov cx,8000
    mov es,[shapeaddr]
    xor di,di
    mov al,5
    mov ah,al
    rep stosw
    pop es
  end;
end;



procedure outbackline(line,row:word);
label bob,bob2,bob3;
var off2,off:word;
begin
  off2:=vaddr3;  {not sure why necessary, but keep}
  off:=backseg;  {losing the values of the original}
 asm
              {saves registers}
     push ds
     push es
     xor ax,ax {old xx variable (times to repeat)=0}
     push ax

     {mov cx,[(4-row)*10)] }    {multiplies y value by 2400}

   {calculates initial offset for y value}

     mov ax,4
     sub ax,row
     shl ax,5
     mov bx,ax
     shl ax,1
     add bx,ax
     shl ax,2
     add bx,ax
     shl ax,3
     add bx,ax  {original row offset is in bx}


bob2:                  {get background array}
      mov ax,[off]
      mov ds,ax
      mov dx,[line]
      shl dx, 2
      mov ax,dx
      shl dx,1
      add ax,dx
      pop cx
      add ax,cx
      push cx
      add ax,[backofs]
      mov si,ax
      lodsb
      xor ah,ah
      mov cx,ax

      
      shl cx,3       {from background number, get actual offset}
      mov ax,cx
      shl cx,3
      add ax,cx
      shl cx,1
      add ax,cx
      add ax,8000
      mov si,ax

      mov ax,[shapeaddr]
      mov ds,ax

      


      mov ax,[off2]        {loads place to move it}
      mov es,ax
      mov di,bx
      mov ax,220   {what to add to increase one y}
      xor dx,dx
    bob:
      mov cx,20        {how long to loop across (the x-size of array)}
      rep movsb        {move, overwriting}
      cmp dx,9         {see if longer than y size}
      jz bob3          {if so, end}
      add di,ax        {if not, increase y by one}
      inc dx
      jmp bob
  bob3:
    add bx,20          {Increase starting offset value}

    pop cx             {jump if not drawn 12 across}
    inc cx
    cmp cx,12
    push cx
    jnz bob2
                  {restore variables}
    pop cx
    pop es
    pop ds
  end;
end;

procedure savegame;
begin
   cls(0,vga);
   coolbox(0,0,319,199,false,vga);
   outtextxy('SAVE GAME',124,10,4,0,vga,false);
   outtextxy('THIS WILL ONLY SAVE HOW YOU WERE',10,20,4,0,vga,false);
   outtextxy('AT THE BEGINNING OF THE LEVEL!!!',10,30,4,0,vga,false);
   findfirst('SG?.TB1',ANYFILE,filestuff);
   ondisk:='ALREADY SAVED GAMES:';
   while doserror=0 do with filestuff do
         begin
           ondisk:=concat(ondisk,name[3]);
           findnext(filestuff);
         end;
         if ondisk='' then ondisk:=concat(ondisk,'NONE');
         outtextxy(ondisk,10,50,4,0,vga,false);
         outtextxy('SAVE WHICH GAME?',10,60,4,0,vga,false);
         repeat until keypressed;
         tempch:=readkey;
         if (tempch>='0') and (tempch<='9') then begin
         {$I-}
           ondisk:=concat('SG',tempch,'.TB1');
           assign(savegamef,ondisk);
           rewrite(savegamef);
         {$I+}
           If IOResult=0 then begin
              writeln(savegamef,beginscore);
              writeln(savegamef,level);
              writeln(savegamef,beginshields);
              close(savegamef);
              outtextxy('GAME SAVED',120,150,15,0,vga,false);
           end
           else outtextxy('ERROR! GAME NOT SAVED!',102,150,15,0,vga,false);
           outtextxy('PRESS ANY KEY...',96,180,4,0,vga,false);
           repeat; until keypressed; tempch:=readkey;
         end;
end;


function upcollide(x,y,speed:word;add:integer;howfar,where:word):word;
var add2,color:word;
label mree1,mree2,mree3,mree4,mree5,mree6;
begin
  add2:=y+add;
  asm
              {saves registers}
     push ds
     mov cx,[add2]     {multiplies y value by 320}
      shl cx,4
      mov ax,cx
      shl cx,1
      add ax,cx
      shl cx,1
      add ax,cx
      shl cx,1
      add ax,cx
      add ax,[x]        {adds the x-value to get offset}
      mov si,ax
      mov ax,[where]
      mov ds,ax
      xor ah,ah

      mov [color],0
      mov dx,[speed]
      inc dx
  mree2:
      dec dx

      mov cx,[howfar]
  mree3:
      lodsb
      cmp ax,3
      je  mree1
      cmp ax,5
      je  mree4
      jne  mree6
      mree4:
        cmp bx,3
        je mree6
  mree1:
      mov [color],ax
      mov bx,ax
  mree6:
    loop mree3
    add si,240
    sub si,[howfar]
    cmp dx,0
    jne mree2
    pop ds
  end;
  upcollide:=color;
end;

function leftcollide(x,y,speed:word;add:integer;howfar,where:word):word;
var add2,color:word;
label mree1,mree2,mree3,mree4,mree5,mree6;
begin
  add2:=x+add;
  asm
              {saves registers}
     push ds
     mov cx,[y]     {multiplies y value by 320}
      shl cx,4
      mov ax,cx
      shl cx,1
      add ax,cx
      shl cx,1
      add ax,cx
      shl cx,1
      add ax,cx
      add ax,[add2]        {adds the x-value to get offset}
      mov si,ax
      mov ax,[where]
      mov ds,ax
      xor ah,ah

      mov [color],0
      mov dx,[howfar]
      inc dx

  mree2:
      dec dx

      mov cx,[speed]
      inc cx
  mree3:
      lodsb
      cmp ax,3
      je  mree1
      cmp ax,5
      jne  mree6
        cmp bx,3
        je mree6
  mree1:
      mov [color],ax
      mov bx,ax
  mree6:
    loop mree3
    add si,240
    sub si,[speed]
    cmp dx,0
    jne mree2
    pop ds
  end;
  leftcollide:=color;
end;

procedure loadlevel3shapes;
var x,y,i,j,shape:byte;
begin
   clearshape;
   grapherror:=loadpicsuperpacked(0,0,vaddr,false,true,'tblev3.tb1');
   
  for j:=0 to 3 do
    for i:=0 to 19 do
      for x:=0 to 9 do
          for y:=0 to 9 do
              ShapeTable1^[shape3array[(j*20)+i]+((y*10)+x)]
                          :=getpixel(1+x+(i*11),1+y+(j*11),vaddr);
  cls(0,vaddr);
end;


Procedure levelthree;
label newroom;
type{
EnemyInfo = record
               x,y:integer;
               kind:byte;
               out,exploding,boundarycheck,dead:boolean;
               explodprogress:byte;
               minx,maxx,xspeed,yspeed:integer;
               hitsneeded:integer;
             end;}
pittype = record
              px,py:integer;
           end;
kctype = record
              kcx,kcy:integer;
              kcout:boolean;
           end;

const framedir:array[0..1,0..3] of integer =((0,1,0,-1),(1,0,-1,0));
VAR loop1,loop2:integer;
    ch,ch2:char;
    pits:array[0..4] of pittype;
    numpits:byte;

    bullet1x,bullet1y,bullet2x,bullet2y,i:integer;
    bullet1dir,bullet2dir,collide:byte;
    bullet1out,bullet2out:boolean;
    whichone,temp,temp2:integer;
    tempst:string;
    what:byte;
    k:integer;
    itemp,jtemp:byte;
    whatdelay:byte;
    tempilg:byte;
    xx:integer;
    ucollide,dcollide,lcollide,rcollide:word;
    walking,feet:byte;
    room:byte;
   backrow,topblocky:integer;
   levelover,odd,changeroom,havegun:boolean;
   whichroomnext:array[0..3] of byte; {n,s,e,w}
   keycards:array[0..3] of boolean;
   keycard:kctype;
   shieldup:array[0..11] of boolean;
   shieldx,shieldy,laserx,lasery:integer;
   laserup:array[0..11] of boolean;
   computers:array[0..11] of boolean;
   computerx,computery:integer;
   computer_0hits:integer;
   tempkc:byte;

procedure horizwalls(x,y,howfar:word);
var i:word;
begin
  for i:=0 to howfar do
      put10shape240(shape3array[3],vaddr2,x+(i*10),y);
end;

procedure verticalwalls(x,y,howfar:word);
var i:word;
begin
  for i:=0 to howfar do
      put10shape240(shape3array[2],vaddr2,x,y+(i*10));
end;

procedure dopits;
var i:word;
begin
  for i:=0 to (numpits-1) do
      with pits[i] do
           put10shape240(shape3array[5],vaddr2,px,py);
end;

procedure clearroom;
var i,j:byte;
begin
  for i:=0 to 23 do
    for j:=0 to 19 do
         put10shape240(shape3array[0],
                    vaddr2,i*10,j*10);
end;


procedure doroom(n,s,e,w:boolean);
var i:byte;
begin
    for i:=0 to 2 do begin
        put10shape240(shape3array[18],vaddr2,100+(i*10),0);
        put10shape240(shape3array[18],vaddr2,100+(i*10),199);
        put10shape240(shape3array[19],vaddr2,0,90+(i*10));
        put10shape240(shape3array[19],vaddr2,239,90+(i*10));
    end;

    if n then begin
         horizwalls(0,0,10);
         horizwalls(130,0,10);
    end
    else horizwalls(0,0,23);
    if s then begin
         horizwalls(0,197,10);
         horizwalls(130,197,10);
    end
    else horizwalls(0,197,23);

    if e then begin
       verticalwalls(237,0,8);
       verticalwalls(237,110,8);
    end

    else for i:=0 to 20 do put10shape240(shape3array[2],vaddr2,237,i*10);
    if w then begin
       verticalwalls(0,0,8);
       verticalwalls(0,110,8);
    end
    else for i:=0 to 20 do put10shape240(shape3array[2],vaddr2,0,i*10);

end;



BEGIN
  {loadlevel3shapes;}
  computer_0hits:=0;
  whatdelay:=1;
  havegun:=true;
  for i:=0 to 3 do keycards[i]:=false;
  for i:=0 to 11 do begin
      shieldup[i]:=false;
      laserup[i]:=false;
      computers[i]:=false;
  end;
  shieldup[0]:=true;
  laserup[0]:=true;
  computers[0]:=true;

  shipx:=115;
  shipy:=180;
  odd:=false;
  shipxadd:=0;
  room:=0;
  shipyadd:=0;

  shipframe:=1;
  shipspeed:=5;

  ch:=#1; ch2:=#1;
  bullet1out:=false;  bullet2out:=false;
  bullet1x:=0; bullet1y:=0;
  bullet2x:=0; bullet2y:=0;

  flip(vaddr2,vaddr);
  str(level,tempst);
  fillblock(251,52,314,59,0,vaddr);
  outtextxy(tempst,307,51,12,0,vaddr,false);

  cls(0,vaddr2);

  pal(250,0,0,63);
  pal(251,63,0,0);

  pal(254,shields*4,0,0);
  howmuchscroll:=50;

  cls(0,vga);
  coolbox(70,85,240,120,true,vga);
  outtextxy('   LEVEL THREE:',84,95,4,7,vga,false);
  outtextxy('  THE ALIEN SHIP',84,105,4,7,vga,false);

  clearkeyboardbuffer;
  pauseawhile(300);
  numpassive:=0;
  levelover:=false;
  
newroom:
   numpits:=0;
   changeroom:=false;
   if room=254 then room:=0;
   clearroom;
   if shipframe=1 then shipy:=188
   else if shipframe=3 then shipy:=3
   else if shipframe=2 then shipx:=3
   else if shipframe=4 then shipx:=228;
   keycard.kcout:=false;


  if room=0 then begin
     whichroomnext[0]:=1;
     whichroomnext[1]:=255;
     clearroom;
     verticalwalls(30,0,16);
     verticalwalls(207,0,16);
     verticalwalls(57,30,16);
     verticalwalls(180,30,16);
     with pits[0] do begin
          px:=95;
          py:=80;
     end;
     with pits[1] do begin
          px:=135;
          py:=80;
     end;
     numpits:=2;  { actual}
     dopits;
     
  for i:=0 to 10 do
      for j:=0 to 2 do begin
          put10shape240(shape3array[4],vaddr2,60+(j*10),80+(i*10));
          put10shape240(shape3array[4],vaddr2,150+(j*10),80+(i*10));
      end;
{walkway}
  for i:=0 to 15 do for j:=0 to 1 do
      put10shape240(shape3array[1],vaddr2,110+(j*10),40+(i*10));

      put10shape240(shape3array[6],vaddr2,110,30);
      put10shape240(shape3array[6],vaddr2,120,30);
      computerx:=110;
      computery:=30;

      if shieldup[0] then begin
        shieldx:=10;
        shieldy:=10;
        put10shape240(shape3array[8],vaddr2,10,10);
      end;

      if laserup[0] then begin
         laserx:=220;
         lasery:=10;
         put10shape240(shape3array[7],vaddr2,220,10);
      end;

      {badguys}
      {put10shape240(shape3array[32],vaddr2,40,180);
      put10shape240(shape3array[32],vaddr2,190,180);}


     doroom(true,true,false,false);

  
  end;
  if room=1 then begin
     whichroomnext[0]:=2;
     whichroomnext[1]:=0;
     doroom(true,true,false,false);
  end;
  if room=2 then begin
     whichroomnext[1]:=1;
     whichroomnext[2]:=6;
     whichroomnext[3]:=4;
     with pits[0] do begin
          px:=95;
          py:=80;
     end;
     with pits[1] do begin
          px:=135;
          py:=80;
     end;
     numpits:=2;
     dopits;

     doroom(false,true,true,true);
  end;
  if room=3 then begin
     whichroomnext[0]:=4;
     if not(keycards[3]) then
        begin
          keycard.kcout:=true;
          keycard.kcx:=100;
          keycard.kcy:=100;
          put10shape240(shape3array[12],vaddr2,100,100);
        end;
     doroom(true,false,false,false);
  end;
  if room=4 then begin
     whichroomnext[0]:=7;
     whichroomnext[1]:=3;
     whichroomnext[2]:=2;
     doroom(true,true,true,false);
  end;
  if room=5 then begin
     whichroomnext[0]:=6;
     doroom(true,false,false,false);
     if not(keycards[2]) then begin
        keycard.kcout:=true;
        keycard.kcx:=100;
        keycard.kcy:=100;
        put10shape240(shape3array[11],vaddr2,100,100);
     end;
  end;
  if room=6 then begin
     whichroomnext[0]:=11;
     whichroomnext[1]:=5;
     whichroomnext[3]:=2;
     doroom(true,true,false,true);
  end;
  if room=7 then begin
     whichroomnext[0]:=8;
     whichroomnext[1]:=4;
     doroom(true,true,false,false);
  end;
  if room=8 then begin
     whichroomnext[1]:=7;
     whichroomnext[2]:=9;
     if not(keycards[0]) then begin
        keycard.kcout:=true;
        keycard.kcx:=100;
        keycard.kcy:=100;
        put10shape240(shape3array[9],vaddr2,100,100);
     end;
     doroom(false,true,true,false);
  end;
  if room=9 then begin
     whichroomnext[2]:=10;
     whichroomnext[3]:=8;
     doroom(false,false,true,true);
  end;
  if room=10 then begin
     whichroomnext[1]:=11;
     whichroomnext[3]:=9;
     if not(keycards[1]) then begin
        keycard.kcout:=true;
        keycard.kcx:=100;
        keycard.kcy:=100;
        put10shape240(shape3array[10],vaddr2,100,100);
     end;
     doroom(false,true,false,true);
  end;
  if room=11 then begin
     whichroomnext[0]:=10;
     whichroomnext[1]:=6;
     doroom(true,true,false,false);
  end;
  if room=255 then begin
     if keycards[0] and keycards[1] and
        keycards[2] and keycards[3] then
        begin
          levelover:=true;
          cls(0,vga);
          outtextxy('You Have won!',5,5,9,7,vga,false);
          outtextxy('But as you can see this level is not done yet.',5,15,9,7,vga,false);
          {outtextxy('Anyway');}
          readln;
          inc(level);
        end
     else begin
          cls(0,vga);
          outtextxy('You Cannot Leave Yet',5,5,9,7,vga,true);
          repeat until keypressed; tempch:=readkey;
          levelover:=false;
          shipframe:=1;
          room:=254;
     end;
  end;
  if (room=254) and not(levelover) then goto newroom;





{**** GAME LOOP ****}
{*******************}
  Repeat
    ch2:=#1;
    ch:=#1;
    
    flip240(0);

{***Collision Check***}
   if numpits>0 then
    for i:=0 to (numpits-1) do with pits[i] do
        if collision(shipx,shipy,5,4,px,py,4,4)
            then begin
                 levelover:=true;
                 cls(0,vga);
                 grapherror:=loadpicsuperpacked(0,0,vga,false,true,'tbpit.tb1');


                 {outtextxy('You Fell In A Pit!',5,5,9,2,vga,false);}
                 clearkeyboardbuffer;
                 repeat until keypressed; tempch:=readkey;
            end;
    if keycard.kcout then
       if collision(shipx,shipy,5,4,keycard.kcx,keycard.kcy,5,5)
          then begin
               keycard.kcout:=false;
               case room of
                    3: keycards[3]:=true;
                    5: keycards[2]:=true;
                    8: keycards[0]:=true;
                    10:keycards[1]:=true;
               end;
               put10shape240(shape3array[0],vaddr2,keycard.kcx,keycard.kcy);
          end;
    if shieldup[room] then
       if collision(shipx,shipy,5,4,shieldx,shieldy,5,5)
          then begin
               shieldup[room]:=false;
               put10shape240(shape3array[0],vaddr2,shieldx,shieldy);
               inc(shields);
               pal(254,shields*4,0,0);
               for itemp:=0 to shields do
                   for jtemp:=71 to 78 do
                       horizontalline(250+(itemp*4),
                             254+(itemp*4),jtemp,
                             47-itemp,vaddr);
          end;
    if laserup[room] then
       if collision(shipx,shipy,5,4,laserx,lasery,5,5)
          then begin
               laserup[room]:=false;
               put10shape240(shape3array[0],vaddr2,laserx,lasery);
          end;
    if computers[room] then
       if collision(shipx,shipy,5,4,computerx,computery,10,5)
          then begin
               cls(0,vga);
               grapherror:=loadpicsuperpacked(0,0,vga,false,true,'tbconsol.tb1');
               tempkc:=0;
               if keycards[0] then begin
                  inc(tempkc);
                  putpixel(147,132,0,vga);
               end;
               if keycards[1] then begin
                  inc(tempkc);
                  putpixel(179,132,0,vga);
               end;
               if keycards[2] then begin
                  inc(tempkc);
                  putpixel(179,160,0,vga);
               end;
               if keycards[3] then begin
                  inc(tempkc);
                  putpixel(147,160,0,vga);
               end;

               case room of
                 0: begin
                    inc(computer_0hits);
                    if computer_0hits=1 then begin

               outtextxy('COMPUTER ACTIVATED:',47,30,2,0,vga,true);
               outsmalltextxy(' HUMAN YOU HAVE COME TOO SOON.  LEVELS 3',47,40,2,0,vga,true);
               outsmalltextxy('  AND 4 ARE INCOMPLETE.',47,48,2,0,vga,true);
               outsmalltextxy(' ANYWAY I CAN SEE YOU ARE NOT THE',47,58,2,0,vga,true);
               outsmalltextxy('  TENTACLEE COMMANDER.  YOU ARE IN',47,66,2,0,vga,true);
               outsmalltextxy('  GRAVE DANGER.  LUCKILY THE MAIN',47,74,2,0,vga,true);
               outsmalltextxy('  COMPUTER SYSTEM DOES NOT APPROVE',47,82,2,0,vga,true);
               outsmalltextxy('  OF THE TENTACLEE''S POLICIES.',47,90,2,0,vga,true);
               outsmalltextxy(' I PERSONALLY CANNOT SHUT OFF THE TRACTOR',47,100,2,0,vga,true);
               outsmalltextxy('  BEAM.  YOU MUST RETRIEVE FOUR KEYCARDS',47,108,2,0,vga,true);
               outsmalltextxy('  SCATTERED AROUND THE FLIGHT DECK.',47,116,2,0,vga,true);
               outsmalltextxy(' THE MAP BELOW WILL AID YOU.',47,124,2,0,vga,true);
              end;
              if computer_0hits=2 then begin

               outtextxy('COMPUTER ACTIVATED:',47,30,2,0,vga,true);
               outsmalltextxy(' HUMAN I HAVE ALREADY TOLD YOU MUCH.',47,40,2,0,vga,true);
               outsmalltextxy('  COLLECT THE 4 KEYCARDS, MADE OF',47,48,2,0,vga,true);
               outsmalltextxy('  RUBY, GOLD, EMERALD, AND ALUMINUM.',47,56,2,0,vga,true);
               outsmalltextxy(' WATCH OUT FOR ENEMIES NOT UNDER MY',47,66,2,0,vga,true);
               outsmalltextxy('  CONTROL, RADIOACTIVE FLOORS, AND',47,74,2,0,vga,true);
               outsmalltextxy('  EXTREMELY DEEP PITS.',47,82,2,0,vga,true);
              end;
          if computer_0hits>2 then begin
                outtextxy('COMPUTER ACTIVATED:',47,30,2,0,vga,true);
               outsmalltextxy(' HUMAN, GO AWAY.  YOU ANNOY ME.',47,40,2,0,vga,true);
               outsmalltextxy('  I HAVE TOLD YOU EVERYTHING.',47,48,2,0,vga,true);
          end;



                  end;
               end;
               clearkeyboardbuffer;
               repeat until keypressed; tempch:=readkey;
               shipx:=shipx+(4*framedir[1,shipframe]);
               shipy:=shipy+(4*framedir[0,shipframe]);
          end;


{***DO EXPLOSIONS***}
{    for i:=0 to 30 do
        if passive[i].exploding then with passive[i] do begin
           inc(explodeprogress);
           putshape240(shape2array[35+explodeprogress],vaddr2,
                          20,9,x,y+howmuchscroll);
           if explodeprogress>4 then begin
              dead:=true;
              exploding:=false;
              putshape240over(14800(*shape2array[34]*),vaddr2,
                          20,9,x,y+howmuchscroll);
           end;
        end;

 }
{***MOVE BULLET***}
    if bullet1out then begin
       case bullet1dir of
            1:begin dec(bullet1y,5);
                    collide:=upcollide(bullet1x,bullet1y,5,-5,3,vaddr2);
              end;
            2:begin inc(bullet1x,5);
                    collide:=leftcollide(bullet1x,bullet1y,5,10,3,vaddr2);
              end;
            3:begin inc(bullet1y,5);
                    collide:=upcollide(bullet1x,bullet1y,5,10,3,vaddr2);
              end;
            4:begin dec(bullet1x,5);
                    collide:=leftcollide(bullet1x,bullet1y,5,-5,3,vaddr2);
              end;
       end;

       if collide<>0 then bullet1out:=false;
       if bullet1out then putshape(shape3array[76],vaddr,10,9,bullet1x,bullet1y);
    end;
    if bullet2out then begin
       dec(bullet2y,5);
       if bullet2y<5 then bullet2out:=false;
       if bullet2out then putshape(shape3array[76],vaddr,10,9,bullet2x,bullet2y);
    end;
{***MOVE ENEMIES***}
  {  for j:=0 to 30 do begin
        if passive[j].dead=false then begin
           inc(passive[j].y);
           if(passive[j].y)>190 then passive[j].dead:=true;
        end;
    end;
    for j:=0 to 30 do begin
        if passive[j].lastshot>0 then dec(passive[j].lastshot);
        if (passive[j].dead=false) and (passive[j].shooting)
           and (passive[j].lastshot=0) and (passive[j].y>0)
           then begin
             tempilg:=7;
             for i:=0 to 5 do if enemy[i].out=false then tempilg:=i;
             if tempilg<>7 then begin
                passive[j].lastshot:=30;
                enemy[tempilg].out:=true;
                enemy[tempilg].y:=passive[j].y;
                enemy[tempilg].x:=passive[j].x+5;
                enemy[tempilg].yspeed:=5;
                enemy[tempilg].kind:=25;
                if passive[j].kind=11 then enemy[tempilg].kind:=26;
             end;
        end;
    end;
    for j:=0 to 5 do begin
        if enemy[j].out then begin
           putshape(shape2array[enemy[j].kind],vaddr,
                          20,9,enemy[j].x,enemy[j].y);
           enemy[j].y:=enemy[j].y+enemy[j].yspeed;
           if enemy[j].y>189 then enemy[j].out:=false;
        end;
    end;
   }

{***READ KEYBOARD***}

    if keypressed then BEGIN
      ch:=readkey;
      if ch=chr(0) then ch2:=readkey;
      if ch=#27 then levelover:=true;
      clearkeyboardbuffer;

      if ch2='M' then begin
         if (shipframe=2) and (shipxadd=0) then shipxadd:=2
         else if (shipframe<>2) then shipframe:=2
         else inc(shipxadd);
      end;
      if ch2='K' then begin
         if (shipframe=4) and (shipxadd=0) then shipxadd:=-2
         else if (shipframe<>4) then shipframe:=4
         else dec(shipxadd);
      end;
      if ch2='H' then begin
         if (shipframe=1) and (shipyadd=0) then shipyadd:=-2
         else if (shipframe<>1) then shipframe:=1
         else dec(shipyadd);

      end;
      if ch2='P' then begin
         if (shipframe=3) and (shipyadd=0) then shipyadd:=2
         else if (shipframe<>3) then shipframe:=3
         else inc(shipyadd);

      end;
      if ch2=';' then help;
      if ch='+' then begin
         inc(whatdelay);
         if whatdelay>25 then whatdelay:=25;
      end;
      if (ch='P') or (ch='p') then begin
         coolbox(65,85,175,110,true,vga);
         outtextxy('GAME PAUSED',79,95,4,7,vga,false);
         clearkeyboardbuffer;
         repeat until keypressed;  tempch:=readkey;
      end;
      if ch='-' then begin
         dec(whatdelay);
         if whatdelay<1 then whatdelay:=1;
      end;
     {if (ch='S') or (ch='s') then sbeffects:=not(sbeffects);}

      if ch2='<' then savegame;

      end;
      if (ch=' ') and havegun then begin
         if (bullet1out=false) then begin
            {if sbeffects then StartSound(Sound[4], 0, false);}
            bullet1out:=true;
            bullet1x:=shipx+3;
            bullet1y:=shipy+4;
            bullet1dir:=shipframe;
            putshape(shape3array[76],vaddr,10,9,bullet1x,bullet1y);
         end
         else
         if (bullet2out=false) then begin
            {if sbeffects then StartSound(Sound[4], 0, false);}
            bullet2out:=true;
            bullet2x:=shipx;
            bullet2y:=shipy;
            bullet2dir:=shipframe;
            putshape(shape3array[76],vaddr,10,9,bullet2x,bullet2y);
       end;
    end;

{***MOVE SHIP***}
        

        ucollide:=upcollide(shipx,shipy,abs(shipyadd),-abs(shipyadd),10,vaddr2);
        dcollide:=upcollide(shipx,shipy,abs(shipyadd),8,10,vaddr2);
        lcollide:=leftcollide(shipx,shipy,abs(shipxadd),-abs(shipxadd),8,vaddr2);
        rcollide:=leftcollide(shipx,shipy,abs(shipxadd),0,8,vaddr2);
        if (shipframe=1) and (ucollide<>0) then shipyadd:=0;
        if (shipframe=3) and (dcollide<>0) then shipyadd:=0;
        if (shipframe=2) and (rcollide<>0) then shipxadd:=0;
        if (shipframe=4) and (lcollide<>0) then shipxadd:=0;
        shipy:=shipy+shipyadd;
        shipyadd:=shipyadd-sgn(shipyadd);
        shipx:=shipx+shipxadd;
        shipxadd:=shipxadd-sgn(shipxadd);

        case ucollide of
           5: begin
                changeroom:=true;
                room:=whichroomnext[0];
              end;
        end;
        case dcollide of
           5: begin
                changeroom:=true;
                room:=whichroomnext[1];
              end;
        end;
        case rcollide of
           5: begin
                changeroom:=true;
                room:=whichroomnext[2];
              end;
        end;
        case lcollide of
           5: begin
                changeroom:=true;
                room:=whichroomnext[3];
              end;
        end;
       if (shipyadd<>0) or (shipxadd<>0) then inc(walking,4)
       else walking:=0;
       if walking>12 then walking:=0;

        CASE shipframe of
           1   : putshape (shape3array[60+walking],vaddr,10,9,shipx,shipy);
           2   : putshape (shape3array[61+walking],vaddr,10,9,shipx,shipy);
           3   : putshape (shape3array[62+walking],vaddr,10,9,shipx,shipy);
           4   : putshape (shape3array[63+walking],vaddr,10,9,shipx,shipy);
        END;
    waitretrace;
    flip (vaddr,vga);
    if odd then begin
       pal(250,0,0,63);
       pal(251,63,0,0);
       odd:=not(odd);
    end
    else begin
       pal(251,0,0,63);
       pal(250,63,0,0);
       odd:=not(odd);
    end;


    for i:=0 to whatdelay do waitretrace;

    if changeroom then goto newroom;

  until levelover;
END;







function grinput(xv,yv,howlong,forecol,backcol:integer):string;
var tempst:string;
    ch3:char;
    label bob;
begin
  tempst:='';
  ch3:='Û';
  repeat
    repeat until keypressed;
    ch3:=readkey;
    if (ch3=#8) and (ord(tempst[0])>0) then begin
       fillblock(xv,yv,ord(tempst[0])*8+xv,yv+8,backcol,vga);
       dec(tempst[0]);
    end;
    if length(tempst)>=howlong then goto bob;
    if ch3>#20 then tempst:=concat(tempst,ch3);
    if tempst[0]>#0 then
        outtextxy(tempst,xv,yv,forecol,backcol,vga,true);
    bob:
  until ch3=#13;
  grinput:=tempst;
end;

procedure loadshapes;
var x,y,i,j,shape:byte;
begin
   grapherror:=loadpicsuperpacked(0,0,vaddr,false,true,'ships.tb1');
   for i:=0 to 47 do
       for j:=0 to 29 do begin
           ShapeTable1^[bigship1off+((j*43)+i)]:=getpixel(i,j,vaddr);
           ShapeTable1^[bigship2off+((j*43)+i)]:=getpixel(i,j+32,vaddr);
           ShapeTable1^[bigship3off+((j*43)+i)]:=getpixel(i,j+64,vaddr);
       end;

 grapherror:=loadpicsuperpacked(0,0,vaddr,false,true,'tbshapes.tb1');
 for j:=0 to 1 do
  for i:=0 to 9 do
      for x:=0 to 17 do
          for y:=0 to 17 do
              ShapeTable1^[shapearray[(((j*10)+1)+i)]+((y*18)+x)]
                          :=getpixel(1+x+(i*19),1+y+(j*19),vaddr);
  cls(0,vaddr);
end;



procedure littleopener;
begin
  grapherror:=loadpicsuperpacked(0,0,vga,true,false,'moon2.tb1');
  fade;
  cls(0,vga);
  grapherror:=loadpicsuperpacked(0,0,vga,false,true,'moon2.tb1');
  for i:=0 to 15 do
       for j:=0 to 18 do begin
           ShapeTable1^[6000+((j*16)+i)]:=getpixel(i+9,j+178,vga);
           ShapeTable1^[6400+((j*16)+i)]:=getpixel(i+30,j+178,vga);
       end;
  fillblock(0,178,319,199,0,vga);
  unfade;
  flip(vga,vaddr);
  flip(vaddr,vaddr2);
  for i:=100 downto 0 do begin
      flip(vaddr2,vaddr);
      putshape(6400,vaddr,16,18,i,100);
      waitretrace;
      if keypressed then if readkey=#27 then exit;
      flip(vaddr,vga);
  end;
  flip(vaddr2,vga);
  outtextxy('>KCHK< TOM! WHERE ARE YOU GOING?',5,180,15,0,vga,true);
  pauseawhile(500); if keypressed then ch:=readkey;
  waitretrace;
  fillblock(0,178,319,199,0,vga);
  outtextxy('Ooops. ',5,180,24,0,vga,true);
  pauseawhile(500); if keypressed then ch:=readkey;
  for inte:=0 to 151 do begin
      flip(vaddr2,vaddr);
      putshape(6000,vaddr,16,18,inte*2,100);
      waitretrace;
      flip(vaddr,vga);
      if keypressed then if readkey=#27 then exit;
  end;
  flip(vaddr2,vga);
  pauseawhile(5);
  fade;
  cls(0,vga);
  unfade;
end;

procedure littleopener3;
var star_x:array[0..5]of integer;
    star_y:array[0..5]of integer;
begin
  loadlevel3shapes;
  grapherror:=loadpicsuperpacked(0,0,vga,true,false,'tbl2ship.tb1');
  fade;
  cls(0,vga);
  grapherror:=loadpicsuperpacked(0,0,vaddr,false,true,'tbl3intr.tb1');
  blockmove(0,3,171,117,vaddr,10,10,vga);
  putshape (shape3array[60],vga,10,9,113,52);


  unfade;
  outtextxy('Hmmmm... STUPID TRACTOR BEAM.',10,155,10,0,vga,false);
  outtextxy('I GUESS I''D BETTER GO SHUT IT OFF.',10,165,10,0,vga,false);
  pauseawhile(700);
  clearkeyboardbuffer;

  for i:=24 downto 0 do begin
      blockmove(0,3,171,117,vaddr,10,10,vga);
      putshape (shape3array[60+(4*(i mod 4))],vga,10,9,113,28+i);
      
      waitretrace; waitretrace; waitretrace;
      waitretrace; waitretrace; waitretrace;
  end;
  putshape (shape3array[60],vga,10,9,113,28);
  fillblock(10,155,300,185,0,vga);
  outtextxy('I''M LUCKY I WORE MAGNETIC SHOES.',10,155,12,0,vga,false);
  outtextxy('Hmmmm.  SOMEONE LEFT THE AIR-LOCK',10,165,12,0,vga,false);
  outtextxy('        UNLOCKED.  STRANGE.',10,175,12,0,vga,false);
  pauseawhile(600);
  clearkeyboardbuffer;
  putpixel(110,20,10,vga);
  putpixel(110,22,10,vga);
  fillblock(111,14,123,29,0,vga);
  fillblock(10,155,300,185,0,vga);
  outtextxy('I HOPE THIS ISN''T A TRAP.',10,155,9,0,vga,false);
  outtextxy('I WISH I HAD SOME FORM OF ',10,165,9,0,vga,false);
  outtextxy('        WEAPON.',10,175,9,0,vga,false);
  pauseawhile(600);
  clearkeyboardbuffer;
  fade;
  cls(0,vga);
  blockmove(179,41,287,134,vaddr,10,10,vga);
  for i:=0 to 5 do begin
      star_x[i]:=37+random(70);
      star_y[i]:=18+random(56);
      putpixel(star_x[i],star_y[i],15,vga);
  end;
  outtextxy('WOW!! A GLASS-WALLED AIR-LOCK.',10,135,9,0,vga,false);
  unfade;
  pauseawhile(500);
  clearkeyboardbuffer;
  fillblock(10,135,300,185,0,vga);
  outtextxy('NOW WHERE ARE WE GOING?',5,125,9,0,vga,false);
  outtextxy('I GUESS THE PLANET EERM.',5,135,9,0,vga,false);
  outtextxy('WHAT AN ODD NAME.',5,145,9,0,vga,false);
  outtextxy('AND WHY AM I TALKING TO MYSELF?',5,155,10,0,vga,false);
  outtextxy('ANYWAY I JUST WANT TO GO HOME',5,165,9,0,vga,false);
  outtextxy('       AND SLEEP.',5,175,9,0,vga,false);

  clearkeyboardbuffer;
  j:=0;
  while (j<2400) and (not(keypressed)) do begin
        inc(j);
        for i:=0 to 5 do begin
            putpixel(star_x[i],star_y[i],0,vga);
            inc(star_x[i]);
            if star_x[i]>107 then begin
               star_x[i]:=37;
               star_y[i]:=18+random(56);
            end;
            putpixel(star_x[i],star_y[i],15,vga);
        end;
        waitretrace; waitretrace;waitretrace;

  end;

  if keypressed then ch:=readkey;

  fade;
  cls(0,vga);
  unfade;
end;


procedure playthegame(lev:integer);

begin

  fade;
  cls(0,vga);
  unfade;
  cls(0,vaddr);
  level:=lev;
  if level=0 then begin
     littleopener;
     shields:=10;
     score:=0;
     inc(level);
  end;
  beginscore:=score;
  beginshields:=shields;
  loadshapes;
  grapherror:=loadpicsuperpacked(0,0,vga,true,false,'tbsobj.tb1');

  hiscore:=showhiscore(false,false);


  if level=1 then begin
     setupsidebar;
     y:=0;
     {levelone};
  end;
  if level=3 then begin
     setupsidebar;
     beginscore:=score;
     beginshields:=shields;
     littleopener3;
     levelthree;
  end;


  clearkeyboardbuffer;

  coolbox(70,85,170,110,true,vga);
  outtextxy('GAME OVER',84,94,4,7,vga,false);
  for inte:=0 to 100 do waitretrace;
  repeat until keypressed;  tempch:=readkey;
  clearkeyboardbuffer;

  hiscore:=showhiscore(false,false);
  if score>lowscore then begin
     coolbox(10,75,310,125,true,vga);
     outtextxy('NEW HIGH SCORE!',90,80,12,7,vga,false);
     outtextxy('ENTER YOUR NAME (10 Chars)',30,90,0,7,vga,false);
     hiname:=grinput(110,110,10,11,7);
     hiscore:=showhiscore(false,true);
  end;
  hiscore:=showhiscore(true,false);
  fade;
  cls(0,vga);
end;



procedure shadowrite(st:string;x5,y5,forecol,backcol:integer);
begin
   outtextxy(st,x5+1,y5+1,backcol,0,vga,false);
   outtextxy(st,x5,y5,forecol,0,vga,false);
end;



procedure QUIT;
label menu2;
begin
  coolbox(90,75,230,125,true,vga);
   barpos:=0;
   outtextxy('QUIT??? ARE YOU',97,82,9,7,vga,false);
   outtextxy('ABSOLUTELY SURE?',97,90,9,7,vga,false);
   repeat
     if barpos=0 then outtextxy('YES-RIGHT NOW!',97,98,150,0,vga,true)
        else outtextxy('YES-RIGHT NOW!',97,98,150,7,vga,true);
     if barpos=1 then outtextxy('NO--NOT YET.',97,106,150,0,vga,true)
        else outtextxy('NO--NOT YET.',97,106,150,7,vga,true);
     ch:=menuread;
     if (ord(ch)>219) and (ord(ch)<224) then inc(barpos);
     if ch='Y' then barpos:=0;
     if ch='N' then barpos:=1;
     if barpos=2 then barpos:=0;
   until ch=#13;
   if barpos=1 then goto menu2;
   settext;
   gotoxy(1,23);
   textcolor(7);
   {if sbsound then shutdownsb1;}
   shutdown;
   halt;
  menu2:
  barpos:=6;
end;
  




label picloader,menu;

begin
   setuptb1;
   ReadConfigFile;
   axel_und_carmen:=true;
      {as_of_9-22-94} {change_back_10-6-94} {uh_oh 2-21-95} {gone for real long time 10-12-95}
      {not carmen anymore, but Gus}

   randomize;
   setupvirtual;
   fade;
   setmcga;
    cls(0,vga);
   grapherror:=loadpicsuperpacked(0,0,vga,true,false,'tbomb1.tb1');
   score:=500;
   shields:=10;
   playthegame(3);
   unfade;
   quit;
 
end.