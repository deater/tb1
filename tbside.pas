uses crt,vmwgraph;
var i,j:integer;
    palf:text;
    temp:array[0..3] of byte;
procedure makehole(y:integer);
begin
  for i:=10 to 75 do line(239+i,y,239+i,y+9,0,vga);
  line(249,y,314,y,24,vga);
  line(249,y+10,313,y+10,18,vga);
  line(249,y,249,y+9,24,vga);
  line(314,y+1,314,y+10,18,vga);
end;
begin
  setmcga;
  assign(palf,'pal.tb1');
   reset(palf);
   for i:=0 to 255 do begin
       for j:=1 to 3 do readln(palf,temp[j]);
       pal(i,temp[1],temp[2],temp[3]);
   end;
   close(palf);
   for i:=240 to 319 do line(i,0,i,199,19,vga);
   line(240,0,240,199,18,vga);
   line(240,0,319,0,18,vga);
   line(319,0,319,199,24,vga);
   line(241,199,319,199,24,vga);
  outtextxy('SCORE',241,1,127,0,vga,false);
  outtextxy('SCORE',242,2,143,0,vga,false);
    makehole(10);
  outtextxy('00001233',251,12,12,0,vga,false);
  outtextxy('HI-SCORE',241,21,127,0,vga,false);
  outtextxy('HI-SCORE',242,22,143,0,vga,false);
  makehole(30);
  outtextxy('12345672',251,32,12,0,vga,false);
  outtextxy('LEVEL',241,41,127,0,vga,false);
  outtextxy('LEVEL',242,42,143,0,vga,false);
  makehole(50);
  outtextxy('12345675',251,52,12,0,vga,false);
  outtextxy('SHIELDS',241,61,127,0,vga,false);
  outtextxy('SHIELDS',242,62,143,0,vga,false);
  makehole(70);
  for i:=0 to 63 do line(250+i,71,250+i,79,((i div 4)+32),vga);

  outtextxy('WEAPONS',241,81,127,0,vga,false);
  outtextxy('WEAPONS',242,82,143,0,vga,false);
  makehole(90);
  for i:=0 to 65 do line(249+i,111,249+i,189,0,vga);
  line(249,111,249,189,24,vga);
  line(315,111,315,189,18,vga);
  line(249,111,315,111,24,vga);
  line(249,189,315,189,18,vga);

  outtextxy('  VMW   ',251,114,15,0,vga,false);
  outtextxy('F1-HELP  ',251,124,15,0,vga,false);
  outtextxy('ESC-QUIT',251,135,15,0,vga,false);
  outtextxy('F2-SAVE ',251,145,15,0,vga,false);

  readln;

  settext;
end.