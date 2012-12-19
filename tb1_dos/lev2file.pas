var background:array[0..11,0..100] of byte;
    i,j:integer;
    f:text;


begin
  for i:=0 to 11 do
      for j:=0 to 100 do
          background[i,j]:=0;
  assign(f,'level2.tbx');
  rewrite(f);
  for j:=0 to 100 do begin
      for i:=0 to 10 do write(f,background[i,j],' ');
      writeln(f,background[11,j]);
  end;
  close(f);

  assign(f,'level2.tbx');
  reset(f);
  for j:=0 to 100 do
      for i:=0 to 10 do read(f,background[i,j]);

  close(f);
  for j:=0 to 100 do for i:=0 to 10 do write(background[i,j],' ');
end.