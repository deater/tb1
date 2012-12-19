program developnewvmwtpu;
uses nvmwgraph,crt;
var error:byte;
    ch:char;
    i:integer;
begin
  setmcga;
  {for i:=0 to 39 do
      horizontalline(0,319,i*5,i,vga);}
  for i:=0 to 319 do
      verticalline(0,199,i,i mod 215,vga);

  error:=loadpicsuperpacked(0,0,vga,true,true,'t2tract.tb1');
  {error:=savepicsuperpacked(0,0,239,50,256,vga,'t2tract.tb1');}


  readln;
  for i:=0 to 319 do
      verticalline(0,199,i,i mod 215,vga);
  readln;
  settext;
end.