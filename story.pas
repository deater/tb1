program story_from_tom_bombem;
uses nvmwgraph,crt;


{$G+}

const
  {Shape Table Values For Story}
    bargeoff       = 0;
    bigflame1off = 290;
    bigflame2off = 780;
    sflame2off   = 1270;
    sflame1off   = 1290;
    truck1off    = 1310;
    truck2off    = 1370;
    explo1off    = 1430;
    explo2off    = 1730;
    rent1off     = 2030;
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


type ScreenType = array [0..3999] of byte;  {For Ansi Loading}

     ShipInfo = Record
                 x,y:integer;
                 speed,frame:integer;
                 active:boolean;
               END;

     EnemyInfo = record
               x,y,oldx,oldy:integer;
               kind:byte;
               out,exploding:boolean;
               explodprogress:byte;
               minx,maxx,xspeed,yspeed:integer;
             end;

     Virtual = Array [1..64000] of byte;  { The size of our Virtual Screen }
     VirtPtr = ^Virtual;                  { Pointer to the virtual screen }

     ShapeTable = Array [0..16000] of byte;
     ShapePtr = ^ShapeTable;

var ho,mi,se,s100:word;
    tempsec:integer;
    flames:boolean;
    error:byte;
    dev,mix,stat,pro,loop : integer;
    md : string;

VAR Virscr : VirtPtr;                      { Our first Virtual screen }
    ShapeTable1:ShapePtr;
    num:byte;
    VirScr2 : VirtPtr;                     { Our second Virtual screen }
    Vaddr  : word;                      { The segment of our virtual screen}
    Vaddr2 : Word;                      { The segment of our 2nd virt. screen}
    ShapeAddr:word;                     { The segment of the shape table}
    ShapeOfs:word;
    ourpal : Array [0..255,1..3] of byte; { A virtual pallette }
    shipv : Array [1..1] of shipinfo; { The toaster info }
    enemy:array[0..5] of enemyinfo;
    Soundp : Pointer;
    Check : BOOLEAN;

    hiscore,level,lives,energy:integer;
    score:longint;
    axel_und_carmen:boolean;
    tempi,tempj:integer;
    scorest:string[8];
    musicon:boolean;
    lowscore:integer;
    it,hiname:string[10];

var grapherror:byte;
    temp:array[1..3] of byte;
    i,j:byte;
    x,y,barpos:integer;
    screen:screentype absolute $B800:0000;
    ch:char;


Procedure SetUpVirtual;
   { This sets up the memory needed for the virtual screen }
BEGIN
  GetMem (VirScr,64000);
  vaddr := seg (virscr^);
  GetMem (VirScr2,64000);
  vaddr2 := seg (virscr2^);
  GetMem (ShapeTable1,16000);
  shapeaddr:= seg (ShapeTable1^);
  shapeofs:=ofs (ShapeTable1^);
END;



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
   end;
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
    rep     movsb
    add     di,bx        { 320 - 48 = 272 .. or 110 in hex }
    add     si,bx
    dec     ax
    jnz     @@1

    pop     es
    pop     ds
  end;
  { I wrote this procedure late last night, so it may not be in it's
    most optimised state. Sorry :-)}
END;





{************
*************
************
************}


procedure story;
var error:byte;
    xtemp,ytemp:integer;
    thrustcol:integer;
    thrust:real;
    tempch:char;
    alienchar:byte;
    cycles:byte;




procedure doflames;
begin
   
   if flames=true then begin
      putshape(bigflame1off,vaddr,27,17,213,100);
      putshapeover(sflame2off,vaddr,4,4,105,90);
      putshapeover(sflame1off,vaddr,4,4,151,71);
      putshapeover(sflame2off,vaddr,4,4,218,72);
    end
    else
       begin
       putshape(bigflame2off,vaddr,27,17,213,100);
       putshapeover(sflame1off,vaddr,4,4,105,90);
       putshapeover(sflame2off,vaddr,4,4,151,71);
       putshapeover(sflame1off,vaddr,4,4,218,72);
       outtextxy(chr(alienchar),10,10,12,0,vaddr,true);
       inc(alienchar);
       if alienchar>44 then alienchar:=34;
    end;
    flip(vaddr,vga);
    vdelay(5);
    flames:=not(flames);
end;

procedure puttruck(xp,yp,frame:integer;where:word);
var xtemp,into,ytemp:integer;
    col:byte;
begin
  for into:=0 to 7 do begin
     if(into+yp>=172) then
      if frame=0 then putshapeline(truck1off,where,6,8,xp,yp,into)
                 else putshapeline(truck2off,where,6,8,xp,yp,into);
  end;
end;

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


begin
  fade;
  error:=loadpicsuperpacked(0,0,vaddr,false,true,'tbsobj.tb1');
  for ytemp:=0 to 18 do
      for xtemp:=0 to 26 do begin
          ShapeTable1^[((ytemp*27)+xtemp)+bigflame1off]:=getpixel(xtemp,ytemp+2,vaddr);
          ShapeTable1^[((ytemp*27)+xtemp)+bigflame2off]:=getpixel(xtemp,ytemp+21,vaddr);
      end;
  for ytemp:=0 to 18 do
      for xtemp:=0 to 15 do begin
          ShapeTable1^[((ytemp*16)+xtemp)+explo1off]:=getpixel(127+xtemp,ytemp+100,vaddr);
          ShapeTable1^[((ytemp*16)+xtemp)+explo2off]:=getpixel(148+xtemp,ytemp+100,vaddr);
  end;
  for ytemp:=0 to 6 do
      for xtemp:=0 to 15 do
          ShapeTable1^[((ytemp*16)+xtemp)+rent1off]:=getpixel(168+xtemp,ytemp+100,vaddr);

  for ytemp:=0 to 4 do
      for xtemp:=0 to 3 do begin
          ShapeTable1^[((ytemp*4)+xtemp)+sflame1off]:=getpixel(xtemp,ytemp+43,vaddr);
          ShapeTable1^[((ytemp*4)+xtemp)+sflame2off]:=getpixel(xtemp,ytemp+47,vaddr);
      end;

  for ytemp:=0 to 18 do
      for xtemp:=0 to 15 do
          ShapeTable1^[((ytemp*16)+xtemp)+bargeoff]:=getpixel(xtemp+65,ytemp+100,vaddr);

  for ytemp:=0 to 8 do
      for xtemp:=0 to 5 do begin
          ShapeTable1^[((ytemp*6)+xtemp)+truck1off]:=getpixel(xtemp+85,ytemp+100,vaddr);
          ShapeTable1^[((ytemp*6)+xtemp)+truck2off]:=getpixel(xtemp+95,ytemp+100,vaddr);
      end;

{******FIRST MESSAGE*******}
  cls(0,vga);
  outtextxy('THE STORY SO FAR...',20,20,4,0,vga,false);
  unfade;
  pauseawhile(300); if keypressed then if readkey=#27 then exit;
  fade;
  cls(0,vga);
  error:=loadpicsuperpacked(0,0,vaddr2,false,true,'tbcobj.tb1');
  blockmove(129,56,178,188,vaddr2,10,10,vga);
  outtextxy('YOU ARE TOM BOMBEM,  A STRANGE',80,10,1,0,vga,false);
  outtextxy('    BUT EFFICIENT MEMBER OF',80,20,1,0,vga,false);
  outtextxy('    THE LUNAR SPACE FORCE.',80,30,1,0,vga,false);
  outtextxy('YOU NEVER SAY MUCH AND YOU ARE',80,50,4,0,vga,false);
  outtextxy('    RARELY SEEN OUTSIDE OF ',80,60,4,0,vga,false);
  outtextxy('    YOUR BLUE SPACESUIT.',80,70,4,0,vga,false);
  outtextxy('YOU OFTEN GET YOURSELF IN ',80,90,2,0,vga,false);
  outtextxy('    TROUBLE BY SCRATCHING',80,100,2,0,vga,false);
  outtextxy('    YOUR HEAD AT INAPPROPRIATE',80,110,2,0,vga,false);
  outtextxy('    TIMES.',80,120,2,0,vga,false);
  outtextxy('PRESS ANY KEY....',96,185,15,0,vga,false);
  unfade;
  pauseawhile(900); if keypressed then if readkey=#27 then exit;
  fade;
  cls(0,vga);
  blockmove(129,56,178,188,vaddr2,260,10,vga);
  blockmove(99,104,128,185,vaddr2,287,13,vga);
  outtextxy('IT IS THE YEAR 2028.',10,10,1,0,vga,false);
  outtextxy('YOU HAVE BEEN SUMMONED BY',10,30,3,0,vga,false);
  outtextxy('    LUNAR DICTATOR-IN-CHIEF',10,40,3,0,vga,false);
  outtextxy('    VINCENT WEAVER ABOUT A',10,50,3,0,vga,false);
  outtextxy('    TOP SECRET THREAT TO ',10,60,3,0,vga,false);
  outtextxy('    INTERPLANETARY SECURITY.',10,70,3,0,vga,false);
  outtextxy('YOU ATTEND THE BRIEFING WITH',10,90,5,0,vga,false);
  outtextxy('    YOUR USUAL CONFUSED',10,100,5,0,vga,false);
  outtextxy('    QUIETNESS.  YOU STILL DO',10,110,5,0,vga,false);
  outtextxy('    NOT UNDERSTAND YOUR OWN',10,120,5,0,vga,false);
  outtextxy('    SUCCESSFULNESS.',10,130,5,0,vga,false);
  outtextxy('PRESS ANY KEY....',96,185,15,0,vga,false);
  unfade;
  pauseawhile(900); if keypressed then if readkey=#27 then exit;
  fade;

  error:=loadpicsuperpacked(0,0,vga,false,true,'tbchief.tb1');

  unfade;
  blockmove(115,55,206,114,vga,115,55,vaddr);
  blockmove(188,14,279,73,vaddr2,115,55,vga);
  pauseawhile(600); if keypressed then if readkey=#27 then exit;
  blockmove(115,55,206,114,vaddr,115,55,vga);
  outtextxy('Ahhh.... Mr. Bombem.... ',1,1,15,0,vga,true);
  pauseawhile(400); if keypressed then if readkey=#27 then exit;
  outtextxy('I''ll be brief.                       ',1,1,15,0,vga,true);
  pauseawhile(400); if keypressed then if readkey=#27 then exit;
  outtextxy('Do you know how this base was founded?',1,1,15,0,vga,true);
  pauseawhile(600); if keypressed then if readkey=#27 then exit;
  outtextxy('No?  Well watch the screen.             ',1,1,15,0,vga,true);
  pauseawhile(400); if keypressed then if readkey=#27 then exit;
  blockmove(210,75,295,134,vaddr2,210,136,vga);
  pauseawhile(300); if keypressed then if readkey=#27 then exit;

  pal(250,0,0,0);
  fade;

{******BARGE TAKING OFF**********}
  error:=loadpicsuperpacked(0,0,vaddr2,false,true,'tbma1.tb1');
  outsmalltextxy('MY WIFE AND I FOUNDED',212,3,14,0,vaddr2,false);
  outsmalltextxy('THIS BASE IN 2008.',212,9,14,0,vaddr2,false);
  outsmalltextxy('THE ONLY WAY TO ',212,16,13,0,vaddr2,false);
  outsmalltextxy('FINANCE IT WAS TO',212,22,13,0,vaddr2,false);
  outsmalltextxy('ENGAGE IN A DUBIOUS',212,28,13,0,vaddr2,false);
  outsmalltextxy('BUSINESS.',212,34,13,0,vaddr2,false);
  outsmalltextxy('WE LAUNCHED EARTH''S',212,41,12,0,vaddr2,false);
  outsmalltextxy('TRASH INTO SPACE',212,47,12,0,vaddr2,false);
  outsmalltextxy('FOR A PROFIT.',212,53,12,0,vaddr2,false);
  outsmalltextxy('HERE IS FOOTAGE FROM',212,60,11,0,vaddr2,false);
  outsmalltextxy('THE FINAL LAUNCH FIVE',212,66,11,0,vaddr2,false);
  outsmalltextxy('YEARS AGO.',212,72,11,0,vaddr2,false);
  flip(vaddr2,vaddr);
  putshape(bargeoff,vaddr,16,18,141,157);
  flip(vaddr,vga);
  unfade;

  pauseawhile(700); if keypressed then if readkey=#27 then exit;

  for ytemp:=191 downto 164 do begin
      if ytemp>=172 then blockmove(145,ytemp,152,ytemp+10,vaddr2,145,ytemp,vaddr)
                    else blockmove(145,172,152,182,vaddr2,145,172,vaddr);
      puttruck(145,ytemp,ytemp mod 2,vaddr);
      pauseawhile(7);
      if keypressed then if readkey=#27 then exit;
      flip(vaddr,vga);
  end;

  pauseawhile(20);
  flip(vaddr2,vaddr);
  putshape(bargeoff,vaddr,16,18,141,157);
  thrustcol:=0;
  ytemp:=157;
  thrust:=0;
  while ytemp>-25 do begin
      thrust:=thrust+0.05;
      if thrustcol<63 then inc(thrustcol);
      blockmove(141,ytemp,171,ytemp+30,vaddr2,141,ytemp,vaddr);
      for i:=1 to 17 do
          if ytemp+i>=0 then
          putshapeline(bargeoff,vaddr,16,18,141,ytemp,i);
      pauseawhile(5);
      if keypressed then if readkey=#27 then exit;
      pal(250,thrustcol,0,0);
      flip(vaddr,vga);
      ytemp:=ytemp-round(thrust);
  end;

  pauseawhile(100);
  if keypressed then if readkey=#27 then exit;
  fade;

{****SECOND CHIEF*******}
 error:=loadpicsuperpacked(0,0,vaddr2,false,true,'tbcobj.tb1');
 error:=loadpicsuperpacked(0,0,vga,false,true,'tbchief.tb1');
 blockmove(7,104,97,125,vaddr2,6,174,vga);
  unfade;
 outtextxy('You might wonder why this is important.',1,1,15,0,vga,true);
  pauseawhile(600); if keypressed then if readkey=#27 then exit;
 outtextxy('Last week we received a message.       ',1,1,15,0,vga,true);
  pauseawhile(600); if keypressed then if readkey=#27 then exit;
 outtextxy('It is of extra-terrestrial origin.     ',1,1,15,0,vga,true);
  pauseawhile(600); if keypressed then if readkey=#27 then exit;
 outtextxy('Watch the screen.                      ',1,1,15,0,vga,true);
  blockmove(210,136,295,195,vaddr2,210,136,vga);
  pauseawhile(300); if keypressed then if readkey=#27 then exit;
 fade;

{******ALIEN DELEGATION*****}
  error:=loadpicsuperpacked(0,0,vaddr2,false,true,'tbcrash.tb1');
  alienchar:=34;
  flip(vaddr2,vaddr);
  flip(vaddr,vga);
  putshape(bargeoff,vaddr,16,18,97,180);
  unfade;
  xtemp:=97;
  ytemp:=181;
  flames:=true;
     while ytemp>118 do begin
      blockmove(xtemp,ytemp,xtemp+20,ytemp+20,vaddr2,xtemp,ytemp,vaddr);
      putshape(bargeoff,vaddr,16,18,xtemp,ytemp);
      doflames;
      dec(ytemp);
      inc(xtemp);
      pauseawhile(1);
      if keypressed then if readkey=#27 then exit;
     end;
      blockmove(xtemp,ytemp,xtemp+30,ytemp+30,vaddr2,xtemp,ytemp,vaddr);
     pal(0,63,63,63);
     putshape(explo1off,vga,16,18,160,118);
     pauseawhile(5);
     if keypressed then if readkey=#27 then exit;
     pal(0,0,0,0);
     for xtemp:=0 to 10 do begin
        putshapeover(explo2off,vaddr,16,18,160,118);
        doflames;
        putshapeover(explo1off,vaddr,16,18,160,118);
        doflames;
     end;
     blockmove(159,114,189,144,vaddr2,159,114,vaddr);
     putshapeover(rent1off,vaddr,16,6,160,115);
     flip(vaddr,vga);
     cycles:=0;
  repeat
    doflames;
    inc(cycles);
  until ((keypressed) or (cycles>60));
  if keypressed then if readkey=#27 then exit;

{****ALIEN MESSAGE*****}
  fade;
  error:=loadpicsuperpacked(0,0,vga,false,true,'tbgorg.tb1');
  unfade;
  outtextxy('GREETINGS EARTHLINGS.',0,162,12,0,vga,false);
  outtextxy('I AM GORGONZOLA THE REPULSIVE.',0,171,12,0,vga,false);
  outtextxy('YOU HAVE MADE A BIG MISTAKE.',0,180,12,0,vga,false);
  pauseawhile(600);
  if keypressed then if readkey=#27 then exit;
  outtextxy('YOUR SHIP FULL OF REFUSE HAS',0,162,12,0,vga,true);
  outtextxy('DAMAGED OUR OFFICIAL PEACE    ',0,171,12,0,vga,true);
  outtextxy('ENVOY.  IT WAS ON ITS WAY TO ',0,180,12,0,vga,true);
  outtextxy('YOUR PLANET.                  ',0,189,12,0,vga,true);
  pauseawhile(600);
  if keypressed then if readkey=#27 then exit;
  outtextxy('IN AN IRONIC FORM OF RETALLIATION',0,162,12,0,vga,true);
  outtextxy('WE HAVE MADE YOUR TRASH EVIL AND',0,171,12,0,vga,true);
  outtextxy('TURNED IT AGAINST YOU.          ',0,180,12,0,vga,true);
  outtextxy('        DIE EARTH SCUM!         ',0,189,12,0,vga,true);
  pauseawhile(600);
  if keypressed then if readkey=#27 then exit;
  fade;
{****** THIRD CHIEF *******}
 error:=loadpicsuperpacked(0,0,vaddr2,false,true,'tbcobj.tb1');
 error:=loadpicsuperpacked(0,0,vga,false,true,'tbchief.tb1');
 blockmove(7,127,97,148,vaddr2,6,174,vga);
  unfade;
 outtextxy('Tom, our radar detects approaching ',1,1,15,0,vga,true);
  pauseawhile(500); if keypressed then if readkey=#27 then exit;
 outtextxy('objects.  They are inside the      ',1,1,15,0,vga,true);
  pauseawhile(500); if keypressed then if readkey=#27 then exit;
 outtextxy('orbit of Jupiter.                  ',1,1,15,0,vga,true);
  pauseawhile(500); if keypressed then if readkey=#27 then exit;
 outtextxy('You are our only hope!             ',1,1,15,0,vga,true);
  pauseawhile(500); if keypressed then if readkey=#27 then exit;
 outtextxy('Will you fly our only spaceship    ',1,1,15,0,vga,true);
  pauseawhile(500); if keypressed then if readkey=#27 then exit;
 outtextxy('and save the human race?           ',1,1,15,0,vga,true);
  pauseawhile(500); if keypressed then if readkey=#27 then exit;
 blockmove(5,16,44,98,vaddr2,146,59,vga);
  pauseawhile(10);
 blockmove(46,16,85,98,vaddr2,146,59,vga);
  pauseawhile(10);
 blockmove(87,16,126,98,vaddr2,146,59,vga);
 outtextxy('Scratch.  Scratch. <Ow that itches>',1,1,9,0,vga,true);
 pauseawhile(300); if keypressed then if readkey=#27 then exit;
 outtextxy('I knew you''d do it. Good Luck!     ',1,1,15,0,vga,true);
 pauseawhile(600); if keypressed then if readkey=#27 then exit;
 outtextxy('<Huh?>                             ',1,1,9,0,vga,true);
 pauseawhile(600); if keypressed then if readkey=#27 then exit;
end;

begin
  setmcga;
  setuptb1;
  setupvirtual;
  error:=loadpicsuperpacked(0,0,vga,true,false,'tbma1.tb1');
  story;
  unfade;
  settext;
end.