
Procedure levelone;
VAR loop1,loop2:integer;
    ch,ch2:char;
    shieldcol:integer;
    saucersout:integer;
    bullet1x,bullet1y,bullet2x,bullet2y,i,wave:integer;
    bullet1out,bullet2out:boolean;
    whichone,temp,temp2,whichwave:integer;
    tempst:string;
    what:byte;
    itemp,jtemp:byte;


procedure waveincrease;
begin
  inc(wave);
  dec(saucersout);
  if saucersout<0 then saucersout:=0;
  if wave<6 then whichwave:=4;
  if (wave>=6) and (wave<=12) then whichwave:=4; {1}
  if (wave>12) and (wave<=20) then whichwave:=4; {2}
  if (wave>20) and (wave<=30) then whichwave:=4; {3}
  if (wave>30) and (wave<=36) then whichwave:=4; {2}
  if (wave>36) and (wave<=45) then whichwave:=3;
  if (wave>45) and (wave<=50) then whichwave:=1;
  if (wave>50) and (wave<=60) then whichwave:=3;
  if (wave>60) and (wave<=70) then whichwave:=2;
  if (wave>70) and (wave<=75) then whichwave:=1;
  if (wave>75) and (wave<=85) then whichwave:=3;
  if (wave>85) and (wave<=95) then whichwave:=2;
  if (wave>95) and (wave<=100) then whichwave:=1;
  if (wave>100) then whichwave:=4;
end;



BEGIN
  wave:=0;
  whichwave:=0;
  pal(254,0,0,0);
  shieldcol:=0;
  shipx:=36;
  shipadd:=0;
  shipframe:=1;
  shipspeed:=5;
  ch:=#1; ch2:=#1;
  for i:=0 to 4 do begin
      enemy[i].exploding:=false;
      enemy[i].out:=false;
  end;

  bullet1out:=false;  bullet2out:=false;
  bullet1x:=0; bullet1y:=0;
  bullet2x:=0; bullet2y:=0;
  saucersout:=0;


  flip(vaddr2,vaddr);
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
  {putshape240(shapearray[11],
                    vaddr2,18,17,10,20);}

  pal(254,shields*4,0,0);
  howmuchscroll:=65;

{**** GAME LOOP ****}
{*******************}

  Repeat
    ch2:=#1;
    ch:=#1;
    dec(howmuchscroll);
    if howmuchscroll<1 then begin
       flip65(vaddr2,2925,vaddr3,0);
       flip65(vaddr2,1950,vaddr2,2925);
       flip65(vaddr2,975,vaddr2,1950);
       flip65(vaddr2,0,vaddr2,975);
       flip65(vaddr3,0,vaddr2,0);
       howmuchscroll:=65;
    end;
    flip240(howmuchscroll);

{***Collsion Check***}
      for i:=0 to 4 do begin
          if (enemy[i].out=true) then begin
             if (bullet1out) then begin
                if collision(bullet1x+3,bullet1y,bullet1x+17,bullet1y+13,
                        enemy[i].x,enemy[i].y,enemy[i].x+20,
                        enemy[i].y+20) then begin
                        {StartSound(Sound[1], 0, false);}
                        dec(enemy[i].hitsneeded);
                        if enemy[i].hitsneeded=0 then enemy[i].dead:=true
                           else enemy[i].dead:=false;
                        enemy[i].exploding:=true;
                        enemy[i].explodprogress:=0;
                        bullet1out:=false;
                        inc(score,10);
                        changescore;
                end;
             end;
             if (bullet2out) then begin
                if collision(bullet2x+3,bullet2y,bullet2x+17,bullet2y+13,
                        enemy[i].x,enemy[i].y,enemy[i].x+20,
                        enemy[i].y+20) then begin
                        {StartSound(Sound[1], 0, false);}
                        dec(enemy[i].hitsneeded);
                        if enemy[i].hitsneeded=0 then enemy[i].dead:=true
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

              end;
              if enemy[i].y>140 then begin
                 if collision(shipx,165,
                              shipx+48,194,
                              enemy[i].x,enemy[i].y,
                              enemy[i].x+17,enemy[i].y+15)
                 then begin
                    enemy[i].exploding:=true;
                    enemy[i].explodprogress:=0;
                    dec(shields);
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
           enemy[temp].kind:=what;
           enemy[temp].x:=0;
           enemy[temp].y:=0;
           enemy[temp].xspeed:=5;
           enemy[temp].x:=temp*20;
           enemy[temp].minx:=(temp*20);
           enemy[temp].maxx:=(temp*20)+120;
           enemy[temp].boundarycheck:=true;
           enemy[temp].yspeed:=10;
           enemy[temp].out:=true;
           enemy[temp].exploding:=false;
           enemy[temp].hitsneeded:=1;
           enemy[temp].dead:=false;
        end;
    end;
   {**FALLING STRAIGHT**}
    if whichwave=3 then begin
       for temp:=0 to 4 do
           if enemy[temp].out=false then begin
              enemy[temp].kind:=random(8)+3;
              enemy[temp].x:=random(200)+1;
              enemy[temp].y:=0;
              enemy[temp].xspeed:=0;
              enemy[temp].minx:=enemy[temp].x;
              enemy[temp].maxx:=enemy[temp].x;
              enemy[temp].boundarycheck:=true;
              enemy[temp].yspeed:=5;
              enemy[temp].out:=true;
              enemy[temp].exploding:=false;
              enemy[temp].hitsneeded:=1;
              enemy[temp].dead:=false;
           end;
    end;
   {**FALLING GRADUALLY SIDEWAYS**}

    if whichwave=2 then begin
       for temp:=0 to 4 do
           if enemy[temp].out=false then begin
              enemy[temp].kind:=random(8)+3;
              enemy[temp].y:=0;
              enemy[temp].xspeed:=5;
              enemy[temp].minx:=random(100);
              enemy[temp].maxx:=random(100)+120;
              enemy[temp].x:=enemy[temp].minx;
              enemy[temp].boundarycheck:=false;
              enemy[temp].yspeed:=1;
              enemy[temp].out:=true;
              enemy[temp].exploding:=false;
              enemy[temp].hitsneeded:=1;
              enemy[temp].dead:=false;
           end;
    end;
  {**ZIG-ZAG**}
    if (whichwave=1) and (saucersout=0) then begin
       saucersout:=5;
       whichone:=random(8)+3;
       for temp:=0 to 4 do
           if enemy[temp].out=false then begin
              enemy[temp].kind:=whichone;
              enemy[temp].y:=temp*10;
              enemy[temp].xspeed:=5;
              enemy[temp].minx:=0;
              enemy[temp].maxx:=220;
              enemy[temp].x:=temp*20;
              enemy[temp].boundarycheck:=false;
              enemy[temp].yspeed:=1;
              enemy[temp].out:=true;
              enemy[temp].exploding:=false;
              enemy[temp].hitsneeded:=1;
              enemy[temp].dead:=false;
           end;
    end;
    if (whichwave=4) then begin
       if saucersout=0 then begin
          enemy[0].kind:=15;
          enemy[1].kind:=15;
          enemy[2].kind:=14;
          for temp:=0 to 2 do begin
              enemy[temp].x:=temp*20;
              enemy[temp].y:=0;
              enemy[temp].xspeed:=5;
              enemy[temp].minx:=0;
              enemy[temp].maxx:=220;
              enemy[temp].boundarycheck:=true;
              enemy[temp].yspeed:=0;
              enemy[temp].out:=true;
              enemy[temp].exploding:=false;
              enemy[temp].hitsneeded:=3;
              enemy[temp].dead:=false;
          end;
          saucersout:=5;
       end;
       if enemy[i].kind=15 then
       for temp:=3 to 4 do begin
           saucersout:=5;
           if (enemy[temp].out=false)
              and (enemy[temp-3].out=true) then begin
              enemy[temp].kind:=random(8)+3;
              enemy[temp].x:=enemy[temp-3].x;
              enemy[temp].y:=20;
              enemy[temp].xspeed:=0;
              enemy[temp].minx:=enemy[temp].x;
              enemy[temp].maxx:=enemy[temp].x;
              enemy[temp].boundarycheck:=false;
              enemy[temp].yspeed:=7;
              enemy[temp].out:=true;
              enemy[temp].exploding:=false;
              enemy[temp].hitsneeded:=1;
              enemy[temp].dead:=false;
           end;
       end;
    end;
{***READ KEYBOARD***}

    if keypressed then BEGIN
      ch:=readkey;
      if ch=chr(0) then ch2:=readkey;
      if ch2='M' then
         if shipadd>=0 then inc(shipadd,3) else shipadd:=0;
      if ch2='K' then
         if shipadd<=0 then dec(shipadd,3) else shipadd:=0;
      if ch2=';' then help;
      if ch2='<' then begin
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
      end;
      if (ch=' ') then begin
         if (bullet1out=false) then begin
            {if sbsound then StartSound(Sound[0], 0, false);}
            bullet1out:=true;
            bullet1x:=shipx+15;
            bullet1y:=165;
            putshape(shape1off,vaddr,18,17,bullet1x,bullet1y);
         end
         else
         if (bullet2out=false) then begin
            {if sbsound then StartSound(Sound[0], 0, false);}
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
    flip (vaddr,vga);
    waitretrace;
  until (ch=#27) or (shields<0);
END;
