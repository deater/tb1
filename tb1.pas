program TOM_BOMB_EM_AND_INVASION_OF_INANIMATE_OBJECTS;
{version 2.4a}
        {by Vincent Weaver....21085-4706}

uses svmwgraph,nvmwgraph,crt,detect,smix,dos;

{$G+}
{$I tb1ans.pas}

  const
    XMSRequired   = 90;    {XMS memory required to load the sounds (KBytes) }
    SharedEMB     = true;
      {TRUE:   All sounds will be stored in a shared EMB}
      {FALSE:  Each sound will be stored in a separate EMB}
    NumSounds = 8;

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






var {Brodsky's Variables}
    BaseIO: word; IRQ, DMA, DMA16: byte;
    Sound: array[0..NumSounds-1] of PSound;
    OldExitProc: pointer;
    Xsoundeffects,Xsbsound:byte;
    sbsound,sbeffects:boolean;
    XBASEIO,XDMA,XIRQ:word;

type ScreenType = array [0..3999] of byte;  {For Ansi Loading}


     EnemyInfo = record
               x,y:integer;
               kind:byte;
               out,exploding,boundarycheck,dead:boolean;
               explodprogress:byte;
               minx,maxx,xspeed,yspeed:integer;
               hitsneeded:integer;
             end;
     Obstruction = record
                   x,y:integer;
                   shooting,dead,exploding:boolean;
                   explodeprogress:byte;
                   howmanyhits:integer;
                   kind,lastshot:byte;
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
    testr,string_param,md : string;
    howmuchscroll:byte;
    filestuff:searchrec;  {for load game}
    savegamef:text;
    ondisk:string;
    tempch:char;
    CDROMmode:boolean;

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
    shipxadd,shipyadd,shipadd,shipx,shipy,shipspeed,shipframe:integer;


var grapherror:byte;
    temp:array[1..3] of byte;
    i,j:byte;
    x,y,barpos:integer;
    screen:screentype absolute $B800:0000;
    ch:char;


procedure ClearKeyboardBuffer;
begin
  mem[$0040:$001c]:=mem[$0040:$001a];
end;


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
  if not(CDROMmode) then begin
  {$I-}
     assign(ff,'config.tb1');
     reset(ff);
     close(ff);
  {$I+}
     if IOresult<>0 then begin
        textcolor(12);
        writeln('Warning: Missing or Invalid "config.tb1" file');
        writeln;
        textcolor(3);
        writeln('If you are running this off of a CD-ROM drive:');
        textcolor(7);
        writeln('    For optimal performance you should copy this game');
        writeln('    to a floppy or hard disk.  However you can play in');
        writeln('    read only mode.');
        writeln;
        textcolor(3);
        writeln('If you are not running this off of a CD-ROM drive:');
        textcolor(7);
        write('    Please run the "');
        textcolor(2);
        write('tbsetup');
        textcolor(7);
        writeln('" utility included to set');
        writeln('    up your computer.  Thank You.');
        writeln;
        textcolor(3);
        writeln('It is possible to play this game in read only mode.  There will');
        writeln('be no high scores or saved games, but it is playable.');
        writeln;
        textcolor(12);
        writeln('Press ''R'' to play in read-only mode.');
        clearkeyboardbuffer;
        repeat until keypressed;
        ch:=readkey;
        textcolor(7);
        if upcase(ch)='R' then CDROMmode:=true
           else halt(5);
     end;
  end;
     if not(CDROMmode) then begin
        assign(ff,'config.tb1');
        reset(ff);
          readln(ff,Xsoundeffects);
          readln(ff,Xsbsound);
          readln(ff,Xbaseio);
          readln(ff,Xirq);
          readln(ff,Xdma);
        close(ff);
     end
     else begin
          writeln;
          writeln;
          textcolor(2);
          writeln('Do you want soundblaster effects? (y/n)');
          writeln;
          textcolor(7);
          writeln('  (If you encounter problems, be sure the SET BLASTER');
          writeln('   environmental variable is set.  See your soundcard''s manual).');
          clearkeyboardbuffer;
          repeat until keypressed;
          ch:=readkey;  if upcase(ch)='Y' then begin
             xsbsound:=1; Xsoundeffects:=1; soundeffects:=true;
          end
          else begin xsbsound:=0; Xsoundeffects:=0; soundeffects:=false;
          end;
     end;
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
     if not(CDROMmode) then begin
        assign(hilist,'HISCORE.TB1');
        rewrite(hilist);
          for i:=0 to 9 do writeln(hilist,names[i]);
          for i:=0 to 9 do writeln(hilist,scores[i]);
        close(hilist);
     end;
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
  verticalline(y,y+9,249,24,tempaddr);
  verticalline(y+1,y+10,314,18,tempaddr);
end;

BEGIN
  tempaddr:=vaddr;
  fillblock(240,0,320,199,19,tempaddr);
   verticalline(0,199,240,18,tempaddr);
   horizontalline(240,319,0,18,tempaddr);
   verticalline(0,199,399,24,tempaddr);
   horizontalline(241,319,199,24,tempaddr);
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
  for i:=0 to ((4*shields)-1) do
      verticalline(71,78,250+i,(47-(i div 4)),tempaddr);

  outtextxy('WEAPONS',241,81,127,0,tempaddr,false);
  outtextxy('WEAPONS',242,82,143,0,tempaddr,false);
  makehole(90);

  fillblock(249,111,314,189,0,tempaddr);

  verticalline(111,189,249,24,tempaddr);
  verticalline(111,189,315,18,tempaddr);
  horizontalline(249,315,111,24,tempaddr);
  horizontalline(249,315,189,18,tempaddr);

  outtextxy('  TB1   ',250,114,2,0,tempaddr,false);
  outtextxy('  TB1   ',251,115,10,0,tempaddr,false);
  outtextxy('F1-HELP  ',250,124,2,0,tempaddr,false);
  outtextxy('F1-HELP  ',251,125,10,0,tempaddr,false);
  outtextxy('F2-SAVES',250,134,2,0,tempaddr,false);
  outtextxy('F2-SAVES',251,135,10,0,tempaddr,false);
  outtextxy('ESC-QUIT',250,144,2,0,tempaddr,false);
  outtextxy('ESC-QUIT',251,145,10,0,tempaddr,false);
  outtextxy('P-PAUSES',250,154,2,0,tempaddr,false);
  outtextxy('P-PAUSES',251,155,10,0,tempaddr,false);
  outtextxy('S-SOUND ',250,164,2,0,tempaddr,false);
  outtextxy('S-SOUND ',251,165,10,0,tempaddr,false);


  flipd320(vaddr,vaddr2);
END;




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
      if sbeffects then startsound(sound[2],0,false);
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


{000+}

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
   if CDROMmode then exit;
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
   grapherror:=mode13loadpicpacked(0,0,vaddr,false,true,'tblev3.tb1');
   
  for j:=0 to 3 do
    for i:=0 to 19 do
      for x:=0 to 9 do
          for y:=0 to 9 do
              ShapeTable1^[shape3array[(j*20)+i]+((y*10)+x)]
                          :=getpixel(1+x+(i*11),1+y+(j*11),vaddr);
  cls(0,vaddr);
end;


procedure littleopener3;
var star_x:array[0..5]of integer;
    star_y:array[0..5]of integer;
begin
  loadlevel3shapes;
  grapherror:=mode13loadpicpacked(0,0,vga,true,false,'tbl2ship.tb1');
  fade;
  cls(0,vga);
  grapherror:=mode13loadpicpacked(0,0,vaddr,false,true,'tbl3intr.tb1');
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

  flipd320(vaddr2,vaddr);
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
    
    flipd240(0,vaddr,vaddr2);

{***Collision Check***}
   if numpits>0 then
    for i:=0 to (numpits-1) do with pits[i] do
        if collision(shipx,shipy,5,4,px,py,4,4)
            then begin
                 levelover:=true;
                 cls(0,vga);
                 grapherror:=mode13loadpicpacked(0,0,vga,false,true,'tbpit.tb1');
                 if sbeffects then StartSound(Sound[7], 0, false);

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
               grapherror:=mode13loadpicpacked(0,0,vga,false,true,'tbconsol.tb1');
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
     if (ch='S') or (ch='s') then sbeffects:=not(sbeffects);

      if ch2='<' then savegame;

      end;
      if (ch=' ') and havegun then begin
         if (bullet1out=false) then begin
            if sbeffects then StartSound(Sound[4], 0, false);
            bullet1out:=true;
            bullet1x:=shipx+3;
            bullet1y:=shipy+4;
            bullet1dir:=shipframe;
            putshape(shape3array[76],vaddr,10,9,bullet1x,bullet1y);
         end
         else
         if (bullet2out=false) then begin
            if sbeffects then StartSound(Sound[4], 0, false);
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
    flipd320 (vaddr,vga);
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
   grapherror:=mode13loadpicpacked(0,0,vaddr,false,true,'ships.tb1');

   for i:=0 to 42 do
       for j:=0 to 30 do begin
           ShapeTable1^[bigship1off+((j*43)+i)]:=getpixel(i,j,vaddr);
           ShapeTable1^[bigship2off+((j*43)+i)]:=getpixel(i,j+32,vaddr);
           ShapeTable1^[bigship3off+((j*43)+i)]:=getpixel(i,j+64,vaddr);
       end;


 if l_level=2 then
    grapherror:=mode13loadpicpacked(0,0,vaddr,false,true,'tbaship.tb1');
 if l_level=4 then
    grapherror:=mode13loadpicpacked(0,0,vaddr,false,true,'tbeerm.tb1');
 for j:=0 to 3 do
  for i:=0 to 9 do
      for x:=0 to 19 do
          for y:=0 to 9 do
              ShapeTable1^[shape2array[(j*10)+i]+((y*20)+x)]
                          :=getpixel(1+x+(i*21),1+y+(j*11),vaddr);
  cls(0,vaddr);
end;



BEGIN
  level:=l_level;
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
                 if sbeffects then StartSound(Sound[3], 0, false);
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
                 if sbeffects then StartSound(Sound[3], 0, false);
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
           if sbeffects then startsound(Sound[5],0,false);
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
              if sbeffects then startsound(Sound[5],0,false);
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
     if (ch='S') or (ch='s') then sbeffects:=not(sbeffects);

      if ch2='<' then savegame;

      end;
      if (ch=' ') then begin
         if (bullet1out=false) then begin
            if sbeffects then StartSound(Sound[4], 0, false);
            bullet1out:=true;
            bullet1x:=shipx+21;
            bullet1y:=165;
            putshape(12000,vaddr,20,9,bullet1x,bullet1y);
         end
         else
         if (bullet2out=false) then begin
            if sbeffects then StartSound(Sound[4], 0, false);
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
         outtextxy(testr,20,20,9,7,vga,true);}

  if backrow>=200 then begin
     clearkeyboardbuffer;
     pauseawhile(200);
     fade;
      grapherror:=Mode13LoadPicPacked(0,0,vaddr,false,true,'viewscr.tb1');
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
         level:=3;
         clearkeyboardbuffer;
         blockmove(0,42,58,79,vaddr,10,110,vga);
         outsmalltextxy('Wha? Wait!',70,110,9,0,vga,true);
         outsmalltextxy('What''s happening?',70,120,9,0,vga,true);
         pauseawhile(550);
         fade;
     end;

         grapherror:=Mode13LoadPicPacked(0,0,vaddr,false,true,'tbtract.tb1');
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
     levelover:=true;
     unfade;
    end;
  end;
until levelover;
END;


Procedure levelone;
VAR loop1,loop2:integer;
    ch,ch2:char;
    saucersout:integer;
    bullet1x,bullet1y,bullet2x,bullet2y,i,wave:integer;
    bullet1out,bullet2out:boolean;
    whichone,temp,temp2,whichwave:integer;
    tempst:string;
    what:byte;
    itemp,jtemp:byte;
    whatdelay:byte;
    levelover:boolean;


procedure waveincrease;
begin
  inc(wave);
  dec(saucersout);
  if saucersout<0 then saucersout:=0;
  if saucersout>5 then saucersout:=5;
  if wave<6 then whichwave:=0;
  if (wave>=6) and (wave<=12) then whichwave:=1; {1}
  if (wave>12) and (wave<=20) then whichwave:=2; {2}
  if (wave>20) and (wave<=30) then whichwave:=3; {3}
  if (wave>30) and (wave<=36) then whichwave:=2; {2}
  if (wave>36) and (wave<=46) then whichwave:=3;
  if (wave>46) and (wave<=50) then whichwave:=1;
  if (wave>50) and (wave<=60) then whichwave:=3;
  if (wave>60) and (wave<=76) then whichwave:=2;
  if (wave>76) and (wave<=82) then whichwave:=1;
  if (wave>82) and (wave<=85) then whichwave:=3;
  if (wave>85) and (wave<=96) then whichwave:=2;
  if (wave>96) and (wave<=100) then whichwave:=1;
  if (wave>100) then whichwave:=4;
end;

procedure BeforeBoss;
begin
  fade;
  grapherror:=Mode13LoadPicPacked(0,0,vaddr,false,true,'viewscr.tb1');
  cls(0,vga);
  blockmove(0,5,58,42,vaddr,10,10,vga);
  clearkeyboardbuffer;
  outsmalltextxy('HUMAN!',70,10,2,0,vga,true);
  outsmalltextxy('WHAT ARE YOU DOING?!',70,20,2,0,vga,true);
  outsmalltextxy('YOUR SPECIES MUST BE TERMINATED!',70,30,2,0,vga,true);
  unfade;
  pauseawhile(350);
  clearkeyboardbuffer;
  blockmove(0,42,58,79,vaddr,10,50,vga);
  outsmalltextxy('I''M SORRY.',70,50,9,0,vga,true);
  outsmalltextxy('WE DIDN''T MEAN TO DESTROY YOUR ENVOY.',70,60,9,0,vga,true);
  outsmalltextxy('WILL YOU FORGIVE US AND TRY PEACE?',70,70,9,0,vga,true);
  pauseawhile(400);
  clearkeyboardbuffer;
  blockmove(0,5,58,42,vaddr,10,90,vga);
  outsmalltextxy('NO!  YOU MUST BE DESTROYED!',70,90,2,0,vga,true);
  outsmalltextxy('OUR FUNDING ... OUR ENVOY WAS DAMAGED BY',70,100,2,0,vga,true);
  outsmalltextxy('YOU!  VENGEANCE WILL BE OURS!  YOUR PUNY',70,110,2,0,vga,true);
  outsmalltextxy('PRIMITIVE SPACECRAFT WITH ITS INFERIOR',70,120,2,0,vga,true);
  outsmalltextxy('WEAPONS WOULD HAVE TO SCORE 9 DIRECT HITS',70,130,2,0,vga,true);
  outsmalltextxy('TO DESTROY MY SHIP!  DIE EARTH SCUM!!!!',70,140,2,0,vga,true);
  pauseawhile(800);
  flipd240(howmuchscroll,vaddr,vaddr2);
end;


procedure AfterBoss;
begin
  level:=2;
  fade;
  clearkeyboardbuffer;
  grapherror:=Mode13LoadPicPacked(0,0,vaddr,false,true,'viewscr.tb1');
  cls(0,vga);
  blockmove(0,42,58,79,vaddr,10,10,vga);
  outsmalltextxy('HMM.. THEY DON''T BUILD SUPERIOR',70,10,9,0,vga,true);
  outsmalltextxy('TECHNOLOGY LIKE THEY USED TO.',70,20,9,0,vga,true);
  outsmalltextxy('I GUESS I CAN GO HOME NOW.',70,30,9,0,vga,true);
  unfade;
  pauseawhile(400);
  clearkeyboardbuffer;
  blockmove(0,5,58,42,vaddr,10,50,vga);
  outsmalltextxy('NOT SO FAST!  YOU JUST DESTROYED AN ANTIQUATED',70,50,2,0,vga,true);
  outsmalltextxy('DEFENSE SYSTEM THAT WAS PROGRAMMED BY A 16',70,60,2,0,vga,true);
  outsmalltextxy('YEAR OLD!  OUR MAIN DEFENSE PROGRAMMER HAS ',70,70,2,0,vga,true);
  outsmalltextxy('MUCH MORE SKILL NOW!  UNLESS YOU DESTROY OUR',70,80,2,0,vga,true);
  outsmalltextxy('ENTIRE XENOCIDE... I MEAN PEACE... ENVOY',70,90,2,0,vga,true);
  outsmalltextxy('WE WILL STILL DESTROY YOUR HOME PLANET.',70,100,2,0,vga,true);
  outsmalltextxy('NICE TRY PUNY EARTHLING!',70,110,2,0,vga,true);
  pauseawhile(1200);
  clearkeyboardbuffer;
  blockmove(0,42,58,79,vaddr,10,130,vga);
  outsmalltextxy('HMM.. I GUESS I BETTER SAVE THE EARTH.',70,130,9,0,vga,true);
  outsmalltextxy('I''D BETTER SAVE MY GAME TOO.',70,140,9,0,vga,true);
  outsmalltextxy('D''OH!  I''M OUT OF BIG MISSILES! ',70,150,9,0,vga,true);
  outsmalltextxy('WELL AT LEAST I HAVE SOME SMALLER SPARES.',70,160,9,0,vga,true);
  pauseawhile(500);
  clearkeyboardbuffer;
  fade;
  cls(0,vga);
  flipd240(howmuchscroll,vaddr,vaddr2);
  unfade;


  {outsmalltextxy('YOU''VE BEATEN LEVEL ONE!!!',70,60,12,0,vga,true);
  outsmalltextxy('NO, THIS IS NOT ONE OF THOSE ANNOYING',70,70,12,0,vga,true);
  outsmalltextxy('"REGISTER NOW!"  MESSAGES.  ACTUALLY ',70,80,12,0,vga,true);
  outsmalltextxy('THIS IS AS FAR AS THE GAME IS WRITTEN.',70,90,12,0,vga,true);
  outsmalltextxy('OF COURSE, TO HASTEN ITS COMPLETION YOU',70,110,11,0,vga,true);
  outsmalltextxy('COULD SEND A CONTRIBUTION TO THE AUTHOR.',70,120,11,0,vga,true);
  outsmalltextxy('       <HINT.  HINT.>',70,130,11,0,vga,true);
  outsmalltextxy('IN ANY CASE WATCH SOON FOR THE FURTHER ',70,150,14,0,vga,true);
  outsmalltextxy('ADVENTURES OF TOM BOMBEM!!!',70,160,14,0,vga,true);
  pauseawhile(1800);
  flipd240(howmuchscroll,vaddr,vaddr2);}
end;


BEGIN
  wave:=0;
  whichwave:=0;
  whatdelay:=1;
  shipx:=36;
  shipadd:=0;
  shipframe:=1;
  shipspeed:=5;
  levelover:=false;
  ch:=#1; ch2:=#1;
  for i:=0 to 4 do begin
      enemy[i].exploding:=false;
      enemy[i].out:=false;
      enemy[i].dead:=false;
  end;

  bullet1out:=false;  bullet2out:=false;
  bullet1x:=0; bullet1y:=0;
  bullet2x:=0; bullet2y:=0;
  saucersout:=0;


  flipd320(vaddr2,vaddr);
  str(level,tempst);
  fillblock(251,52,314,59,0,vaddr);
  outtextxy(tempst,307,51,12,0,vaddr,false);

  cls(0,vaddr2);
  for i:=0 to 50 do
    putshape240(shapearray[12],
                    vaddr2,18,17,random(238),random(260));
  for i:=0 to 50 do
    putshape240(shapearray[13],
                    vaddr2,18,17,random(238),random(260));
  {for i:=0 to 10 do putshape240(shapearray[11],
                    vaddr2,18,17,i*20,i*20);}

  pal(254,shields*4,0,0);
  howmuchscroll:=50;

  cls(0,vga);
  coolbox(70,85,240,120,true,vga);
  outtextxy('   LEVEL ONE:',84,95,4,7,vga,false);
  outtextxy('INANIMATE OBJECTS',84,105,4,7,vga,false);

  clearkeyboardbuffer;
  pauseawhile(300);
{**** GAME LOOP ****}
{*******************}

  Repeat
    ch2:=#1;
    ch:=#1;
    dec(howmuchscroll);
    if howmuchscroll<1 then begin
       flipd50(vaddr2,3000,vaddr3,0);
       flipd50(vaddr2,2250,vaddr2,3000);
       flipd50(vaddr2,1500,vaddr2,2250);
       flipd50(vaddr2,750,vaddr2,1500);
       flipd50(vaddr2,0,vaddr2,750);
       flipd50(vaddr3,0,vaddr2,0);
       howmuchscroll:=50;
    end;
    flipd240(howmuchscroll,vaddr,vaddr2);

{***Collsion Check***}
      for i:=0 to 4 do begin
       if (enemy[i].dead=false) then begin
        if (bullet1out) then begin
          if collision(bullet1x,bullet1y,10,10,
                       enemy[i].x,enemy[i].y,9,9) then begin
            if sbeffects then StartSound(Sound[3], 0, false);
            dec(enemy[i].hitsneeded);
            if enemy[i].hitsneeded<1 then enemy[i].dead:=true
               else enemy[i].dead:=false;
            enemy[i].exploding:=true;
            enemy[i].explodprogress:=0;
            bullet1out:=false;
            inc(score,10);
            changescore;
          end;
             end;
             if (bullet2out) then begin
                if collision(bullet2x,bullet2y,10,10,
                        enemy[i].x,enemy[i].y,9,9) then begin
                        if sbeffects then StartSound(Sound[3], 0, false);
                        dec(enemy[i].hitsneeded);
                        if enemy[i].hitsneeded<1 then enemy[i].dead:=true
                           else enemy[i].dead:=false;
                        enemy[i].exploding:=true;
                        enemy[i].explodprogress:=0;
                        bullet2out:=false;
                        inc(score,10);
                        changescore;
                end;
             end;
         end;
       end;

{***DO EXPLOSIONS***}
    for i:=0 to 4 do begin
        if enemy[i].exploding=true then begin
           inc(enemy[i].explodprogress);
           if enemy[i].explodprogress<=5 then
              putshape(shapearray[enemy[i].explodprogress+15],
                    vaddr,18,17,enemy[i].x,enemy[i].y)
              else
                if enemy[i].dead then begin
                   enemy[i].out:=false;
                   enemy[i].exploding:=false;
                   waveincrease;
                end
                else enemy[i].exploding:=false;
        end;
    end;
{***MOVE BULLET***}
    if bullet1out then begin
       dec(bullet1y,5);
       if bullet1y<5 then bullet1out:=false;
       if bullet1out then putshape(shape1off,vaddr,18,17,bullet1x,bullet1y);
    end;
    if bullet2out then begin
       dec(bullet2y,5);
       if bullet2y<5 then bullet2out:=false;
       if bullet2out then putshape(shape1off,vaddr,18,17,bullet2x,bullet2y);
    end;
{***MOVE ENEMIES***}
    for i:=0 to 4 do begin
        if (enemy[i].out=true) and (enemy[i].dead=false) then begin
           putshape(shapearray[enemy[i].kind],vaddr,18,17,enemy[i].x,enemy[i].y);
           enemy[i].x:=enemy[i].x+enemy[i].xspeed;
      {*Check Position*}
      if enemy[i].boundarycheck=false then inc(enemy[i].y,enemy[i].yspeed);

           if (enemy[i].x<=enemy[i].minx) or (enemy[i].x>=enemy[i].maxx) then begin
              enemy[i].xspeed:=-enemy[i].xspeed;
              enemy[i].x:=enemy[i].x+enemy[i].xspeed;
              inc(enemy[i].y,enemy[i].yspeed);
           end;
           {*Too Low*}
              if enemy[i].y>179 then begin
                 enemy[i].out:=false;
                 dec(saucersout);
              end;
              if enemy[i].y>140 then begin
                 if collision(shipx,165,
                              24,15,
                              enemy[i].x,enemy[i].y,
                              9,9)
                 then begin
                    if sbeffects then startsound(Sound[5],0,false);
                    dec(enemy[i].hitsneeded);
                    if enemy[i].hitsneeded=0 then enemy[i].dead:=true
                       else enemy[i].dead:=false;
                    enemy[i].exploding:=true;
                    enemy[i].explodprogress:=0;
                    dec(shields);
                    if shields<0 then levelover:=true;
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
     end;
{***START NEW WAVE***}
   {**STANDARD**}
    if (saucersout=0) and (whichwave=0) then begin
       saucersout:=5;
       what:=random(8)+3;
       for temp:=0 to 4 do begin
           with enemy[temp] do begin
                kind:=what;
                x:=0;
                y:=0;
                xspeed:=5;
                x:=temp*20;
                minx:=(temp*20);
                maxx:=(temp*20)+120;
                boundarycheck:=true;
                yspeed:=10;
                out:=true;
                exploding:=false;
                hitsneeded:=1;
                dead:=false;
           end;
        end;
    end;
   {**FALLING STRAIGHT**}
    if whichwave=3 then begin
       for temp:=0 to 4 do
           if enemy[temp].out=false then begin
              with enemy[temp] do begin
                   kind:=random(8)+3;
                   x:=random(200)+1;
                   y:=0;
                   xspeed:=0;
                   minx:=enemy[temp].x;
                   maxx:=enemy[temp].x;
                   boundarycheck:=true;
                   yspeed:=5+(wave div 40);
                   out:=true;
                   exploding:=false;
                   hitsneeded:=1;
                   dead:=false;
                   inc(saucersout);
              end;
           end;
    end;
   {**FALLING GRADUALLY SIDEWAYS**}

    if whichwave=2 then begin
       for temp:=0 to 4 do
           if enemy[temp].out=false then begin
              with enemy[temp] do begin
                   kind:=random(8)+3;
                   y:=0;
                   xspeed:=5;
                   minx:=random(100);
                   maxx:=random(100)+120;
                   x:=enemy[temp].minx;
                   boundarycheck:=false;
                   yspeed:=1;
                   out:=true;
                   exploding:=false;
                   hitsneeded:=1;
                   dead:=false;
                   inc(saucersout);
              end;
           end;
    end;
  {**ZIG-ZAG**}
    if (whichwave=1) and (saucersout=0) then begin
       saucersout:=5;
       whichone:=random(8)+3;
       for temp:=0 to 4 do
           if enemy[temp].out=false then begin
              with enemy[temp] do begin
                   kind:=whichone;
                   y:=temp*10;
                   xspeed:=5;
                   minx:=0;
                   maxx:=220;
                   x:=temp*20;
                   boundarycheck:=false;
                   yspeed:=1;
                   out:=true;
                   exploding:=false;
                   hitsneeded:=1;
                   dead:=false;
              end;
           end;
    end;

    if (whichwave=4) then begin
       if saucersout=0 then begin
          beforeboss;
          enemy[0].kind:=15;
          enemy[1].kind:=15;
          enemy[2].kind:=14;
          for temp:=0 to 2 do begin
              with enemy[temp] do begin
                    x:=temp*20;
                   y:=0;
                   xspeed:=5;
                   minx:=0;
                   maxx:=220;
                   boundarycheck:=true;
                   yspeed:=0;
                   out:=true;
                   exploding:=false;
                   hitsneeded:=3;
                   dead:=false;
                   inc(Saucersout);
              end;
          end;
       end;

       if enemy[1].kind=15 then
          for temp:=3 to 4 do begin
              inc(saucersout);
              if (enemy[temp].out=false)
                 and (enemy[temp-3].out=true) then begin
                      with enemy[temp] do begin
                           kind:=random(8)+3;
                           x:=enemy[temp-3].x;
                           y:=20;
                           xspeed:=0;
                           minx:=x;
                           maxx:=x;
                           boundarycheck:=false;
                           yspeed:=5;
                           out:=true;
                           exploding:=false;
                           hitsneeded:=1;
                           dead:=false;
                      end;
           end;
       end;
    end;

    if (whichwave=4) and (enemy[0].dead=true) and
       (enemy[1].dead=true) and (enemy[2].dead=true)
       and (wave>109)
       then begin
            AfterBoss;
            levelover:=true;
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
      if (ch='S') or (ch='s') then sbeffects:=not(sbeffects);

      if ch2='<' then savegame;

      end;
      if (ch=' ') then begin
         if (bullet1out=false) then begin
            if sbeffects then StartSound(Sound[4], 0, false);
            bullet1out:=true;
            bullet1x:=shipx+15;
            bullet1y:=165;
            putshape(shape1off,vaddr,18,17,bullet1x,bullet1y);
         end
         else
         if (bullet2out=false) then begin
            if sbeffects then StartSound(Sound[4], 0, false);
            bullet2out:=true;
            bullet2x:=shipx+15;
            bullet2y:=165;
            putshape(shape1off,vaddr,18,17,bullet2x,bullet2y);
       end;
    end;

{***MOVE SHIP***}
        shipx:=shipx+shipadd;
        if (shipx<1) then shipx:=1;
        if (shipx>190) then shipx:=190;
        CASE shipframe of
           1   : putshape (bigship1off,vaddr,48,29,shipx,165);
           3   : putshape (bigship3off,vaddr,48,29,shipx,165);
           2,4 : putshape (bigship2off,vaddr,48,29,shipx,165);
        END;
        inc(shipframe);;
        if shipframe=5 then shipframe:=1;
    waitretrace;
    flipd320 (vaddr,vga);
    for i:=0 to whatdelay do waitretrace;
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
   grapherror:=Mode13LoadPicpacked(0,0,vaddr,false,true,'ships.tb1');
   for i:=0 to 47 do
       for j:=0 to 29 do begin
           ShapeTable1^[bigship1off+((j*48)+i)]:=getpixel(i,j,vaddr);
           ShapeTable1^[bigship2off+((j*48)+i)]:=getpixel(i,j+32,vaddr);
           ShapeTable1^[bigship3off+((j*48)+i)]:=getpixel(i,j+64,vaddr);
       end;

 grapherror:=Mode13LoadPicpacked(0,0,vaddr,false,true,'tbshapes.tb1');
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
  grapherror:=Mode13LoadPicpacked(0,0,vga,true,false,'moon2.tb1');
  fade;
  cls(0,vga);
  grapherror:=Mode13LoadPicpacked(0,0,vga,false,true,'moon2.tb1');
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
  cls(0,vga);
  grapherror:=Mode13LoadPicpacked(0,0,vga,true,false,'tbl2ship.tb1');
  fade;
  grapherror:=Mode13LoadPicpacked(0,0,vga,false,true,'tbl2ship.tb1');
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
  grapherror:=Mode13LoadPicpacked(0,0,vga,true,false,'tbsobj.tb1');

  hiscore:=showhiscore(false,false);


  if level=1 then begin
     setupsidebar;
     y:=0;
     levelone;
     if level=2 then littleopener2;
  end;
  if level=2 then begin
     setupsidebar;
     beginscore:=score;
     beginshields:=shields;
     leveltwoengine(2);
  end;
  if level=3 then begin
     setupsidebar;
     beginscore:=score;
     beginshields:=shields;
     littleopener3;
     levelthree;
  end;
  if level=4 then begin
     setupsidebar;
     beginscore:=score;
     beginshields:=shields;
     leveltwoengine(4);
  end;




  clearkeyboardbuffer;

  coolbox(70,85,170,110,true,vga);
  outtextxy('GAME OVER',84,95,4,7,vga,false);
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



 function HexW(W: word): string; {Word}
    const
      HexChars: array [0..$F] of Char = '0123456789ABCDEF';
    begin
      HexW :=
        HexChars[(W and $F000) shr 12] +
        HexChars[(W and $0F00) shr 8]  +
        HexChars[(W and $00F0) shr 4]  +
        HexChars[(W and $000F)];
    end;

  procedure OurExitProc; far;
   {If the program terminates with a runtime error before the extended memory}
   {is deallocated, then the memory will still be allocated, and will be lost}
   {until the next reboot.  This exit procedure is ALWAYS called upon program}
   {termination and will deallocate extended memory if necessary.            }
    var
      i: byte;
    begin
      for i := 0 to NumSounds-1 do
        if Sound[i] <> nil then FreeSound(Sound[i]);
      if SharedEMB then ShutdownSharing;
      ExitProc := OldExitProc; {Chain to next exit procedure}
    end;

procedure Init;

  begin
      if not(GetSettings(BaseIO, IRQ, DMA, DMA16)) then
      begin
        baseio:=xbaseio; irq:=xirq; dma:=xdma; dma16:=0;
      end;
      if not(InitSB(BaseIO, IRQ, DMA, DMA16)) then
      begin
        settext;
        writeln('Error initializing sound card');
        writeln('Incorrect base IO address, sound card not installed, or broken');
        writeln('Check your BLASTER= environmental value or re-run TBSETUP.');
        Halt(2); {Sound card could not be initialized}
      end;
      if not(InitXMS) then
      begin
            writeln('Error initializing extended memory');
            writeln('HIMEM.SYS must be installed');
            writeln('You might be better off by choosing no sound effects');
            writeln('In the TBSETUP program');
            Halt(3); {XMS driver not installed}
        end
        else
          begin
            if GetFreeXMS < XMSRequired
              then
                begin
                  writeln('Insufficient free XMS');
                  writeln('You might be better off by choosing no sound effects');
                  writeln('In the TBSETUP program.');
                  Halt(4); {Insufficient XMS memory}
                end
              else
                begin
                  if SharedEMB then InitSharing;
                  OpenSoundResourceFile('SOUNDS.TB1');
                  LoadSound(Sound[0], 'CLICK');
                  LoadSound(Sound[1], 'AHH');
                  LoadSound(Sound[2], 'ZOOP');
                  LoadSound(Sound[3], 'KAPOW');
                  LoadSound(Sound[4], 'CC');
                  LoadSound(Sound[5],'BONK');
                  LoadSound(Sound[6],'OW');
                  LoadSound(Sound[7],'SCREAM');
                  CloseSoundResourceFile;
                  OldExitProc := ExitProc;
                  ExitProc := @OurExitProc;
                end
          end;
      InitMixing;
    end;

  procedure Shutdownsb1;
    begin
      ShutdownMixing;
      ShutdownSB5;

      for i := 0 to NumSounds-1 do
        FreeSound(Sound[i]);
      if SharedEMB then ShutdownSharing;
      writeln;
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
   move(imagedata,screen,4000);
   textcolor(7);
   if sbsound then shutdownsb1;
   shutdown;
   halt;
  menu2:
  barpos:=6;
end;
  









procedure options;
var opbarpos,argh:integer;
begin
  cls(0,vaddr);
  coolbox(0,0,319,199,false,vaddr);
  flipd320(vaddr,vga);
  outtextxy('ESC QUITS',120,175,32,0,vga,true);
  opbarpos:=0;
    repeat
    if sbeffects then begin
      if opbarpos=0 then outtextxy('SOUND ON ',30,30,32,7,vga,true)
         else outtextxy('SOUND ON ',30,30,32,0,vga,true);
    end;
    if not(sbeffects) then begin
      if opbarpos=0 then outtextxy('NO SOUND ',30,30,32,7,vga,true)
         else outtextxy('NO SOUND ',30,30,32,0,vga,true);
    end;
      if opbarpos=1 then outtextxy('VIEW HIGH SCORES',30,40,32,7,vga,true)
         else outtextxy('VIEW HIGH SCORES',30,40,32,0,vga,true);
      ch:=menuread;
       if (ord(ch)=222) or (ord(ch)=220) then inc(opbarpos);
       if (ord(ch)=223) or (ord(ch)=221) then dec(opbarpos);
       if ch='M' then opbarpos:=0;
       if ch='V' then opbarpos:=1;
       if (ch=#13) and (opbarpos=0) then sbeffects:=not(sbeffects);
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
  ondisk:='';
  coolbox(90,75,230,125,true,vga);
  outtextxy('LOAD WHICH GAME',97,82,9,7,vga,false);

  findfirst('SG?.TB1',ANYFILE,filestuff);
  while doserror=0 do with filestuff do
  begin
     ondisk:=concat(ondisk,name[3]);
     findnext(filestuff);
  end;
  if ondisk='' then ondisk:='NO GAMES SAVED';
  outtextxy(ondisk,97,92,4,7,vga,false);
  repeat until keypressed;
  ch:=readkey;
  if (ch>='0') and (ch<='9') then begin
     {$I-}
     ondisk:=concat('SG',ch,'.TB1');
     assign(savegamef,ondisk);
     reset(savegamef);
     {$I+}
     If IOResult=0 then begin
        readln(savegamef,score);
        readln(savegamef,level);
        readln(savegamef,shields);
        close(savegamef);
        playthegame(level);
     end;
  end;

end;



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
    flipd320(vaddr,vga);
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
  error:=Mode13LoadPicpacked(0,0,vaddr,false,true,'tbsobj.tb1');
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
  error:=Mode13LoadPicpacked(0,0,vaddr2,false,true,'tbcobj.tb1');
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

  error:=Mode13LoadPicpacked(0,0,vga,false,true,'tbchief.tb1');

  unfade;
  blockmove(115,55,206,114,vga,115,55,vaddr);
  
  {pauseawhile(600); if keypressed then if readkey=#27 then exit;}

  outtextxy('Ahhh.... Mr. Bombem.... ',1,1,15,0,vga,true);
  if sbeffects then startsound(sound[1],0,false);
  pauseawhile(200); if keypressed then if readkey=#27 then exit;
  if sbeffects then startsound(sound[3],0,false);
  blockmove(188,14,279,73,vaddr2,115,55,vga);
  pauseawhile(500); if keypressed then if readkey=#27 then exit;
  blockmove(115,55,206,114,vaddr,115,55,vga);
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
  error:=Mode13LoadPIcpacked(0,0,vaddr2,false,true,'tbma1.tb1');
{  outsmalltextxy('MY WIFE AND I FOUNDED',212,3,14,0,vaddr2,false);
  outsmalltextxy('THIS BASE IN 2008.',212,9,14,0,vaddr2,false);
  outsmalltextxy('THE ONLY WAY TO ',212,16,13,0,vaddr2,false);
  outsmalltextxy('FINANCE IT WAS TO',212,22,13,0,vaddr2,false);
  outsmalltextxy('ENGAGE IN A DUBIOUS',212,28,13,0,vaddr2,false);
  outsmalltextxy('BUSINESS.',212,34,13,0,vaddr2,false);
  outsmalltextxy('WE LAUNCHED EARTH''S',212,41,12,0,vaddr2,false);
  outsmalltextxy('TRASH INTO SPACE',212,47,12,0,vaddr2,false);
  outsmalltextxy('FOR A PROFIT.',212,53,12,0,vaddr2,false);
  outsmalltextxy('HERE IS FOOTAGE FROM',212,60,11,0,vaddr2,false);
  outsmalltextxy('THE LAST LAUNCH EIGHT',212,66,11,0,vaddr2,false);
  outsmalltextxy('YEARS AGO.',212,72,11,0,vaddr2,false);}
  flipd320(vaddr2,vaddr);
  putshape(bargeoff,vaddr,16,18,141,157);
  flipd320(vaddr,vga);
  unfade;

  pauseawhile(700); if keypressed then if readkey=#27 then exit;

  for ytemp:=191 downto 164 do begin
      if ytemp>=172 then blockmove(145,ytemp,152,ytemp+10,vaddr2,145,ytemp,vaddr)
                    else blockmove(145,172,152,182,vaddr2,145,172,vaddr);
      puttruck(145,ytemp,ytemp mod 2,vaddr);
      pauseawhile(7);
      if keypressed then if readkey=#27 then exit;
      flipd320(vaddr,vga);
  end;

  pauseawhile(20);
  flipd320(vaddr2,vaddr);
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
      flipd320(vaddr,vga);
      ytemp:=ytemp-round(thrust);
  end;

  pauseawhile(100);
  if keypressed then if readkey=#27 then exit;
  fade;

{****SECOND CHIEF*******}
 error:=Mode13LoadPicpacked(0,0,vaddr2,false,true,'tbcobj.tb1');
 error:=Mode13LoadPicpacked(0,0,vga,false,true,'tbchief.tb1');
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
  error:=Mode13LoadPicpacked(0,0,vaddr2,false,true,'tbcrash.tb1');
  alienchar:=34;
  flipd320(vaddr2,vaddr);
  flipd320(vaddr,vga);
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
     if sbeffects then startsound(sound[3],0,false);
     for xtemp:=0 to 10 do begin
        putshapeover(explo2off,vaddr,16,18,160,118);
        doflames;
        putshapeover(explo1off,vaddr,16,18,160,118);
        doflames;
     end;
     blockmove(159,114,189,144,vaddr2,159,114,vaddr);
     putshapeover(rent1off,vaddr,16,6,160,115);
     flipd320(vaddr,vga);
     cycles:=0;
  repeat
    doflames;
    inc(cycles);
  until ((keypressed) or (cycles>60));
  if keypressed then if readkey=#27 then exit;

{****ALIEN MESSAGE*****}
  fade;
  error:=Mode13LoadPicpacked(0,0,vga,false,true,'tbgorg.tb1');
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
 error:=Mode13LoadPicpacked(0,0,vaddr2,false,true,'tbcobj.tb1');
 error:=Mode13LoadPicpacked(0,0,vga,false,true,'tbchief.tb1');
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

procedure credits;
var j:integer;
    sp:boolean;
    ky:word;
  procedure rotate(stri:string;col:integer);
  var j1,k1:integer;

  begin
   if not(sp) then begin
    for j1:=0 to 7 do begin

            outtextline(stri,0,198,col,0,j1,vga);
        {move(mem vga:320 vga:0 63680}
        asm
         push ds;
         push es;
          mov ax,vga
          mov ds,ax
          mov es,ax
          mov cx,31840
          mov si,320
          mov di,0
          rep movsw
         pop es;
         pop ds;
        end;
        if keypressed then begin sp:=true; ch:=readkey; end;
    end;
   end;
  end;
  procedure skip; begin rotate(' ',0); end;
  label ender;
begin
  sp:=false;
  cls(0,vaddr);
  flipd320(vaddr,vga);
  j:=0;
  if keypressed then ch:=readkey;
      rotate('               TOM BOMBEM',4);
      rotate('    INVASION OF THE INANIMATE OBJECTS',4);
      skip;       rotate('              PROGRAMMING',9);
      skip;       rotate('            VINCENT M WEAVER',9);
      skip; skip; rotate('                GRAPHICS',10);
        if sp then goto ender;
      skip;       rotate('            VINCENT M WEAVER',10);
      skip; skip; rotate('             SOUND EFFECTS',11);
        if sp then goto ender;
      skip;       rotate('            VINCENT M WEAVER',11);
      skip; skip; rotate('          GRAPHICS INSPIRATION',12);
        if sp then goto ender;
      skip;       rotate('              JEFF WARWICK',12);
      skip; skip; rotate('              GENERAL HELP',13);
        if sp then goto ender;
      skip;       rotate('              JOHN CLEMENS',13);
      skip;       rotate('              JASON GRIMM',13);
      skip; skip; rotate('           PCGPE AUTHORS, esp',14);
        if sp then goto ender;
      skip;       rotate('              GRANT SMITH',14);
      skip; skip; rotate('           SOUND BLASTER CODE',15);
        if sp then goto ender;
      skip;       rotate('              ETHAN BRODSKY',15);
      skip; skip; rotate('               INSPIRATION',9);
        if sp then goto ender;
      skip;       rotate('              DOUGLAS ADAMS',9);
      skip;       rotate('               CLIFF STOLL',9);
      skip;       rotate('             ARTHUR C CLARKE',9);
      skip;       rotate('               ISAAC ASIMOV',9);
      skip;       rotate('              GORDON KORMAN',9);
      skip; skip; rotate('         THANKS TO ALL THE AGENTS',10);
        if sp then goto ender;
      skip;       rotate('        B,D,JL,L,N,P,S,W,PM,E,G,TK',10);
      skip;       rotate('           AND ESPECIALLY MP',10);

  ender:
      ky:=0;
      repeat
        asm
         push ds;
         push es;
          mov ax,vaddr2
          mov ds,ax
          mov ax,vga
          mov es,ax
          mov ax,ky
          shl ax,5
          mov cx,ax
          shl ax,1
          shl cx,3
          add ax,cx
          mov si,ax
          mov di,63680
          mov cx,160
          rep movsw
         pop es;
         pop ds;
        end;

   {move(mem[vaddr2:(ky*320)],mem[vga:(32000)],320);}
     asm
      push ax;
      push ds;
      push es;
      push cx;
      push si;
      push di;
       mov ax,vga
       mov ds,ax
       mov es,ax
       mov cx,31840
       mov si,320
       mov di,0
       rep movsw
      pop di;
      pop si;
      pop cx;
      pop es;
      pop ds;
      pop ax;
   end;{
          Move (mem[vga:320],mem[vga:0],63680);    }
          inc(ky);
      until (keypressed) or (ky=199);
      if keypressed then ch:=readkey;
  end;


procedure register;

var pagenum,oldpagenum,numpages:integer;
    pagest:string;
    numst:string[2];

procedure page1;
begin
  flipd320(vaddr,vga);
  shadowrite('              TO REGISTER',10,10,9,1);
  shadowrite('I STARTED THIS GAME IN LATE',70,30,9,1);
  shadowrite('  1994, WHEN I WAS 16.',70,40,9,1);
  shadowrite('I WROTE THIS GAME ENTIRELY IN',75,50,9,1);
  shadowrite('  MY FREE TIME.',74,60,9,1);
  shadowrite('   ^(AUTHOR AT AGE 17)',10,70,10,2);
  shadowrite('HOPEFULLY YOU FEEL MY FREE TIME IS',10,90,12,4);
  shadowrite('  WORTH SOMETHING.  YOU DO NOT NEED',10,100,12,4);
  shadowrite('  TO SEND MONEY, BUT ANY GIFT WOULD',10,110,12,4);
  shadowrite('  BE APPRECIATED.  ALSO I WOULD BE',10,120,12,4);
  shadowrite('  GLAD TO HEAR ANY COMMENTS AND ',10,130,12,4);
  shadowrite('  ANSWER ANY QUESTIONS.',10,140,12,4);
end;
procedure page2;
begin
  flipd320(vaddr2,vga);
  shadowrite('I CAN BE REACHED AS:',10,10,10,2);
  shadowrite('     VINCENT WEAVER',10,20,10,2);
  shadowrite('     326 FOSTER KNOLL DR.',10,30,10,2);
  shadowrite('     JOPPA, MD 21085-4706, USA, ETC.',10,40,10,2);
  shadowrite('AFTER THE SUMMER OF 1996 I WILL BE',10,60,13,5);
  shadowrite(' AWAY AT COLLEGE, AT THE UNIVERSITY',10,70,13,5);
  shadowrite(' OF MARYLAND, COLLEGE PARK.',10,80,13,5);
  shadowrite('GET THE NEWEST VERSION OF TB1 AT',10,100,11,3);
  shadowrite(' THE OFFICIAL TB1 WEB SITE:',10,110,11,3);
  shadowrite(' http://www.wam.umd.edu/~vmweaver/tb1/',10,120,11,3);
  shadowrite('I CAN BE CONTACTED VIA E-MAIL AT:',10,140,12,4);
  shadowrite('  VMWEAVER@WAM.UMD.EDU',10,150,9,1);
  shadowrite('FEEL FREE TO SEND COMMENTS/MONEY.',10,160,12,4);
end;

procedure page3;
begin
  flipd320(vaddr2,vga);
  shadowrite('OTHER VMW SOFTWARE PRODUCTIONS:',10,10,15,7);
  shadowrite(' PAINTPRO:',10,30,13,5);
  shadowrite('   LOAD AND SAVE GRAPHICS PICTURES',10,40,13,5);
  shadowrite('   INTO C, PASCAL, BASIC, ETC.',10,50,13,5);
  shadowrite('   WITH SCREEN CAPTURE UTILITY.',10,60,13,5);
  shadowrite(' SPACEWAR III:',10,70,11,3);
  shadowrite('   NEVER COMPLETED GAME WITH WORKING',10,80,11,3);
  shadowrite('   SPACESHIPS.  SORT OF COOL.',10,90,11,3);
  shadowrite(' AITAS: (ADVENTURES IN TIME AND SPACE)',10,100,12,4);
  shadowrite('   THIS GAME WILL BE FINISHED SOMEDAY.',10,110,12,4);
  shadowrite('   IT HAS BEEN UNDER WAY FOR 5 YEARS.',10,120,12,4);
{  shadowrite(' MISC PASCAL/BASIC PROGRAMS:',10,130,9,1);
  shadowrite('   OVER 500 PROGRAMS WRITTEN OR TYPED',10,140,9,1);
  shadowrite('   IN BY ME....FUN TO LOOK AT.',10,150,9,1);}
end;

procedure page4;
begin
  flipd320(vaddr2,vga);
  shadowrite('DISCLAIMERS:',10,10,12,14);
  shadowrite('* THE ABOVE PROGRAMS HAVE NEVER DONE *',8,30,12,4);
  shadowrite('* ANYTHING BAD TO MY COMPUTER THAT   *',8,40,12,4);
  shadowrite('* CTRL-ALT-DEL WOULDN''T FIX.  I AM   *',8,50,12,4);
  shadowrite('* NOT RESPONSIBLE FOR HARD DISK      *',8,60,12,4);
  shadowrite('* DISSAPPEARANCES, MISSING MODEMS    *',8,70,12,4);
  shadowrite('* MOUSE BREAKDOWNS, MELTING MONITORS *',8,80,12,4);
  SHADOWRITE('* OR ANYTHING ELSE.                  *',8,90,12,4);
  shadowrite('% ALL VMW SOFTWARE PRODUCTIONS ARE   %',8,110,11,3);
  shadowrite('% RELEASED VIRUS FREE !!!!!!!!!!!!   %',8,120,11,3);
end;

begin
 fade;
 cls(0,vga);
 unfade;
 cls(0,vaddr);
 grapherror:=Mode13LoadPIcpacked(0,0,vaddr,true,true,'register.tb1');
  coolbox(0,0,319,199,false,vaddr);
 cls(0,vaddr2);
  coolbox(0,0,319,199,false,vaddr2);
 pagenum:=1;
 oldpagenum:=1;
 numpages:=4;
 page1;
 shadowrite('PAGE  1 of  4: ESC QUITS',50,180,15,7);
 repeat
 ch:=menuread;
     if (ch=' ') or (ch=#13) then inc(pagenum);
     if (ch='õ') or (ch='Þ') or (ch='Ü') then inc(pagenum);
     if (ch='ô') or (ch='Ý') or (ch='ß') then dec(pagenum);
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
  fade;
  cls(0,vga);
  unfade;
  grapherror:=Mode13LoadPIcpacked(0,0,vaddr2,true,true,'tbomb1.tb1');
  fade;
end;


label picloader,menu;

begin
   CDROMmode:=false;
   for i:=1 to paramcount do begin
       string_param:=paramstr(i);
       for j:=1 to length(string_param) do
           string_param[j]:=upcase(string_param[j]);
       if pos('CDROM',string_param)<>0 then CDROMmode:=true;
       if (pos('HELP',string_param)<>0)
       or (pos('H',string_param)<>0) then begin
          writeln('Tom Bombem');
          writeln;
          writeln('Command line:  tb1 [-CDROM] [-HELP]');
          writeln;
          writeln('-CDROM starts game in read-only mode');
          writeln('-HELP gives this message');
          writeln;
          halt(7);
       end;
   end;

   setuptb1;
   ReadConfigFile;
   axel_und_carmen:=true;
      {as_of_9-22-94} {change_back_10-6-94} {uh_oh 2-21-95} {gone for real long time 10-12-95}
      {11-95 not carmen anymore, but Gus}
      {3-26-96 oh well... gave up on Gus finally}
      {5-11-96 Now Marie... what fun life is}
   if xsoundeffects=1 then soundeffects:=true
                      else soundeffects:=false;
   if xsbsound=0 then sbsound:=false
                 else sbsound:=true;
   if (sbsound) and (soundeffects) then sbeffects:=true;
   if sbsound then init;
   randomize;
   setupvirtual;
   fade;
   setmcga;
   for x:=0 to 40 do begin
       pal(100+x,x+20,0,0);
       pal(141+x,0,0,x+20);
       pal(182+x,0,x+20,0);
   end;
   fade;

     for x:=0 to 40 do begin
       verticalline(45,45+(2*x),x+40,100+x,vga);
       verticalline(45,45+(2*x),x+120,141+x,vga);
       verticalline(45,45+(2*x),x+200,141+x,vga);
       verticalline(125-(2*x),125,x+80,182+x,vga);
       verticalline(125-(2*x),125,x+160,182+x,vga);
   end;
   for x:=40 downto 0 do begin
       verticalline(45,125-(2*x),x+80,140-x,vga);
       verticalline(45,125-(2*x),x+160,181-x,vga);
       verticalline(45,125-(2*x),x+240,181-x,vga);
       verticalline(125,45+(2*x),x+120,222-x,vga);
       verticalline(125,45+(2*x),x+200,222-x,vga);
   end;
   unfade;
   {if sbeffects then startsound(sound[0],0,false);}

   outtextxy('A VMW SOFTWARE PRODUCTION',60,140,15,15,VGA,false);
   y:=0;
   hiscore:=0;
   pauseawhile(400);

   fade;
   cls(0,vga);
   grapherror:=Mode13LoadPicpacked(0,0,vga,true,false,'tbomb1.tb1');
   fade;
   grapherror:=Mode13LoadPicpacked(0,0,vga,false,true,'tbomb1.tb1');
   unfade;

PICLOADER:
   grapherror:=Mode13LoadPicpacked(0,0,vaddr2,true,true,'tbomb1.tb1');
   if not(axel_und_carmen) then begin
      for tempi:=193 to 199 do
          for tempj:=290 to 319 do
          putpixel(tempj,tempi,0,vaddr2);
   end;
MENU:


   flipd320(vaddr2,vga);
   unfade;
   gettime(ho,mi,se,s100);
   tempsec:=se-30;
   if tempsec<0 then tempsec:=tempsec+60;
   repeat
     gettime(ho,mi,se,s100);
     if se=tempsec then begin

        credits;
        flipd320(vaddr2,vga);
        gettime(ho,mi,se,s100);
        tempsec:=se-30;
        if tempsec<0 then tempsec:=tempsec+60;

     end;
   until keypressed;
   ch:=readkey;
   clearkeyboardbuffer;
   barpos:=0;
   outtextxy('F1 HELP',0,190,9,7,vga,false);
   coolbox(117,61,199,140,true,vga);
   repeat
      if barpos=0 then outtextxy('NEW GAME',123,67,32,0,vga,true)
         else outtextxy('NEW GAME',123,67,32,7,vga,true);
      if barpos=1 then outtextxy('OPTIONS',123,77,32,0,vga,true)
         else outtextxy('OPTIONS',123,77,32,7,vga,true);
      if barpos=2 then outtextxy('REGISTER',123,87,32,0,vga,true)
         else outtextxy('REGISTER',123,87,32,7,vga,true);
      if barpos=3 then outtextxy('LOAD GAME',123,97,32,0,vga,true)
         else outtextxy('LOAD GAME',123,97,32,7,vga,true);
      if barpos=4 then outtextxy('STORY',123,107,32,0,vga,true)
         else outtextxy('STORY',123,107,32,7,vga,true);
      if barpos=5 then outtextxy('CREDITS',123,117,32,0,vga,true)
         else outtextxy('CREDITS',123,117,32,7,vga,true);
      if barpos=6 then outtextxy('QUIT',123,127,32,0,vga,true)
         else outtextxy('QUIT',123,127,32,7,vga,true);

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

   if barpos=6 then quit;
   if barpos=1 then options;
   if barpos=2 then register;
   if barpos=3 then loadgame;
   if barpos=4 then
      begin
        story;
        fade;
        cls(0,vga);
      end;
   if barpos=5 then credits;
   if barpos=10 then help;
   if barpos=0 then playthegame(0);
   if barpos=0 then goto picloader;
   if barpos=4 then goto picloader;
   if barpos=3 then goto picloader;
   goto menu;
end.