program TOM_BOMB_EM_AND_INVASION_OF_INANIMATE_OBJECTS;
        {by Vincent Weaver....21085-4706}

uses svmwgraph,nvmwgraph,crt,detect,smix,dos;

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
    vaddr,vaddr2,Vaddr3 : Word;                      { The segment of our 2nd virt. screen}
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
    shipadd,shipx,shipy,shipspeed,shipframe:integer;


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


procedure flip240(hm:word);
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

  flipd320 (vaddr,vaddr2);
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





Procedure leveltwoengine(l_level:byte);
VAR loop1,loop2:integer;
    ch,ch2:char;
    saucersout:integer;
    bullet1x,bullet1y,bullet2x,bullet2y,i:integer;
    bullet1out,bullet2out:boolean;
    whichone,temp,temp2:integer;
    tempst:string;
    what:byte;
    k:integer;
    itemp,jtemp:byte;
    whatdelay:byte;
    tempilg:byte;
    xx:integer;
   
   backrow,topblocky:integer;
   f:text;
   levelover:boolean;

procedure loadlevel2shapes;
var x,y,i,j,shape:byte;
begin
   clearshape;
   grapherror:=loadpicsuperpacked(0,0,vaddr,false,true,'ships.tb1');

   for i:=0 to 42 do
       for j:=0 to 30 do begin
           ShapeTable1^[bigship1off+((j*43)+i)]:=getpixel(i,j,vaddr);
           ShapeTable1^[bigship2off+((j*43)+i)]:=getpixel(i,j+32,vaddr);
           ShapeTable1^[bigship3off+((j*43)+i)]:=getpixel(i,j+64,vaddr);
       end;


 if l_level=2 then
    grapherror:=loadpicsuperpacked(0,0,vaddr,false,true,'tbaship.tb1');
 if l_level=4 then
    grapherror:=loadpicsuperpacked(0,0,vaddr,false,true,'tbeerm.tb1');
 for j:=0 to 3 do
  for i:=0 to 9 do
      for x:=0 to 19 do
          for y:=0 to 9 do
              ShapeTable1^[shape2array[(j*10)+i]+((y*20)+x)]
                          :=getpixel(1+x+(i*21),1+y+(j*11),vaddr);
  cls(0,vaddr);
end;



BEGIN
  loadlevel2shapes;
  if l_level=2 then assign(f,'level2.tb1');
  if l_level=4 then assign(f,'level4.tb1');
  reset(f);
  for j:=0 to 200 do
      for i:=0 to 11 do read(f,background[j,i]);
  close(f);

  for i:=0 to 30 do begin
      passive[i].dead:=true;
      passive[i].exploding:=false;
  end;
  for i:=0 to 5 do begin
      enemy[i].out:=false;
  end;


  backrow:=0;
  whatdelay:=1;
  shipx:=36;
  shipadd:=0;
  shipframe:=1;
  shipspeed:=5;
  ch:=#1; ch2:=#1;
  bullet1out:=false;  bullet2out:=false;
  bullet1x:=0; bullet1y:=0;
  bullet2x:=0; bullet2y:=0;

  flipd320(vaddr2,vaddr);
  str(level,tempst);
  fillblock(251,52,314,59,0,vaddr);
  outtextxy(tempst,307,51,12,0,vaddr,false);

  cls(0,vaddr2);

  for i:=0 to 100 do
      begin
         putshape240(14400{32},
                    vaddr2,20,9,random(237),random(227));
         putshape240(14400{shape2array[32]},
                    vaddr2,20,9,random(237),random(227));
      end;

  pal(254,shields*4,0,0);
  howmuchscroll:=50;

  cls(0,vga);
  coolbox(70,85,240,120,true,vga);
  if l_level=2 then begin
     outtextxy('   LEVEL TWO:',84,95,4,7,vga,false);
     outtextxy('THE "PEACE ENVOY"',84,105,4,7,vga,false);
  end;
  if l_level=4 then begin
     outtextxy('   LEVEL FOUR:',84,95,4,7,vga,false);
     outtextxy(' THE PLANET EERM',84,105,4,7,vga,false);
  end;

  clearkeyboardbuffer;
  pauseawhile(300);
  numpassive:=0;
  levelover:=false;
{**** GAME LOOP ****}
{*******************}
  Repeat
    ch2:=#1;
    ch:=#1;
    dec(howmuchscroll);
    if howmuchscroll<1 then begin
       flipd50(vaddr2,2250,vaddr2,3000);
       flipd50(vaddr2,1500,vaddr2,2250);
       flipd50(vaddr2,750,vaddr2,1500);
       flipd50(vaddr2,0,vaddr2,750);
       for i:=0 to 4 do begin
           outbackline(backrow,i);
           for xx:=0 to 11 do begin
               if (background[backrow,xx]>9) and
                  (background[backrow,xx]<20) then begin
                  k:=-1;
                  for j:=0 to numpassive do
                      if passive[j].dead=true then k:=j;
                  if k=-1 then begin
                     inc(numpassive); k:=numpassive; end;
                  with passive[k] do begin
                       dead:=false;
                       kind:=background[backrow,xx];
                       if (kind>10) and
                          (kind<15) then
                            shooting:=true
                            else shooting:=false;
                       exploding:=false;
                       x:=xx*20;
                       y:=-(((i+1)*10));
                       howmanyhits:=1;
                       lastshot:=0;
                  end;
               end;
           end;
           inc(backrow);
       end;
       flipd50(vaddr3,0,vaddr2,0);
       howmuchscroll:=50;
    end;

    flipd240(howmuchscroll,vaddr,vaddr2);

    topblocky:=backrow-(howmuchscroll div 10)-1;

{    str(passive[1].y,it);
   outtextxy(it,10,10,12,0,vaddr,true);

   str(passive[1].x,it);
   outtextxy(it,10,20,12,0,vaddr,true); }




{***Collision Check***}

  for i:=0 to 30 do if (passive[i].dead=false) and (passive[i].exploding=false) then with passive[i] do begin
      if (bullet1out) and collision(bullet1x,bullet1y,3,4,x,y,10,5)
         then begin
              if kind<>10 then begin
                 {if sbeffects then StartSound(Sound[3], 0, false);}
                 exploding:=true;
                 explodeprogress:=0;
                 bullet1out:=false;
                 inc(score,10);
                 changescore;
              end
              else begin
                 bullet1out:=false;
                 tempilg:=7;
                 for i:=0 to 5 do if enemy[i].out=false then tempilg:=i;
                 if tempilg<>7 then begin
                    enemy[tempilg].out:=true;
                    enemy[tempilg].y:=bullet1y;
                    enemy[tempilg].x:=bullet1x;
                    enemy[tempilg].yspeed:=7;
                    enemy[tempilg].kind:=21;
                  end;
              end;
         end;
      if (bullet2out) and collision(bullet2x,bullet2y,3,4,x,y,10,5)
         then begin
              if kind<>10 then begin
                 {if sbeffects then StartSound(Sound[3], 0, false);}
                 exploding:=true;
                 explodeprogress:=0;
                 bullet2out:=false;
                 inc(score,10);
                 changescore;
              end
              else begin
                   bullet2out:=false;
                  tempilg:=7;
                  for i:=0 to 5 do if enemy[i].out=false then tempilg:=i;
                  if tempilg<>7 then begin
                     enemy[tempilg].out:=true;
                     enemy[tempilg].y:=bullet2y;
                     enemy[tempilg].x:=bullet2x;
                     enemy[tempilg].yspeed:=7;
                     enemy[tempilg].kind:=21;
                  end;
              end;
         end;

      if (y>155) and (kind<>10) then begin
         if collision(x,y,10,5,shipx+16,165,5,5)
         or collision(x,y,10,5,shipx+6,175,18,8)
         then begin
           {if sbeffects then startsound(Sound[5],0,false);}
           {exploding:=true;
           explodeprogress:=2;}
           dead:=true;
           dec(shields);
           if shields<0 then levelover:=true;
           putshape240over(14800{shape2array[34]},vaddr2,
                       20,9,x,y+howmuchscroll);
           fillblock(250,71,314,79,0,vaddr);
           if shields>0 then begin
              pal(254,shields*4,0,0);
              for itemp:=0 to shields do
                  for jtemp:=71 to 78 do
                      horizontalline(250+(itemp*4),
                                     254+(itemp*4),jtemp,
                                     47-itemp,vaddr);
            end;
         end;
      end;
  end;

  for i:=0 to 5 do if (enemy[i].out) then
      if collision(enemy[i].x,enemy[i].y,2,5,shipx+16,165,5,5)
      or collision(enemy[i].x,enemy[i].y,2,5,shipx+6,175,18,8)
         then begin
              {if sbeffects then startsound(Sound[5],0,false);}
              enemy[i].out:=false;
              dec(shields);
              if shields<0 then levelover:=true;
              {putshape240over(shape2array[34],vaddr2,
                       20,9,passive[i].x,passive[i].y+howmuchscroll);}
              fillblock(250,71,314,79,0,vaddr);
              if shields>0 then begin
                 pal(254,shields*4,0,0);
                 for itemp:=0 to shields do
                     for jtemp:=71 to 78 do
                         horizontalline(250+(itemp*4),
                                     254+(itemp*4),jtemp,
                                     47-itemp,vaddr);
               end;

  end;
{  for i:=0 to 30 do if (passive[i].dead=false) and (passive[i].y>-1) then
      putshape(shape2array[16],
                           vaddr,20,9,passive[i].x,passive[i].y);}


{***DO EXPLOSIONS***}
    for i:=0 to 30 do
        if passive[i].exploding then with passive[i] do begin
           inc(explodeprogress);
           putshape240(shape2array[35+explodeprogress],vaddr2,
                          20,9,x,y+howmuchscroll);
           if explodeprogress>4 then begin
              dead:=true;
              exploding:=false;
              putshape240over(14800{shape2array[34]},vaddr2,
                          20,9,x,y+howmuchscroll);
           end;
        end;


{***MOVE BULLET***}
    if bullet1out then begin
       dec(bullet1y,5);
       if bullet1y<5 then bullet1out:=false;
       if bullet1out then putshape(12000,vaddr,20,9,bullet1x,bullet1y);
    end;
    if bullet2out then begin
       dec(bullet2y,5);
       if bullet2y<5 then bullet2out:=false;
       if bullet2out then putshape(12000,vaddr,20,9,bullet2x,bullet2y);
    end;
{***MOVE ENEMIES***}
    for j:=0 to 30 do begin
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


{***READ KEYBOARD***}

    if keypressed then BEGIN
      ch:=readkey;
      if ch=chr(0) then ch2:=readkey;
      if ch=#27 then levelover:=true;
      if ch2='M' then
         if shipadd>=0 then inc(shipadd,3) else shipadd:=0;
      if ch2='K' then
         if shipadd<=0 then dec(shipadd,3) else shipadd:=0;
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
      if (ch=' ') then begin
         if (bullet1out=false) then begin
            {if sbeffects then StartSound(Sound[4], 0, false);}
            bullet1out:=true;
            bullet1x:=shipx+21;
            bullet1y:=165;
            putshape(12000,vaddr,20,9,bullet1x,bullet1y);
         end
         else
         if (bullet2out=false) then begin
            {if sbeffects then StartSound(Sound[4], 0, false);}
            bullet2out:=true;
            bullet2x:=shipx+21;
            bullet2y:=165;
            putshape(12000,vaddr,20,9,bullet2x,bullet2y);
       end;
    end;

{***MOVE SHIP***}
        shipx:=shipx+shipadd;
        if (shipx<1) then shipx:=1;
        if (shipx>190) then shipx:=190;
        CASE shipframe of
           1   : putshape (bigship1off,vaddr,43,30,shipx,165);
           3   : putshape (bigship3off,vaddr,43,30,shipx,165);
           2,4 : putshape (bigship2off,vaddr,43,30,shipx,165);
        END;
        inc(shipframe);
        if shipframe=5 then shipframe:=1;
    waitretrace;
    flipd320 (vaddr,vga);
    for i:=0 to whatdelay do waitretrace;
    {str(backrow,testr);
         outtextxy(testr,181,46+(i*10),9,7,vga,true);}
  if backrow>=200 then begin
     clearkeyboardbuffer;
     pauseawhile(200);
     fade;
      grapherror:=LoadPicSuperPacked(0,0,vaddr,false,true,'viewscr.tb1');
      cls(0,vga);
      blockmove(0,79,58,116,vaddr,10,10,vga);
      clearkeyboardbuffer;
      outsmalltextxy('UNIDENTIFIED SPACECRAFT!',70,10,2,0,vga,true);
      outsmalltextxy('DO YOU WISH TO DEACTIVATE ',70,20,2,0,vga,true);
      outsmalltextxy('THIS SHIP''S SECURITY SYSTEMS? (Y/N)',70,30,2,0,vga,true);
     unfade;
      clearkeyboardbuffer;
      ch:='!';
      repeat
         if keypressed then ch:=readkey;
      until (upcase(ch)='Y') or (upcase(ch)='N');

      if upcase(ch)='N' then begin
         blockmove(0,79,58,116,vaddr,10,50,vga);
         outsmalltextxy('NO?  AFFIRMATIVE. ',70,50,9,0,vga,true);
         outsmalltextxy('ARMING REMOTE DESTRUCTION RAY.',70,60,9,0,vga,true);
         outsmalltextxy('GOOD-BYE.',70,70,9,0,vga,true);
         pauseawhile(400);
         fade;
      end;

      if upcase(ch)='Y' then begin
         blockmove(0,79,58,116,vaddr,10,50,vga);
         outsmalltextxy('"Y"=CORRECT PASSWORD. ',70,50,2,0,vga,true);
         outsmalltextxy('WELCOME SUPREME TENTACLEE COMMANDER.',70,60,2,0,vga,true);
         outsmalltextxy('INITIATING TRACTOR BEAM AND AUTOMATIC',70,70,2,0,vga,true);
         outsmalltextxy('LANDING PROCEDURE.',70,80,2,0,vga,true);
         outsmalltextxy('WE WILL BE DEPARTING FOR THE PLANET',70,90,2,0,vga,true);
         outsmalltextxy('EERM IN THREE MICROCYCLE UNITS.',70,100,2,0,vga,true);
         pauseawhile(550);
         clearkeyboardbuffer;
         blockmove(0,42,58,79,vaddr,10,110,vga);
         outsmalltextxy('Wha? Wait!',70,110,9,0,vga,true);
         outsmalltextxy('What''s happening?',70,120,9,0,vga,true);
         pauseawhile(550);
         fade;
     end;

         grapherror:=LoadPicSuperPacked(0,0,vaddr,false,true,'tbtract.tb1');
         for i:=0 to 239 do
             for j:=0 to 49 do
                 putpixel240(i,j,getpixel(i,j,vaddr),vaddr2);
         cls(0,vga);
         unfade;
         for howmuchscroll:=50 downto 1 do begin
            flipd240(howmuchscroll,vaddr,vaddr2);
            putshape (bigship3off,vaddr,43,30,shipx,165);
            waitretrace;
            flipd320(vaddr,vga);
        end;

        if upcase(ch)='N' then begin
           clearkeyboardbuffer;
           line(7,6,shipx+10,180,4,vga);
           line(shipx+37,180,231,6,4,vga);
           pauseawhile(50);
           clearkeyboardbuffer;
           for i:=shipx to shipx+48 do
               verticalline(165,195,i,4,vga);
           pauseawhile(200);
           flipd240(howmuchscroll,vaddr,vaddr2);
           flipd320(vaddr,vga);
           pauseawhile(150);
        end;


     if upcase(ch)='Y' then begin;
        shipadd:=sgn(shipx-95);
        shipy:=165;
        repeat
          if shipx<>95 then shipx:=shipx-shipadd;
          if shipy>9 then dec(shipy);
          flipd240(howmuchscroll,vaddr,vaddr2);
          line(7,6,shipx+10,shipy+15,2,vaddr);
          line(shipx+37,shipy+15,231,6,2,vaddr);
          putshape (bigship3off,vaddr,43,30,shipx,shipy);
          waitretrace;
          flipd320(vaddr,vga);
        until (shipx=95) and (shipy=9);
        clearkeyboardbuffer;
     pauseawhile(850);
     fade;
        cls(0,vga);


     while keypressed do ch:=readkey;
     if level=4 then begin
       outsmalltextxy('THE PLANET EERM?',20,20,10,0,vga,true);
       outsmalltextxy('XENOCIDE FLEET?',20,30,10,0,vga,true);
       outsmalltextxy('WHAT''S GOING ON?',20,40,10,0,vga,true);
       outsmalltextxy('A MAJOR GOVERNMENT CONSPIRACY?  MASS HALUCINATIONS?',20,50,10,0,vga,true);

       outsmalltextxy('WATCH FOR TOM BOMBEM LEVEL 3 (CURRENTLY IN THE DESIGN PHASE).',10,70,12,0,vga,true);
       outsmalltextxy('ALL THESE QUESTIONS WILL BE ANSWERED!',10,80,12,0,vga,true);
       outsmalltextxy('ALSO MORE FEATURES WILL BE ADDED:',10,90,12,0,vga,true);
       outsmalltextxy('     BETTER GRAPHICS, SOUND AND SPEED.',10,100,12,0,vga,true);

       outsmalltextxy('TO HASTEN COMPLETION, SEND QUESTIONS/COMMENTS/DONATIONS TO ',9,120,9,0,vga,true);
       outsmalltextxy('THE AUTHOR (SEE THE REGISTER MESSAGE FOR RELEVANT ADDRESSES).',9,130,9,0,vga,true);

       outsmalltextxy('THANK YOU FOR PLAYING TOM BOMBEM',80,150,14,0,vga,true);
       unfade;
       pauseawhile(1800);
     end;
     inc(level);
     levelover:=true;
    end;
  end;

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
  flipd320(vga,vaddr);
  flipd320(vaddr,vaddr2);
  for i:=100 downto 0 do begin
      flipd320(vaddr2,vaddr);
      putshape(6400,vaddr,16,18,i,100);
      waitretrace;
      if keypressed then if readkey=#27 then exit;
      flipd320(vaddr,vga);
  end;
  flipd320(vaddr2,vga);
  outtextxy('>KCHK< TOM! WHERE ARE YOU GOING?',5,180,15,0,vga,true);
  pauseawhile(500); if keypressed then ch:=readkey;
  waitretrace;
  fillblock(0,178,319,199,0,vga);
  outtextxy('Ooops. ',5,180,24,0,vga,true);
  pauseawhile(500); if keypressed then ch:=readkey;
  for inte:=0 to 151 do begin
      flipd320(vaddr2,vaddr);
      putshape(6000,vaddr,16,18,inte*2,100);
      waitretrace;
      flipd320(vaddr,vga);
      if keypressed then if readkey=#27 then exit;
  end;
  flipd320(vaddr2,vga);
  pauseawhile(5);
  fade;
  cls(0,vga);
  unfade;
end;

procedure littleopener2;
begin
  grapherror:=loadpicsuperpacked(0,0,vga,true,false,'tbl2ship.tb1');
  fade;
  cls(0,vga);
  grapherror:=loadpicsuperpacked(0,0,vga,false,true,'tbl2ship.tb1');
  unfade;
  outtextxy('Hmmmm... ',10,10,4,0,vga,false);
  outtextxy('This Might Be Harder Than I Thought.',10,20,4,0,vga,false);
  pauseawhile(500); if keypressed then ch:=readkey;
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
  if level=2 then begin
     littleopener2;
     setupsidebar;
     beginscore:=score;
     beginshields:=shields;
     leveltwoengine(2);
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
   playthegame(2);
   unfade;
   quit;
 
end.