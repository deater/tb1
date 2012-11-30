program TOM_BOMBEM_SETUP;

uses dos,crt,detect;

{$I tbs1.pas}

type screentype = array[0..3999] of byte;

var screen: screentype absolute $B800:0000;
    ch:char;
    i:byte;
    getsettings,redo:boolean;
    BLASTER: string;
    BASEIO,DMA,IRQ,DMA16:word;
    sbsound,soundeffects:byte;
    ii:integer;
    ff:text;
    bobb:word;

label bob,skipper;

function gets(x,y,z,a:integer;hex:boolean):word;
var tempst:string;
    tempw:word;
    codei:integer;
begin
   codei:=0;
   gotoxy(x,y);
   readln(tempst);
   if hex then begin val(tempst,bobb,codei); insert('$',tempst,1); end;
   val(tempst,tempw,codei);
   if (tempw>=a) or (tempw<=z) then tempw:=255;
   if codei<>0 then tempw:=255;
   gets:=tempw;
end;

function UpcaseStr(Str: string): string;
  var     i: byte;
          Temp: string;

  begin
    Temp[0] := Str[0];
    for i := 1 to Length(Str) do Temp[i] := Upcase(Str[i]);
    UpcaseStr := Temp;
  end;

function getsetting(str:string;id:char;hex:boolean):word;
  var   Temp : string;
        Num  : word;
        Code : integer;
  begin
    Temp := Str;
    if Pos(ID, Temp) <> 0 then begin
       Delete(Temp, 1, Pos(ID, Temp));
       Delete(Temp, Pos(' ', Temp), 255);
       if Hex then begin val(temp, bobb, code); Insert('$', Temp, 1); end;
       Val(Temp, Num, Code);
       if Code = 0 then GetSetting := Num
                   else GetSetting := $FF;
     end
     else GetSetting := $FF;
end;

procedure quit;
begin
  halt(0);
end;

begin
{OPENING}
   clrscr;
     move(imagedata1,screen,4000);
bob:
  redo:=false;
{REQUIREMENTS}
   clrscr;
     move(imagedata2,screen,4000);
     repeat
       repeat until keypressed;
       ch:=readkey;
       if upcase(ch)='Q' then quit;
     until ch=#13;
{Ask About SoundBlaster Sound}
     clrscr;
     sbsound:=0;
       move(imagedata4,screen,4000);
       for i:=11 to 20 do begin
           gotoxy(4,i);
           writeln('                                                        ');
       end;
       textcolor(13);
       gotoxy(4,12);  write('Do You Want To Have Sound Effects?');
       gotoxy(6,13);  write('(y/n)---->');
       gotoxy(17,13);
       textcolor(3);
       repeat
         repeat until keypressed;
         ch:=readkey;
         if (upcase(ch)='Y') or (upcase(ch)='N') then begin
            if upcase(ch)='Y' then soundeffects:=1;
            if upcase(ch)='N' then soundeffects:=0;
            write(ch);
            gotoxy(17,13);
         end;
         if upcase(ch)='Q' then quit;
       until ch=#13;
       if soundeffects=0 then goto skipper;
{check if want soundbalster}
       textcolor(13);
       gotoxy(4,16);  write('Do You Have A Soundblaster Card?');
       gotoxy(6,17);  write('(y/n)---->');
       gotoxy(17,17);
       textcolor(3);
       repeat
         repeat until keypressed;
         ch:=readkey;
         if (upcase(ch)='Y') or (upcase(ch)='N') then begin
            if upcase(ch)='Y' then sbsound:=1;
            if upcase(ch)='N' then sbsound:=0;
            write(ch);
            gotoxy(17,17);
         end;
         if upcase(ch)='Q' then quit;
       until ch=#13;

{If Soundblaster, then configure it}
   if (sbsound=1) then begin
     clrscr;
     move(imagedata4,screen,4000);
        BLASTER := UpcaseStr(GetEnv('BLASTER'));
        BaseIO := GetSetting(BLASTER, 'A', true);  {Hex}
        IRQ    := GetSetting(BLASTER, 'I', false); {Decimal}
        DMA    := GetSetting(BLASTER, 'D', false); {Decimal}
        DMA16  := GetSetting(BLASTER, 'H', false); {Decimal}


    ii:=-1;
    repeat
      ii:=ii+22;
      textcolor(3);
      gotoxy(ii,14); write(bobb);
      gotoxy(ii,15); write(irq);
      gotoxy(ii,16); write(dma);
      gotoxy(ii,17); write(dma16);

        if BaseIO  = $FF then begin gotoxy(ii,14); write('---'); end;
        if IRQ     = $FF then begin gotoxy(ii,15); write('---'); end;
        if DMA     = $FF then begin gotoxy(ii,16); write('---'); end;
        if DMA16   = $FF then begin gotoxy(ii,17); write('---'); end;
    until ii>=43;

     gotoxy(14,20);
     repeat
       gotoxy(14,20);
       repeat until keypressed;
       ch:=readkey;

           if (ch>'0') and (ch<'5') then begin
              case ch of
                '1':begin gotoxy(43,14); writeln('     ');
                          baseio:=gets(43,14,100,400,true);
                          gotoxy(43,14); writeln('     ');
                          gotoxy(43,14); write(bobb);
                          end;
                '2':begin gotoxy(43,15); writeln('     ');
                          irq:=gets(43,15,2,12,false);
                          gotoxy(43,15); writeln('     ');
                          gotoxy(43,15); write(irq);
                          end;
                '3':begin gotoxy(43,16); writeln('     ');
                          dma:=gets(43,16,0,100,false);
                          gotoxy(43,16); writeln('     ');
                          gotoxy(43,16); write(dma);
                          end;
                '4':begin gotoxy(43,17); writeln('     ');
                          dma16:=gets(43,17,0,100,false);
                          gotoxy(43,17); writeln('     ');
                          gotoxy(43,17); write(dma16);
                          end;
              end;
           end;
       if upcase(ch)='Q' then quit;
     until ch=#13;
   end;
{Check for mistakes}

skipper:
   clrscr;
     move(imagedata5,screen,4000);
     gotoxy(16,12);
     textcolor(3);

     if soundeffects=1 then begin
        write('Sound Effects : ');
        if sbsound=1 then writeln('Sound Blaster')
           else writeln('No Soundblaster');
     end
     else writeln('No Sound Effects');

     if (sbsound=1) then begin
        gotoxy(16,14);
        writeln('BaseIo:',bobb);
        gotoxy(16,15);
        writeln('IRQ:   ',irq);
        gotoxy(16,16);
        writeln('DMA:   ',dma);
     end;
     repeat
       repeat until keypressed;
       ch:=readkey;
       if upcase(ch)='R' then begin redo:=true; ch:=#13; end;
       if upcase(ch)='Q' then quit;
     until ch=#13;
     if redo then goto bob;
     assign(ff,'config.tb1');
     rewrite(ff);
     writeln(ff,soundeffects);
     writeln(ff,sbsound);
     writeln(ff,baseio);
     writeln(ff,irq);
     writeln(ff,dma);
     close(ff);
   clrscr;
   move(imagedata6,screen,4000);
   gotoxy(1,24);
end.