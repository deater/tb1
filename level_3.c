#include <stdio.h>
#include <unistd.h>

#include "./svmwgraph/svmwgraph.h"
#include "tb1_state.h"

#include "help.h"
#include "loadsave.h"
#include "graphic_tools.h"

#define NORTH 0
#define SOUTH 1
#define EAST  2
#define WEST  3


void loadlevel3shapes() {
   /*
var x,y,i,j,shape:byte;
begin
   clearshape;
   grapherror:=loadpicsuperpacked(0,0,vaddr,false,true,'tblev3.tb1');
   
  for j:=0 to 3 do
    for i:=0 to 19 do
      for x:=0 to 9 do
          for y:=0 to 9 do
              ShapeTable1^[shape3array[(j*20)+i]+((y*10)+x)]
                          :=getpixel(1+x+(i*11),1+y+(j*11),vaddr);
  cls(0,vaddr);
end;
*/
}

/*
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
*/

void LevelThreeEngine(tb1_state *game_state) {

    int level_over=0;
    int ch,direction=NORTH;
    int x_add,y_add;
    int game_paused=0;
   
    vmwVisual *virtual_1,*virtual_2;
    vmwFont *tb1_font;
   
    virtual_1=game_state->virtual_1;
    virtual_2=game_state->virtual_2;
    tb1_font=game_state->graph_state->default_font;
    
/*

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

  flip(vaddr2,vaddr);
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

*/



/**** GAME LOOP ****/
/*******************/
  
    while (!level_over) {


       /***Collision Check***/
/*   if numpits>0 then
    for i:=0 to (numpits-1) do with pits[i] do
        if collision(shipx,shipy,5,4,px,py,4,4)
            then begin
                 levelover:=true;
                 cls(0,vga);
                 grapherror:=loadpicsuperpacked(0,0,vga,false,true,'tbpit.tb1');


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
               grapherror:=loadpicsuperpacked(0,0,vga,false,true,'tbconsol.tb1');
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
*/

       /***DO EXPLOSIONS***/
/*{    for i:=0 to 30 do
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
*/
 
       /***MOVE BULLETS***/
/*    if bullet1out then begin
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
 */
       /***MOVE ENEMIES***/
/*  {  for j:=0 to 30 do begin
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
*/
     
       /***READ KEYBOARD***/
    if ( (ch=vmwGetInput())!=0) {
       switch(ch){
	case VMW_ESCAPE: level_over=1;
	                 break;
	case VMW_RIGHT:  if (direction==EAST) x_add=2;
	                 else direction=EAST;
	                 break;
	  /*
         if (shipframe=2) and (shipxadd=0) then shipxadd:=2
         else if (shipframe<>2) then shipframe:=2
         else inc(shipxadd);
         */
	case VMW_LEFT: if (direction==WEST) x_add=-2;
	               else direction=WEST;
	               break;
	case VMW_UP:       if (direction==NORTH) y_add=-2;
	               else direction=NORTH;
	               break;
	case VMW_DOWN: if (direction==SOUTH) y_add=2;
	               else direction=SOUTH;
	               break;
	case VMW_F1:   game_paused=1;
	               help(game_state);
	               break;
	  
        case 'P':
	case 'p':      game_paused=1;
                       coolbox(65,85,175,110,1,virtual_1);
                       vmwTextXY("GAME PAUSED",79,95,4,7,0,
				 game_state->graph_state->default_font,
				 virtual_1);
                       vmwClearKeyboardBuffer();
	               vmwBlitMemToDisplay(game_state->graph_state,
					   virtual_1);
	               while (vmwGetInput()==0) usleep(30000); 
	               break;
	case 'S':
	case 's':      if (game_state->sound_possible)
	                  game_state->sound_enabled=!(game_state->sound_enabled);
                       break;
	  
	case VMW_F2:   game_paused=1;
	               savegame(game_state);
	               break;
	case ' ':      /* shoot */
	               break;
	  /*
	  
      if (ch=' ') and havegun then begin
         if (bullet1out=false) then begin
            {if sbeffects then StartSound(Sound[4], 0, false);}
            bullet1out:=true;
            bullet1x:=shipx+3;
            bullet1y:=shipy+4;
            bullet1dir:=shipframe;
            putshape(shape3array[76],vaddr,10,9,bullet1x,bullet1y);
         end
         else
         if (bullet2out=false) then begin
            {if sbeffects then StartSound(Sound[4], 0, false);}
            bullet2out:=true;
            bullet2x:=shipx;
            bullet2y:=shipy;
            bullet2dir:=shipframe;
            putshape(shape3array[76],vaddr,10,9,bullet2x,bullet2y);
       end;
    end;
*/
       }
    }
       
       /***MOVE TOM***/
        

     /*   ucollide:=upcollide(shipx,shipy,abs(shipyadd),-abs(shipyadd),10,vaddr2);
        dcollide:=upcollide(shipx,shipy,abs(shipyadd),8,10,vaddr2);
        lcollide:=leftcollide(shipx,shipy,abs(shipxadd),-abs(shipxadd),8,vaddr2);
        rcollide:=leftcollide(shipx,shipy,abs(shipxadd),0,8,vaddr2);*/
       
       /* if (shipframe=1) and (ucollide<>0) then shipyadd:=0;
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
*/
     /*   CASE shipframe of
           1   : putshape (shape3array[60+walking],vaddr,10,9,shipx,shipy);
           2   : putshape (shape3array[61+walking],vaddr,10,9,shipx,shipy);
           3   : putshape (shape3array[62+walking],vaddr,10,9,shipx,shipy);
           4   : putshape (shape3array[63+walking],vaddr,10,9,shipx,shipy);
        END;
  */
    }
}



void littleopener3() {
   /*
var star_x:array[0..5]of integer;
    star_y:array[0..5]of integer;
begin
  loadlevel3shapes;
  grapherror:=loadpicsuperpacked(0,0,vga,true,false,'tbl2ship.tb1');
  fade;
  cls(0,vga);
  grapherror:=loadpicsuperpacked(0,0,vaddr,false,true,'tbl3intr.tb1');
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
    */
}

