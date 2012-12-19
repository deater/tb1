program maketb1better;
uses vmwgraph;
var palf:text;
    x,y,i,j:integer;
    temp:array[1..3] of byte;
    grapherror:byte;

begin
  setmcga;
  cls(0,vga);
   assign(palf,'pal.tb1');
   reset(palf);
   for i:=0 to 255 do begin
       for j:=1 to 3 do readln(palf,temp[j]);
       pal(i,temp[1],temp[2],temp[3]);
   end;
   close(palf);
   grapherror:=loadpicsuperpacked(0,0,vga,'tbomb1.tb1');
   
   readln;
   settext;
end.