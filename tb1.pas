program TOM_BOMB_EM_AND_INVASION_OF_INANIMATE_OBJECTS;
        {by Vincent Weaver....21085-4706}

uses vmwgraph,crt;

{$I c:\vmw\pascal\programs\tb1ans.pas}
{$M $4000,0,$20000} 			{16k stack, no heap - adjust as needed }
{$L MOD-obj.OBJ} 	        { Link in Object file }
{$F+} 				{ force calls to be 'far'}
procedure modvolume(v1,v2,v3,v4:integer); external ; {Can do while playing}
procedure moddevice(var device:integer); external ;
procedure modsetup(var status:integer;device,mixspeed,pro,loop:integer;var str:string); external ;
procedure modstop; external ;
procedure modinit; external;
{$F-}





type screentype = array [0..3999] of byte;
Type Shipinfo = Record                 { This is format of of each of our }
                 x,y:integer;              { records for the flying toasters }
                 speed,frame:integer;
                 active:boolean;
               END;



     Virtual = Array [1..64000] of byte;  { The size of our Virtual Screen }
     VirtPtr = ^Virtual;                  { Pointer to the virtual screen }



var
 dev,mix,stat,pro,loop : integer;
 md : string;
VAR Virscr : VirtPtr;                      { Our first Virtual screen }
    VirScr2 : VirtPtr;                     { Our second Virtual screen }
    Vaddr  : word;                      { The segment of our virtual screen}
    Vaddr2 : Word;                      { The segment of our 2nd virt. screen}
    ourpal : Array [0..255,1..3] of byte; { A virtual pallette }
    shipv : Array [1..1] of shipinfo; { The toaster info }
    Sound : Pointer;
    Check : BOOLEAN;
    frame:array[0..2,0..47,0..29] of byte;
    bigflame:array[0..1,0..26,0..18] of byte;
    smallflame:array[0..1,0..3,0..4] of byte;
    barge:array[0..15,0..18] of byte;
    truck:array[0..1,0..5,0..8] of byte;
    saucer:array[0..20,0..5] of byte;
    shot:array[0..20,0..5] of byte;
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
    palf:text;
    i,j:byte;
    x,y,barpos:integer;
    screen:screentype absolute $B800:0000;
    ch:char;

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
     if ch2='M' then chtemp:='Ş';  {right}
     if ch2='P' then chtemp:='Ü';  {down}
     if ch2='K' then chtemp:='İ';  {left}
     if ch2=';' then chtemp:='¨';  {f1}
     if ch2='I' then chtemp:='ô';  {pgup}
     if ch2='Q' then chtemp:='õ';  {pgdown}
  end;
  menuread:=chtemp;
end;

procedure coolbox(x1,y1,x2,y2:integer;fill:boolean;page:word);
begin
   for i:=0 to 5 do box(x1+i,y1+i,x2-i,y2-i,31-i,page);
   if fill then for i:=y1+5 to y2-5 do line(x1+5,i,x2-5,i,7,page);
end;

{ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ}
Procedure ShutDown;
   { This frees the memory used by the virtual screen }
BEGIN
  FreeMem (VirScr,64000);
  FreeMem (VirScr2,64000);
END;



procedure QUIT;
label menu2;
begin
  coolbox(90,75,230,125,true,vga);
   barpos:=0;
   outtextxy('QUIT??? ARE YOU',97,82,15,9,7,vga,false);
   outtextxy('ABSOLUTELY SURE?',97,90,15,9,7,vga,false);
   repeat
     if barpos=0 then outtextxy('YES-RIGHT NOW!',97,98,15,150,0,vga,true)
        else outtextxy('YES-RIGHT NOW!',97,98,15,150,7,vga,true);
     if barpos=1 then outtextxy('NO--NOT YET.',97,106,15,150,0,vga,true)
        else outtextxy('NO--NOT YET.',97,106,15,150,7,vga,true);
     ch:=menuread;
     if (ord(ch)>219) and (ord(ch)<224) then inc(barpos);
     if ch='Y' then barpos:=0;
     if ch='N' then barpos:=1;
     if barpos=2 then barpos:=0;
   until ch=#13;
   if barpos=1 then goto menu2;
   settext;
   move(imagedata,screen,4000);
   gotoxy(1,23);
   halt;
  menu2:
  barpos:=6;
end;
  
Procedure SetUpVirtual;
   { This sets up the memory needed for the virtual screen }
BEGIN
  GetMem (VirScr,64000);
  vaddr := seg (virscr^);
  GetMem (VirScr2,64000);
  vaddr2 := seg (virscr2^);
END;

function showhiscore(showchart,savelist:boolean):integer;
var hilist:text;
    testr:string[10];
    place,tempi:integer;
    names:array[0..9] of string[10];
    scores:array[0..9] of longint;

begin
   assign(hilist,'HISCORE.TB1');
   reset(hilist);
     for i:=0 to 9 do readln(hilist,names[i]);
     for i:=0 to 9 do readln(hilist,scores[i]);
   close(hilist);
   lowscore:=scores[9];
   showhiscore:=scores[0];
   if showchart then begin
     for i:=40 to 160 do line(45,i,285,i,7,vga);
     coolbox(45,40,285,160,false,vga);
     for i:=0 to 9 do
         outtextxy(names[i],51,46+(i*10),15,9,7,vga,true);
     for i:=0 to 9 do begin
         str(scores[i],testr);
         outtextxy(testr,181,46+(i*10),15,9,7,vga,true);
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




{ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ}
Procedure drawstars(menu:boolean);
  { This procedure sets up the static background to be used in the program }

CONST circ : Array [1..5,1..5] of byte =
        ((00,00,30,00,00),
         (00,00,26,00,00),
         (30,26,18,26,30),
         (00,00,26,00,00),
         (00,00,30,00,00));

VAR i,x,y:integer;
    loop1,loop2,loop3:integer;
    tempaddr:word;

procedure makehole(y:integer);
var i:integer;
begin
  for i:=10 to 75 do line(239+i,y,239+i,y+9,0,tempaddr);
  line(249,y,314,y,24,tempaddr);
  line(249,y+10,313,y+10,18,tempaddr);
  line(249,y,249,y+9,24,tempaddr);
  line(314,y+1,314,y+10,18,tempaddr);
end;
BEGIN
   tempaddr:=vaddr;

  
  For loop1:=1 to 200 do BEGIN
    x:=random (315);
    y:=random (195);
    For loop2:=1 to 5 do
      For loop3:=1 to 5 do
        if circ [loop2,loop3]<>0 then
          putpixel (x+loop2,y+loop3,circ [loop2,loop3],tempaddr);
  END;
  if menu=false then begin
  for i:=240 to 319 do line(i,0,i,199,19,tempaddr);
   line(240,0,240,199,18,tempaddr);
   line(240,0,319,0,18,tempaddr);
   line(319,0,319,199,24,tempaddr);
   line(241,199,319,199,24,tempaddr);
  outtextxy('SCORE',241,1,15,127,0,tempaddr,false);
  outtextxy('SCORE',242,2,15,143,0,tempaddr,false);
    makehole(10);
  outtextxy('       0',250,12,15,12,0,tempaddr,false);
  outtextxy('HI-SCORE',241,21,15,127,0,tempaddr,false);
  outtextxy('HI-SCORE',242,22,15,143,0,tempaddr,false);
  str(hiscore,it);
  outtextxy(it,250,32,15,12,0,tempaddr,false);
  makehole(30);
  outtextxy('       0',250,32,15,12,0,tempaddr,false);
  outtextxy('LEVEL',241,41,15,127,0,tempaddr,false);
  outtextxy('LEVEL',242,42,15,143,0,tempaddr,false);
  makehole(50);
  outtextxy('12345675',251,52,15,12,0,tempaddr,false);
  outtextxy('SHIELDS',241,61,15,127,0,tempaddr,false);
  outtextxy('SHIELDS',242,62,15,143,0,tempaddr,false);
  makehole(70);
  for i:=0 to 63 do line(250+i,71,250+i,79,((i div 4)+32),tempaddr);

  outtextxy('WEAPONS',241,81,15,127,0,tempaddr,false);
  outtextxy('WEAPONS',242,82,15,143,0,tempaddr,false);
  makehole(90);
  for i:=0 to 65 do line(249+i,111,249+i,189,0,tempaddr);
  line(249,111,249,189,24,tempaddr);
  line(315,111,315,189,18,tempaddr);
  line(249,111,315,111,24,tempaddr);
  line(249,189,315,189,18,tempaddr);

  outtextxy('  VMW   ',251,114,15,15,0,tempaddr,false);
  outtextxy('F1-HELP  ',251,124,15,15,0,tempaddr,false);
  outtextxy('ESC-QUIT',251,135,15,15,0,tempaddr,false);
  outtextxy('F2-SAVE ',251,145,15,15,0,tempaddr,false);
 end;
  if not(menu) then begin
  flip (vaddr,vga);  { Copy the entire screen at vaddr, our virtual screen }
                     { on which we have done all our graphics, onto the    }
                     { screen you see, VGA }
  flip (vaddr,vaddr2);
  end;
END;

{ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ}
Procedure ScreenTrans (x,y,p1,p2:word);
  { This is a small procedure to copy a 30x30 pixel block from coordinates
    x,y on the virtual screen to coordinates x,y on the true vga screen }
BEGIN
  asm
    push    ds
    push    es
    mov     ax,p1
    mov     es,ax
    mov     ax,p2
    mov     ds,ax
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
    mov     di, bx                  {; es:di = where to go}
    mov     si, di
    mov     al,60
    mov     bx, 30         { Hight of block to copy }
@@1 :
    mov     cx, 24         { Width of block to copy divided by 2 }
    rep     movsw
    add     di,110h        { 320 - 48 = 272 .. or 110 in hex }
    add     si,110h
    dec     bx
    jnz     @@1

    pop     es
    pop     ds
  end;
  { I wrote this procedure late last night, so it may not be in it's
    most optimised state. Sorry :-)}
END;


{ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ}
Procedure newship;
  { This adds a new toaster to the screen }
VAR loop1:integer;
BEGIN
  loop1:=1;


    if not (shipv[1].active) then BEGIN
      shipv[1].x:=36;
      shipv[1].y:=165;
      shipv[1].active:=true;
      shipv[1].frame:=1;
      shipv[1].speed:=5;
      loop1:=10;
    END;

END;



procedure putico(x,y,fra:byte;where:word);
var i,j,col:integer;
begin
  for i:=0 to 47 do
      for j:=0 to 29 do begin
          col:=frame[fra,i,j];
          if col<>0 then putpixel(i+x,y+j,col,where);
      end;

end;
procedure putwave(x,y,fra:byte;where:word);  {fra 0=saucer, 1=shot}
var i,j,col:integer;
begin
  if fra=0 then for i:=0 to 20 do
     for j:=0 to 5 do begin
         col:=saucer[i,j];
         if col<>0 then putpixel((i+x),y+j,col,where);
     end;
  if fra=1 then for i:=0 to 20 do
     for j:=0 to 5 do begin
         col:=shot[i,j];
         if col<>0 then putpixel((i+x),y+j,col,where);
     end;

end;

procedure changescore;
begin
   str(score:8,scorest);
   outtextxy(scorest,250,12,15,12,0,vaddr,true);
end;
{ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ}
Procedure shiproutine;
VAR loop1,loop2:integer;
    ch,ch2:char;
    shieldcol:integer;
    shields:integer;
    saucersout:integer;
    wave1:array[0..5] of boolean;
    bulletx,bullety,oldwavex,oldwavey,wavex,wavey,i,waveadd:integer;
    bulletout:boolean;
    whichone:integer;
BEGIN
  score:=0;
  pal(254,0,0,0);
  shieldcol:=0;
  shipv[1].active:=false;
  newship;
  ch:=#1; ch2:=#1;
  wavex:=0; wavey:=0; waveadd:=5;
  oldwavex:=0; oldwavey:=0;
  bulletout:=false;  bulletx:=0; bullety:=0;
  for i:=0 to 4 do wave1[i]:=true;
  saucersout:=5;
  shields:=10;
  pal(254,shields*6,0,0);
  Repeat
    ch2:=#1;
    if (bulletout) and (bulletx>wavex) and (bulletx<wavex+100) and
       (bullety>wavey) and (bullety<wavey+10) then begin
           whichone:=((bulletx-wavex) div 20);
           if wave1[whichone] then begin
              bulletout:=false;
              inc(score,10);
              changescore;
              wave1[whichone]:=false;
              dec(saucersout,1);
           end;
    end;
    if bulletout then begin
       screentrans(bulletx,bullety,vaddr,vaddr2);
       dec(bullety,5);
       if bullety<5 then bulletout:=false;
       if bulletout then putwave(bulletx,bullety,1,vaddr);
    end;
    for i:=0 to 4 do
        screentrans((oldwavex+20*i),oldwavey,vaddr,vaddr2);
    for i:=0 to 4 do
        if wave1[i] then putwave(wavex+20*i,wavey,0,vaddr);
    oldwavex:=wavex; oldwavey:=wavey;
    wavex:=wavex+waveadd;
    if (wavex>100) or (wavex<5) then begin
       inc(wavey,5);
       waveadd:=-waveadd;
    end;
    if saucersout=0 then begin
       wavey:=0;
       for i:=0 to 5 do if wave1[i]=false then wave1[i]:=true;
       saucersout:=5;
    end;
    if wavey>150 then begin
       wavey:=0;
       for i:=0 to 5 do if wave1[i]=false then wave1[i]:=true;
       saucersout:=5;
       for i:=0 to saucersout do dec(shields);
       if shields>0 then pal(254,shields*6,0,0);
    end;
    if keypressed then BEGIN
      ch:=readkey;
      if ch=chr(0) then ch2:=readkey;
      if ch2='M' then inc(shipv[1].x,5);
      if ch2='K' then dec(shipv[1].x,5);
      if ch=' ' then begin
         bulletout:=true;
         bulletx:=shipv[1].x+10;
         bullety:=shipv[1].y;
         putwave(bulletx,bullety,1,vaddr);
      end;
      
    end;  
    if shipv[1].active then BEGIN
        screentrans (shipv[1].x,shipv[1].y,vaddr,vaddr2);
          { Restore the backgrond the toaster was over }
        
          { Move the toaster }
        if (shipv[1].x<1) then shipv[1].x:=1;
        if (shipv[1].x>255) then shipv[1].x:=255;
              { When toaster reaches the edge of the screen, render it inactive
            and bring a new one into existance. }
    END;
    if shipv[1].active then BEGIN
        CASE shipv [1].frame of
           1   : putico (shipv[1].x,shipv[1].y,0,vaddr);
           3   : putico (shipv[1].x,shipv[1].y,1,vaddr);
           2,4 : putico (shipv[1].x,shipv[1].y,2,vaddr);
        END;
        shipv[1].frame:=shipv[1].frame+1;
        if shipv[1].frame=5 then shipv[1].frame:=1;
          { Draw all the toasters on the VGA screen }
    end;
    waitretrace;
    flip (vaddr,vga);
  until (ch=#27) or (shields<0);
END;

function grinput(xv,yv,howlong,forecol,backcol:integer):string;
var tempx,tempy:integer;
    tempst:string;
    ch3:char;
    label bob;
begin
  tempst:='';
  ch3:='Û';
  tempx:=xv; tempy:=yv;
  repeat
  bob:
    repeat until keypressed;
    ch3:=readkey;
    if (ch3=#8) and (length(tempst)>0) then begin
       dec(tempx,10);
       outtextxy(ch3,tempx,tempy,15,backcol,backcol,vga,true);
       tempst:=tempst[length(tempst)-1];
    end;
    if ch3=#8 then goto bob;
    if length(tempst)>howlong then goto bob;
    outtextxy(ch3,tempx,tempy,15,forecol,backcol,vga,false);
    inc(tempx,10);
    if ch3<>#13 then tempst:=concat(tempst,ch3);
  until ch3=#13;
  grinput:=tempst;
end;


procedure playthegame(lev:integer);
{ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ}
var palloop,paldir:integer;
begin
  cls(0,vaddr);
  grapherror:=loadpicsuperpacked(0,0,vga,'tbma1.tb1');
  outtextxy('MOONBASE ALPHA: EARTH''S LAST CHANCE!',5,5,15,1,0,vga,false);
  pal(255,0,0,0);
  palloop:=0;
  paldir:=1;
  hiscore:=showhiscore(false,false);
  repeat
    pal(255,palloop,0,0);
    if paldir=1 then inc(palloop)
       else dec(palloop);
    if palloop>62 then paldir:=-1;
    if palloop<1 then paldir:=1;
    delay(50);

  until keypressed;
  ch:=readkey;

  drawstars(false);
  shiproutine;
  hiscore:=showhiscore(false,false);
  if score>lowscore then begin
     coolbox(10,75,310,125,true,vga);
     outtextxy('NEW HIGH SCORE!',90,80,15,12,7,vga,false);
     outtextxy('ENTER YOUR NAME (10 Chars)',30,90,15,0,7,vga,false);
     hiname:=grinput(110,110,10,11,7);
     hiscore:=showhiscore(false,true);
  end;
  hiscore:=showhiscore(true,false);
  fade;
end;

procedure dographics;

var i,j:integer;
begin
   grapherror:=loadpicsuperpacked(0,0,vaddr,'ships.tb1');
   for i:=0 to 47 do
       for j:=0 to 29 do
           frame[0,i,j]:=getpixel(i,j,vaddr);
   for i:=0 to 47 do
       for j:=0 to 29 do
           frame[1,i,j]:=getpixel(i,j+32,vaddr);
   for i:=0 to 47 do
       for j:=0 to 29 do
           frame[2,i,j]:=getpixel(i,j+64,vaddr);
   grapherror:=loadpicsuperpacked(0,0,vaddr,'tbsauc.tb1');
   for i:=0 to 20 do
       for j:=0 to 5 do
           saucer[i,j]:=getpixel(i,j,vaddr);
   for i:=0 to 20 do
       for j:=12 to 17 do
           shot[i,(j-12)]:=getpixel(i,j,vaddr);

end;


procedure background;
begin
  cls(0,vaddr);
  drawstars(true);
  coolbox(0,0,319,199,false,vaddr);
end;

procedure options;
var opbarpos,argh:integer;
begin
  background;
  flip(vaddr,vga);
  outtextxy('ESC QUITS',120,175,15,32,0,vga,true);
  opbarpos:=0;
    repeat
    if musicon then begin
      if opbarpos=0 then outtextxy('MUSIC ON ',30,30,15,32,7,vga,true)
         else outtextxy('MUSIC ON ',30,30,15,32,0,vga,true);
    end;
    if not(musicon) then begin
      if opbarpos=0 then outtextxy('NO MUSIC',30,30,15,32,7,vga,true)
         else outtextxy('NO MUSIC',30,30,15,32,0,vga,true);
    end;
      if opbarpos=1 then outtextxy('VIEW HIGH SCORES',30,40,15,32,7,vga,true)
         else outtextxy('VIEW HIGH SCORES',30,40,15,32,0,vga,true);
      ch:=menuread;
       if (ord(ch)=222) or (ord(ch)=220) then inc(opbarpos);
       if (ord(ch)=223) or (ord(ch)=221) then dec(opbarpos);
       if ch='M' then opbarpos:=0;
       if ch='V' then opbarpos:=1;
       if (ch=#13) and (opbarpos=0) then musicon:=not(musicon);
       if (ch=#13) and (opbarpos=1) then begin
          ch:=#27;
          argh:=4;
       end;
       if opbarpos=2 then opbarpos:=0;
       if opbarpos=-1 then opbarpos:=1;
   until ch=#27;
   if argh=4 then hiscore:=(showhiscore(true,false));
end;

procedure loadgame;
begin
  coolbox(90,75,230,125,true,vga);
  outtextxy('LOAD WHICH GAME',97,82,15,9,7,vga,false);
  outtextxy('     (0-9)      ',97,90,15,9,7,vga,false);
  repeat until keypressed;
  level:=0;
end;

procedure help;
begin
  background;
  flip(vaddr,vga);
  outtextxy('HELP',10,10,15,9,0,vga,false);
  outtextxy('Press ESC to exit most stuff.',10,20,15,9,0,vga,false);
  outtextxy('Use the arrows to manuever.',10,30,15,9,0,vga,false);
  repeat until keypressed;
  ch:=readkey;
end;

procedure story;
var error:byte;
    xtemp,ytemp:integer;
    thrustcol:integer;
    thrust:real;
    tempch:char;
procedure putbigflame(xp,yp,frame:integer);
var xtemp,ytemp:integer;
begin
  for xtemp:=0 to 26 do
      for ytemp:=0 to 18 do
          putpixel(xtemp+xp,ytemp+yp,bigflame[frame,xtemp,ytemp],vaddr);
end;
procedure putsmallflame(xp,yp,frame:integer);
var xtemp,ytemp:integer;
begin
  for xtemp:=0 to 3 do
      for ytemp:=0 to 4 do
          putpixel(xtemp+xp,ytemp+yp,smallflame[frame,xtemp,ytemp],vaddr);
end;

procedure putbarge(xp,yp:integer;where:word);
var xtemp,ytemp:integer;
    col:byte;
begin
  for xtemp:=0 to 15 do
      for ytemp:=0 to 18 do begin
          if (ytemp+yp>=0) then begin
             col:=barge[xtemp,ytemp];
             if col<>0 then putpixel(xtemp+xp,ytemp+yp,col,where);
          end;
      end;
end;
procedure puttruck(xp,yp,frame:integer;where:word);
var xtemp,ytemp:integer;
    col:byte;
begin
  for xtemp:=0 to 5 do
      for ytemp:=0 to 8 do begin
          col:=truck[frame,xtemp,ytemp];
          if (ytemp+yp>=173) then begin
          if col<>0 then putpixel(xtemp+xp,ytemp+yp,col,where);
      end;
  end;
end;

begin
  fade;
  error:=loadpicsuperpacked(0,0,vaddr,'tbsobj.tb1');
  for xtemp:=0 to 26 do
      for ytemp:=0 to 18 do begin
          bigflame[0,xtemp,ytemp]:=getpixel(xtemp,ytemp+2,vaddr);
          bigflame[1,xtemp,ytemp]:=getpixel(xtemp,ytemp+21,vaddr);
      end;
  for xtemp:=0 to 3 do
      for ytemp:=0 to 4 do begin
          smallflame[0,xtemp,ytemp]:=getpixel(xtemp,ytemp+43,vaddr);
          smallflame[1,xtemp,ytemp]:=getpixel(xtemp,ytemp+47,vaddr);
      end;
  for xtemp:=0 to 15 do
      for ytemp:=0 to 18 do
          barge[xtemp,ytemp]:=getpixel(xtemp+65,ytemp+100,vaddr);
  for xtemp:=0 to 5 do
      for ytemp:=0 to 8 do begin
          truck[0,xtemp,ytemp]:=getpixel(xtemp+85,ytemp+100,vaddr);
          truck[1,xtemp,ytemp]:=getpixel(xtemp+95,ytemp+100,vaddr);
      end;
{******FIRST MESSAGE*******}
  cls(0,vga);
  outtextxy('MOON BASE ALPHA:',5,5,15,9,0,vga,false);
  outtextxy('  THE FIRST PRIVATELY FINANCED SPACE  ',5,15,15,9,0,vga,false);
  outtextxy('  VENTURE.  FOUNDED IN 2004 BY PIONEER',5,25,15,9,0,vga,false);
  outtextxy('  VINCE WEAVER.  IN ORDER TO SUPPORT',5,35,15,9,0,vga,false);
  outtextxy('  ITSELF, IT DISPOSED OF GARBAGE AND',5,45,15,9,0,vga,false);
  outtextxy('  NUCLEAR WASTE FROM EARTH BY LAUNCHING',5,55,15,9,0,vga,false);
  outtextxy('  IT INTO DEEP SPACE WITH BARGES.  ',5,65,15,9,0,vga,false);
  outtextxy('NOW IT IS 2018, AND THE LAST BARGE IS',5,75,15,9,0,vga,false);
  outtextxy('  TO BE LAUNCHED.  NOW ALL WORK THERE',5,85,15,9,0,vga,false);
  outtextxy('  WILL BE CONCENTRATED ON BUILDING A',5,95,15,9,0,vga,false);
  outtextxy('  TERRAN SPACE FLEET.',5,105,15,9,0,vga,false);
  unfade;
  repeat until keypressed; tempch:=readkey;
  pal(250,0,0,0);
  fade;
{******BARGE TAKING OFF**********}
  error:=loadpicsuperpacked(0,0,vaddr2,'tbma1.tb1');
  putbarge(141,157,vaddr2);
  flip(vaddr2,vaddr);
  flip(vaddr,vga);
  unfade;
  for ytemp:=191 downto 165 do begin
      screentrans(145,ytemp,vaddr,vaddr2);
      puttruck(145,ytemp,ytemp mod 2,vaddr);
      vdelay(7);
      flip(vaddr,vga);
  end;
  error:=loadpicsuperpacked(0,0,vaddr2,'tbma1.tb1');
  vdelay(20);
  flip(vaddr2,vaddr);
  putbarge(141,157,vaddr);
  thrustcol:=0;
  ytemp:=157;
  thrust:=0;
  while ytemp>-25 do begin
      thrust:=thrust+0.05;
      if thrustcol<63 then inc(thrustcol);
      screentrans(141,ytemp,vaddr,vaddr2);
      putbarge(141,ytemp,vaddr);
      vdelay(7);
      pal(250,thrustcol,0,0);
      flip(vaddr,vga);
      ytemp:=ytemp-round(thrust);
  end;
  vdelay(100);
  fade;
{******SECOND MESSAGE*******}
  cls(0,vga);
  outtextxy('5 YEARS LATER, 1 LIGHT YEAR DISTANT',5,5,15,9,0,vga,false);
  outtextxy('  CATASTROPHE STRIKES!!  ',5,15,15,9,0,vga,false);
  unfade;
  vdelay(100);
  repeat until keypressed; tempch:=readkey;
  fade;
{******ALIEN DELEGATION*****}
  error:=loadpicsuperpacked(0,0,vaddr2,'tbcrash.tb1');
  flip(vaddr2,vaddr);
  unfade;
  repeat
    putbigflame(213,100,0);
    putsmallflame(105,90,1);
    putsmallflame(151,71,0);
    putsmallflame(218,72,1);
    putbarge(160,180,vaddr);
    flip(vaddr,vga);
    vdelay(5);
    putbigflame(213,100,1);
    putsmallflame(105,90,0);
    putsmallflame(151,71,1);
    putsmallflame(218,72,0);
    flip(vaddr,vga);
    vdelay(5);
  until keypressed;
  ch:=readkey;
{****ALIEN MESSAGE*****}
  fade;
  error:=loadpicsuperpacked(0,0,vga,'tbgorg.tb1');
  unfade;
  outtextxy('GREETINGS EARTHLINGS.',0,162,15,12,0,vga,false);
  outtextxy('I AM GORGONZOLA THE REPULSIVE.',0,171,15,12,0,vga,false);
  outtextxy('YOU HAVE MADE A BIG MISTAKE.',0,180,15,12,0,vga,false);
  readln;
end;

procedure credits;
var j:integer;
    sp:integer;
  procedure rotate(stri:string;col:integer);
  var j1:integer;
  begin
  if sp=1 then begin
    for j1:=0 to 7 do begin
        outtextxy(stri,0,198,j1,col,0,vga,false);
        Move (mem[vga:320],mem[vga:0],63680);
        if keypressed then begin inc(sp); cls(0,vga); end;
    end;
  end;
  if sp<>1 then begin
     outtextxy(stri,0,180,15,col,0,vga,true);
     Move (mem[vga:3200],mem[vga:0],60800);
  end;

  end;
  procedure skip; begin rotate(' ',0); end;
begin
  sp:=1;
  cls(0,vaddr);
  flip(vaddr,vga);
  j:=0;
  if keypressed then ch:=readkey;
      rotate('               TOM BOMBEM',4);
      rotate('    INVASION OF THE INANIMATE OBJECTS',4);
      skip;       rotate('              PROGRAMMING',9);
      skip;       rotate('            VINCENT M WEAVER',9);
      skip; skip; rotate('                GRAPHICS',10);
      skip;       rotate('            VINCENT M WEAVER',10);
      skip; skip; rotate('                 SOUND',11);
      skip;       rotate('            VINCENT M WEAVER',11);
      skip; skip; rotate('          GRAPHICS INSPIRATION',12);
      skip;       rotate('              JEFF WARWICK',12);
      skip; skip; rotate('         UTOPIA BBS 410-557-0868',13);
      skip;       rotate('              JOHN CLEMENS',13);
      skip;       rotate('              JASON GRIMM',13);
      skip; skip; rotate('           PCGPE AUTHORS, esp',14);
      skip;       rotate('              GRANT SMITH',14);
      skip; skip; rotate('           SOUND BLASTER INFO',15);
      skip;       rotate('               AXEL STOLZ',15);
      skip; skip; rotate('               INSPIRATION',9);
      skip;       rotate('              DOUGLAS ADAMS',9);
      skip;       rotate('             GENE RODENBERRY',9);
      skip;       rotate('               CLIFF STOLL',9);
      skip;       rotate('             ARTHUR C CLARKE',9);
      skip;       rotate('               ISAAC ASIMOV',9);
      skip;       rotate('              GORDON KORMAN',9);
      skip; skip; rotate('         THANKS TO ALL THE AGENTS',10);
      skip;       rotate('          B,D,JL,L,N,P,S,W,PM,E',10);
      skip;       rotate('          AND ESPECIALLY AGENT G',10);
      i:=0;
      repeat
          move(mem[vaddr2:(i*320)],mem[vga:63680],320);
          Move (mem[vga:320],mem[vga:0],63680);
          inc(i);
      until (keypressed) or (i=299);
      if keypressed then ch:=readkey;
  end;

procedure shadowrite(st:string;x5,y5,forecol,backcol:integer);
begin
   outtextxy(st,x5+1,y5+1,15,backcol,0,vga,false);
   outtextxy(st,x5,y5,15,forecol,0,vga,false);
end;

procedure register;

var pagenum,oldpagenum,numpages:integer;
    pagest:string;
    numst:string[2];

procedure page1;
begin
  flip(vaddr,vga);
  shadowrite('              TO REGISTER',10,10,9,1);
  shadowrite('THIS GAME WAS WRITTEN BY A 16 YEAR OLD',10,30,9,1);
  shadowrite('  ENTIRELY IN HIS FREE TIME.',10,40,9,1);
  shadowrite('HOPEFULLY YOU FEEL HIS FREE TIME IS',10,50,9,1);
  shadowrite('  WORTH SOMETHING.',10,60,9,1);
  shadowrite('WARNING:',10,80,12,4);
  shadowrite('  VMW SOFTWARE IS NOT AN INCORPORATED',10,90,12,4);
  shadowrite('  COMPANY, NOR DOES IT HAVE ANY INCOME',10,100,12,4);
  shadowrite('  EXCEPT DONATIONS.  NONE OF ITS',10,110,12,4);
  shadowrite('  SYMBOLS ARE TRADEMARKED EITHER. (BUT',10,120,12,4);
  shadowrite('  I DOUBT YOU''LL NAME A COMPANY AFTER',10,130,12,4);
  shadowrite('  MY ININTIALS)',10,140,12,4);
end;
procedure page2;
begin
  flip(vaddr,vga);
  shadowrite('PLEASE SEND ANY DONATIONS TO:',10,10,10,2);
  shadowrite('     VINCENT WEAVER',10,20,10,2);
  shadowrite('     326 FOSTER KNOLL DR.',10,30,10,2);
  shadowrite('     JOPPA, MD 21085-4706, USA, ETC.',10,40,10,2);
  shadowrite('ANY DONATION OF $5 OR MORE GETS THE',10,60,13,5);
  shadowrite('  NEWEST VERSION OF THE GAME, PLUS',10,70,13,5);
  shadowrite('  ANY OTHER COOL PROGRAMS I HAVE AT',10,80,13,5);
  shadowrite('  THE TIME.',10,90,13,5);
  shadowrite('ALSO IF YOU SEND ME A SELF ADDRESSED',10,110,11,3);
  shadowrite('  STAMPED ENVELOPE WITH SUFFICIENT',10,120,11,3);
  shadowrite('  POSTAGE AND A 3 1/2 INCH DISK IN',10,130,11,3);
  shadowrite('  IT, I WILL COPY THE NEWEST VERSION',10,140,11,3);
  shadowrite('  OF THE GAME ONTO IT.',10,150,11,3);
end;

procedure page3;
begin
  flip(vaddr,vga);
  shadowrite('OTHER VMW SOFTWARE PRODUCTIONS:',10,10,15,7);
  shadowrite('  PAINTPRO:',10,30,13,5);
  shadowrite('    LOAD AND SAVE GRAPHICS PICTURES',10,40,13,5);
  shadowrite('    INTO C, PASCAL, BASIC, ETC.',10,50,13,5);
  shadowrite('    WITH SCREEN CAPTURE UTILITY.',10,60,13,5);
  shadowrite('  SPACEWAR III:',10,70,11,3);
  shadowrite('    ALMOST COMPLETE GAME WITH WORKING',10,80,11,3);
  shadowrite('    SPACESHIPS.  SORT OF COOL.',10,90,11,3);
  shadowrite('  AITAS: (ADVENTURES IN TIME AND SPACE)',10,100,12,4);
  shadowrite('    THIS GAME WILL BE FINISHED SOMEDAY.',10,110,12,4);
  shadowrite('    IT HAS BEEN UNDER WAY FOR 3 YEARS.',10,120,12,4);
  shadowrite('  MISC PASCAL/BASIC PROGRAMS:',10,130,9,1);
  shadowrite('    OVER 500 PROGRAMS WRITTEN OR TYPED',10,140,9,1);
  shadowrite('    IN BY ME....FUN TO LOOK AT.',10,150,9,1);
end;

procedure page4;
begin
  flip(vaddr,vga);
  shadowrite('DISCLAIMERS:',10,10,12,14);
  shadowrite('** THE ABOVE PROGRAMS HAVE NEVER DONE**',5,30,12,4);
  shadowrite('** ANYTHING BAD TO MY COMPUTER THAT  **',5,40,12,4);
  shadowrite('** CTRL-ALT-DEL WOULDN''T FIX.  I AM  **',5,50,12,4);
  shadowrite('** NOT RESPONSIBLE FOR HARD DISK     **',5,60,12,4);
  shadowrite('** DISSAPPEARANCES, MISSING MODEMS   **',5,70,12,4);
  shadowrite('** MOUSE BREAKDOWNS, MELTING MONITORS**',5,80,12,4);
  SHADOWRITE('** OR ANYTHING ELSE.                 **',5,90,12,4);
  shadowrite('%% ALL VMW SOFTWARE PRODUCTIONS ARE  %%',5,110,11,3);
  shadowrite('%% CERTIFIED VIRUS FREE!!!!!!!!!!!!  %%',5,120,11,3);
end;

begin
 background;
 pagenum:=1;
 oldpagenum:=1;
 numpages:=4;
 page1;
 shadowrite('PAGE  1 of  4: ESC QUITS',50,180,15,7);
 repeat
 ch:=menuread;
     if (ch=' ') or (ch=#13) then inc(pagenum);
     if (ch='õ') or (ch='Ş') or (ch='Ü') then inc(pagenum);
     if (ch='ô') or (ch='İ') or (ch='ß') then dec(pagenum);
     if pagenum>4 then pagenum:=1;
     if pagenum<1 then pagenum:=4;
     if oldpagenum<>pagenum then begin
        if pagenum=1 then page1;
        if pagenum=2 then page2;
        if pagenum=3 then page3;
        if pagenum=4 then page4;
        str(pagenum:2,numst);
        pagest:=concat('PAGE ',numst);
        str(numpages:2,numst);
        pagest:=concat(pagest,' of ',numst,': ESC QUITS');
        shadowrite(pagest,50,180,15,7);
        oldpagenum:=pagenum;
     end;
  until ch=#27;
end;


label picloader,menu;

begin
   axel_und_carmen:=true; {as_of_9-22-94} {change_back_10-6-94}
   musicon:=true;
   randomize;
   setupvirtual;
   fade;
   setmcga;
   dographics;
   energy:=15;
   lives:=2;
   score:=0;
   level:=0;
   for x:=0 to 40 do begin
       pal(100+x,x+20,0,0);
       pal(141+x,0,0,x+20);
       pal(182+x,0,x+20,0);
   end;
   fade;
   modinit;
     dev:=7;
     md:='vmwfan.tb1';
     mix := 10000;   {use 10000 normally }
     pro := 0; {Leave at 0}
     loop :=0; {4 means mod will play forever}
     modvolume (255,255,255,255);    { Full volume }
     modsetup ( stat, dev, mix, pro, loop, md );
     case stat of
           1: writeln('Not a mod');
           2: writeln('Already playing');
           4: writeln('Out of memory');
     end;
     for x:=0 to 40 do begin
       line(x+40,45,x+40,45+(2*x),100+x,vga);
       line(x+120,45,x+120,45+(2*x),141+x,vga);
       line(x+200,45,x+200,45+(2*x),141+x,vga);
       line(x+80,125,x+80,125-(2*x),182+x,vga);
       line(x+160,125,x+160,125-(2*x),182+x,vga);
   end;
   for x:=40 downto 0 do begin
       line(x+80,45,x+80,125-(2*x),140-x,vga);
       line(x+160,45,x+160,125-(2*x),181-x,vga);
       line(x+240,45,x+240,125-(2*x),181-x,vga);
       line(x+120,125,x+120,45+(2*x),222-x,vga);
       line(x+200,125,x+200,45+(2*x),222-x,vga);
   end;
   unfade;

   outtextxy('A VMW SOFTWARE PRODUCTION',60,140,15,15,15,VGA,false);
   y:=0;
   hiscore:=0;
   repeat until keypressed;
   ch:=readkey;
   modstop;
   fade;
   cls(0,vga);
   assign(palf,'pal.tb1');
   reset(palf);
   for i:=0 to 255 do begin
       for j:=1 to 3 do readln(palf,temp[j]);
       pal(i,temp[1],temp[2],temp[3]);
   end;
   close(palf);
   fade;

PICLOADER:
   grapherror:=loadpicsuperpacked(0,0,vaddr2,'tbomb1.tb1');
   if not(axel_und_carmen) then begin
      for tempi:=193 to 199 do
          for tempj:=290 to 319 do
          putpixel(tempj,tempi,0,vaddr2);
   end;
MENU:
   if musicon then begin
   modinit;
     dev:=7;
     md:='weave1.tb1';
     mix := 10000;   {use 10000 normally }
     pro := 0; {Leave at 0}
     loop :=0; {4 means mod will play forever}
     modvolume (255,255,255,255);    { Full volume }
     modsetup ( stat, dev, mix, pro, loop, md );
     case stat of
           1: writeln('Not a mod');
           2: writeln('Already playing');
           4: writeln('Out of memory');
     end;
   end;
   flip(vaddr2,vga);
   unfade;
   repeat until keypressed;
   ch:=readkey;
   barpos:=0;
   outtextxy('F1 HELP',0,190,15,9,7,vga,false);
   coolbox(117,61,199,140,true,vga);
   repeat
      if barpos=0 then outtextxy('NEW GAME',123,67,15,32,0,vga,true)
         else outtextxy('NEW GAME',123,67,15,32,7,vga,true);
      if barpos=1 then outtextxy('OPTIONS',123,77,15,32,0,vga,true)
         else outtextxy('OPTIONS',123,77,15,32,7,vga,true);
      if barpos=2 then outtextxy('REGISTER',123,87,15,32,0,vga,true)
         else outtextxy('REGISTER',123,87,15,32,7,vga,true);
      if barpos=3 then outtextxy('LOAD GAME',123,97,15,32,0,vga,true)
         else outtextxy('LOAD GAME',123,97,15,32,7,vga,true);
      if barpos=4 then outtextxy('STORY',123,107,15,32,0,vga,true)
         else outtextxy('STORY',123,107,15,32,7,vga,true);
      if barpos=5 then outtextxy('CREDITS',123,117,15,32,0,vga,true)
         else outtextxy('CREDITS',123,117,15,32,7,vga,true);
      if barpos=6 then outtextxy('QUIT',123,127,15,32,0,vga,true)
         else outtextxy('QUIT',123,127,15,32,7,vga,true);

     ch:=menuread;
       if (ord(ch)=222) or (ord(ch)=220) then inc(barpos);
       if (ord(ch)=223) or (ord(ch)=221) then dec(barpos);
       if (ord(ch)=168) then begin barpos:=10; ch:=#13; end;
       if ch='N' then barpos:=0;
       if ch='O' then barpos:=1;
       if ch='R' then barpos:=2;
       if ch='L' then barpos:=3;
       if ch='S' then barpos:=4;
       if ch='C' then barpos:=5;
       if ch='Q' then barpos:=6;
       if ch='A' then axel_und_carmen:=not(axel_und_carmen);
       if ch=#27 then begin
          barpos:=6;
          ch:=#13;
       end;
       if barpos=7 then barpos:=0;
       if barpos=-1 then barpos:=6;
   until ch=#13;
   modstop;
   if barpos=6 then quit;
   if barpos=1 then options;
   if barpos=2 then register;
   if barpos=3 then loadgame;
   if barpos=4 then story;
   if barpos=5 then credits;
   if barpos=10 then help;
   if barpos=0 then playthegame(0);
   if barpos=0 then goto picloader;
   if barpos=4 then goto picloader;
   goto menu;
   
end.