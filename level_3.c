#include <stdio.h>
#include <stdlib.h> /* For rand() */
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "./svmwgraph/svmwgraph.h"
#include "tb1_state.h"

#include "help.h"
#include "loadsave.h"
#include "graphic_tools.h"
#include "tblib.h"

#define NORTH 0
#define SOUTH 1
#define EAST  2
#define WEST  3

#define TOM_SHAPE 60

vmwSprite *shape_table[80];


void loadlevel3shapes(tb1_state *game_state) {

   
    int i,j;
   
    vmwLoadPicPacked(0,0,game_state->virtual_2,0,1,
		  tb1_data_file("level3/tblev3.tb1",
      	                      game_state->path_to_data),
		  game_state->graph_state);
   
    for(j=0;j<5;j++) {
       for(i=0;i<20;i++) {
	  shape_table[(j*20)+i]=vmwGetSprite(1+(i*11),1+(j*11),
					     10,10,game_state->virtual_2);
       }
    }
}

int our_sgn(int value) {
   
   if (value==0) return 0;
   if (value<0) return -1;
   return 1;
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
    int x_add=0,y_add=0;
    int game_paused=0;
   
    int tom_x=100,tom_y=100,walking=0;
    
   
    vmwVisual *virtual_1,*virtual_2;
    vmwFont *tb1_font;
   
    long oldsec,oldusec,time_spent;
   
    struct timeval timing_info;
    struct timezone dontcare;
    int speed_factor=0;
   
   
    virtual_1=game_state->virtual_1;
    virtual_2=game_state->virtual_2;
    tb1_font=game_state->graph_state->default_font;
    
    loadlevel3shapes(game_state);
/*  computer_0hits:=0;
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
  vmwTextXY(tempst,307,51,12,0,vaddr,false);

  cls(0,vaddr2);

  pal(250,0,0,63);
  pal(251,63,0,0);

  pal(254,shields*4,0,0);
  howmuchscroll:=50;

  cls(0,vga);
  coolbox(70,85,240,120,true,vga);
  vmwTextXY('   LEVEL THREE:',84,95,4,7,vga,false);
  vmwTextXY('  THE ALIEN SHIP',84,105,4,7,vga,false);

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
          vmwTextXY('You Have won!',5,5,9,7,vga,false);
          vmwTextXY('But as you can see this level is not done yet.',5,15,9,7,vga,false);
          {vmwTextXY('Anyway');}
          readln;
          inc(level);
        end
     else begin
          cls(0,vga);
          vmwTextXY('You Cannot Leave Yet',5,5,9,7,vga,true);
          repeat until keypressed; tempch:=readkey;
          levelover:=false;
          shipframe:=1;
          room:=254;
     end;
  end;
  if (room=254) and not(levelover) then goto newroom;

*/

    gettimeofday(&timing_info,&dontcare);
    oldsec=timing_info.tv_sec; oldusec=timing_info.tv_usec;
   

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


                 {vmwTextXY('You Fell In A Pit!',5,5,9,2,vga,false);}
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

               vmwTextXY('COMPUTER ACTIVATED:',47,30,2,0,vga,true);
               vmwSmallTextXY(' HUMAN YOU HAVE COME TOO SOON.  LEVELS 3',47,40,2,0,vga,true);
               vmwSmallTextXY('  AND 4 ARE INCOMPLETE.',47,48,2,0,vga,true);
               vmwSmallTextXY(' ANYWAY I CAN SEE YOU ARE NOT THE',47,58,2,0,vga,true);
               vmwSmallTextXY('  TENTACLEE COMMANDER.  YOU ARE IN',47,66,2,0,vga,true);
               vmwSmallTextXY('  GRAVE DANGER.  LUCKILY THE MAIN',47,74,2,0,vga,true);
               vmwSmallTextXY('  COMPUTER SYSTEM DOES NOT APPROVE',47,82,2,0,vga,true);
               vmwSmallTextXY('  OF THE TENTACLEE'S POLICIES.',47,90,2,0,vga,true);
               vmwSmallTextXY(' I PERSONALLY CANNOT SHUT OFF THE TRACTOR',47,100,2,0,vga,true);
               vmwSmallTextXY('  BEAM.  YOU MUST RETRIEVE FOUR KEYCARDS',47,108,2,0,vga,true);
               vmwSmallTextXY('  SCATTERED AROUND THE FLIGHT DECK.',47,116,2,0,vga,true);
               vmwSmallTextXY(' THE MAP BELOW WILL AID YOU.',47,124,2,0,vga,true);
              end;
              if computer_0hits=2 then begin

               vmwTextXY('COMPUTER ACTIVATED:',47,30,2,0,vga,true);
               vmwSmallTextXY(' HUMAN I HAVE ALREADY TOLD YOU MUCH.',47,40,2,0,vga,true);
               vmwSmallTextXY('  COLLECT THE 4 KEYCARDS, MADE OF',47,48,2,0,vga,true);
               vmwSmallTextXY('  RUBY, GOLD, EMERALD, AND ALUMINUM.',47,56,2,0,vga,true);
               vmwSmallTextXY(' WATCH OUT FOR ENEMIES NOT UNDER MY',47,66,2,0,vga,true);
               vmwSmallTextXY('  CONTROL, RADIOACTIVE FLOORS, AND',47,74,2,0,vga,true);
               vmwSmallTextXY('  EXTREMELY DEEP PITS.',47,82,2,0,vga,true);
              end;
          if computer_0hits>2 then begin
                vmwTextXY('COMPUTER ACTIVATED:',47,30,2,0,vga,true);
               vmwSmallTextXY(' HUMAN, GO AWAY.  YOU ANNOY ME.',47,40,2,0,vga,true);
               vmwSmallTextXY('  I HAVE TOLD YOU EVERYTHING.',47,48,2,0,vga,true);
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
           vmwPutSprite240(shape2array[35+explodeprogress],vaddr2,
                          20,9,x,y+howmuchscroll);
           if explodeprogress>4 then begin
              dead:=true;
              exploding:=false;
              vmwPutSprite240over(14800(*shape2array[34]*),vaddr2,
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
       if bullet1out then vmwPutSprite(shape3array[76],vaddr,10,9,bullet1x,bullet1y);
    end;
    if bullet2out then begin
       dec(bullet2y,5);
       if bullet2y<5 then bullet2out:=false;
       if bullet2out then vmwPutSprite(shape3array[76],vaddr,10,9,bullet2x,bullet2y);
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
           vmwPutSprite(shape2array[enemy[j].kind],vaddr,
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
            vmwPutSprite(shape3array[76],vaddr,10,9,bullet1x,bullet1y);
         end
         else
         if (bullet2out=false) then begin
            {if sbeffects then StartSound(Sound[4], 0, false);}
            bullet2out:=true;
            bullet2x:=shipx;
            bullet2y:=shipy;
            bullet2dir:=shipframe;
            vmwPutSprite(shape3array[76],vaddr,10,9,bullet2x,bullet2y);
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
	*/
        tom_x+=x_add;
        tom_y+=y_add;
        y_add-=our_sgn(y_add);
        x_add-=our_sgn(x_add);
       /*

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
	*/
       
       if ((!y_add) || (!x_add)) walking+=4;
       else walking=0;
       
       if (walking>12) walking=0;

          switch(direction) {
             case NORTH: vmwPutSprite(shape_table[TOM_SHAPE+walking],tom_x,tom_y,game_state->virtual_1);
	                 break;
             case EAST:  vmwPutSprite (shape_table[TOM_SHAPE+1+walking],tom_x,tom_y,game_state->virtual_1);
                         break;
	     case SOUTH: vmwPutSprite (shape_table[TOM_SHAPE+2+walking],tom_x,tom_y,game_state->virtual_1);
                         break;
	     case WEST:  vmwPutSprite (shape_table[TOM_SHAPE+3+walking],tom_x,tom_y,game_state->virtual_1);
	  }
       
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       
       
          /* Calculate how much time has passed */
       gettimeofday(&timing_info,&dontcare);
       time_spent=timing_info.tv_usec-oldusec;
       if (timing_info.tv_sec-oldsec) time_spent+=1000000;
#ifdef DEBUG_ON
          printf("%f\n",1000000/(float)time_spent);
#endif
              
          /* If time passed was too little, wait a bit */
       while (time_spent<33000){
	  gettimeofday(&timing_info,&dontcare);
	  usleep(5);
	  time_spent=timing_info.tv_usec-oldusec;
	  if (timing_info.tv_sec-oldsec) time_spent+=1000000;
       }
           
          /* It game is paused, don't keep track of time */
       if (game_paused) {
	  gettimeofday(&timing_info,&dontcare);
	  oldusec=timing_info.tv_usec;
	  oldsec=timing_info.tv_sec;
	  game_paused=0;
	  speed_factor=1;
       }
       else {
	  speed_factor=(time_spent/30000);
	  oldusec=timing_info.tv_usec;
	  oldsec=timing_info.tv_sec;
       }
       
    }
}



void LevelThreeLittleOpener(tb1_state *game_state) {
   
    int grapherror,i,j;
   
    vmwFont *tb1_font;
    vmwVisual *virtual_1,*virtual_2;
   
    int star_x[6];
    int star_y[6];
   
    tb1_font=game_state->graph_state->default_font;
    virtual_1=game_state->virtual_1;
    virtual_2=game_state->virtual_2;

    loadlevel3shapes(game_state);
    
    vmwClearScreen(game_state->virtual_1,0);
    grapherror=vmwLoadPicPacked(0,0,virtual_2,
			        1,1,tb1_data_file("level3/tbl3intr.tb1",
				game_state->path_to_data),game_state->graph_state);
    vmwArbitraryCrossBlit(game_state->virtual_2,0,3,171,114,
			  game_state->virtual_1,10,10);

    

    vmwPutSprite(shape_table[TOM_SHAPE],113,52,game_state->virtual_1);

    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
   
    vmwTextXY("Hmmmm... STUPID TRACTOR BEAM.",10,155,10,0,0,tb1_font,virtual_1);
    vmwTextXY("I GUESS I'D BETTER GO SHUT IT OFF.",10,165,10,0,0,tb1_font,virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
  
    pauseawhile(8);
    vmwClearKeyboardBuffer();

    for(i=24;i>=0;i--) {
      vmwArbitraryCrossBlit(game_state->virtual_2,0,3,171,114,
			    game_state->virtual_1,10,10);
      vmwPutSprite(shape_table[TOM_SHAPE+(4*(i% 4))],113,28+i,game_state->virtual_1);
      vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
      usleep(50000);
    }
    vmwPutSprite (shape_table[TOM_SHAPE],113,28,game_state->virtual_1);
    vmwDrawBox(10,155,290,30,0,game_state->virtual_1);
    vmwTextXY("I'M LUCKY I WORE MAGNETIC SHOES.",10,155,12,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("Hmmmm.  SOMEONE LEFT THE AIR-LOCK",10,165,12,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("        UNLOCKED.  STRANGE.",10,175,12,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(8);
    vmwClearKeyboardBuffer();
   
    vmwPutPixel(110,20,10,game_state->virtual_1);
    vmwPutPixel(110,22,10,game_state->virtual_1);
    vmwDrawBox(111,14,12,14,0,game_state->virtual_1);
    vmwDrawBox(10,155,290,30,0,game_state->virtual_1);
    vmwTextXY("I HOPE THIS ISN'T A TRAP.",10,155,9,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("I WISH I HAD SOME FORM OF ",10,165,9,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("        WEAPON.",10,175,9,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(7);
    vmwClearKeyboardBuffer();
  
    vmwClearScreen(game_state->virtual_1,0);
   
    vmwArbitraryCrossBlit(game_state->virtual_2,179,41,108,93,
			  game_state->virtual_1,10,10);
   
    for(i=0;i<6;i++) {
      star_x[i]=37+rand()%70;
      star_y[i]=18+rand()%56;
      vmwPutPixel(star_x[i],star_y[i],15,game_state->virtual_1);
    }

    vmwTextXY("WOW!! A GLASS-WALLED AIR-LOCK.",10,135,9,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5);
    vmwClearKeyboardBuffer();
    
    vmwDrawBox(10,135,290,50,0,game_state->virtual_1);
    vmwTextXY("NOW WHERE ARE WE GOING?",5,125,9,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("I GUESS THE PLANET EERM.",5,135,9,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("WHAT AN ODD NAME.",5,145,9,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("AND WHY AM I TALKING TO MYSELF?",5,155,10,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("ANYWAY I JUST WANT TO GO HOME",5,165,9,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("       AND SLEEP.",5,175,9,0,0,tb1_font,game_state->virtual_1);
    vmwClearKeyboardBuffer();
   
    j=0;
    while ((j<1000) && (!(vmwGetInput()))) {
       j++;
       for(i=0;i<6;i++) {
	  vmwPutPixel(star_x[i],star_y[i],0,game_state->virtual_1);
          star_x[i]++;
          if (star_x[i]>107) {
	     star_x[i]=37;
             star_y[i]=18+rand()%56;
	  }
          vmwPutPixel(star_x[i],star_y[i],15,game_state->virtual_1);
       }
       vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
       usleep(30000);
    }
   
    vmwClearKeyboardBuffer();    
}

