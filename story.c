#include <stdio.h>

#include "svmwgraph/svmwgraph.h"
#include "tb1_state.h"

#include "tblib.h"
#include "sound.h"



void story(tb1_state *game_state)
{
    int error;
    int xtemp,ytemp;
    int thrustcol;
    float thrust;
    char tempch;
    int alienchar;
    int cycles;
   
    vmwFont *tb1_font;
   
    tb1_font=game_state->graph_state->default_font;

/*
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
*/

    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("tbsobj.tb1",game_state->path_to_data));
  /*
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
*/
   
       /******FIRST MESSAGE********/
    vmwClearScreen(game_state->virtual_1,0);
    vmwTextXY("THE STORY SO FAR...",20,20,4,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(7);

    vmwClearScreen(game_state->virtual_1,0);
   
    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("tbcobj.tb1",game_state->path_to_data));
		     
    vmwArbitraryCrossBlit(game_state->virtual_2,129,56,49,132,
			  game_state->virtual_1,10,10);
    vmwTextXY("YOU ARE TOM BOMBEM,  A STRANGE",80,10,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    BUT EFFICIENT MEMBER OF",80,20,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    THE LUNAR SPACE FORCE.",80,30,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU NEVER SAY MUCH AND YOU ARE",80,50,4,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    RARELY SEEN OUTSIDE OF",80,60,4,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    YOUR BLUE SPACESUIT.",80,70,4,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU OFTEN GET YOURSELF IN ",80,90,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    TROUBLE BY SCRATCHING",80,100,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    YOUR HEAD AT INAPPROPRIATE",80,110,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    TIMES.",80,120,2,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("PRESS ANY KEY....",96,185,15,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(15);

    vmwClearScreen(game_state->virtual_1,0);
      
    vmwArbitraryCrossBlit(game_state->virtual_2,129,56,49,132,
		        game_state->virtual_1,260,10);
    vmwArbitraryCrossBlit(game_state->virtual_2,99,104,29,81,
			game_state->virtual_1,287,13);

    vmwTextXY("IT IS THE YEAR 2028.",10,10,1,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU HAVE BEEN SUMMONED BY",10,30,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    LUNAR DICTATOR-IN-CHIEF",10,40,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    VINCENT WEAVER ABOUT A",10,50,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    TOP SECRET THREAT TO ",10,60,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    INTERPLANETARY SECURITY.",10,70,3,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("YOU ATTEND THE BRIEFING WITH",10,90,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    YOUR USUAL CONFUSED",10,100,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    QUIETNESS.  YOU STILL DO",10,110,5,0,0,tb1_font,game_state->virtual_1);  
    vmwTextXY("    NOT UNDERSTAND YOUR OWN",10,120,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("    SUCCESSFULNESS.",10,130,5,0,0,tb1_font,game_state->virtual_1);
    vmwTextXY("PRESS ANY KEY....",96,185,15,0,0,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(12);

    vmwLoadPicPacked(0,0,game_state->virtual_1,1,1,
		    tb1_data_file("tbchief.tb1",game_state->path_to_data));

      /* Save the area where the error will go */
    vmwArbitraryCrossBlit(game_state->virtual_1,115,55,91,59,
		          game_state->virtual_2,115,255);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
   
    vmwTextXY("Ahhh.... Mr. Bombem.... ",1,1,15,0,0,tb1_font,game_state->virtual_1);  
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
  if (game_state->sound_enabled) playGameFX(0);
    pauseawhile(2);
   
    if (game_state->sound_enabled) playGameFX(2);
      /* Show fake error message */
    vmwArbitraryCrossBlit(game_state->virtual_2,188,14,91,59,
			  game_state->virtual_1,115,55);
   
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(6);
   
      /* Restore background where error was */
    vmwArbitraryCrossBlit(game_state->virtual_2,115,255,91,59,
			  game_state->virtual_1,115,55);
 
    vmwTextXY("I'll be brief.                       ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5);
   
    vmwTextXY("Do you know how this base was founded?",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5);
   
    vmwTextXY("No?  Well watch the screen.             ",1,1,15,0,1,tb1_font,game_state->virtual_1);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(5); 
   
       /* Put picture on screen */
    vmwArbitraryCrossBlit(game_state->virtual_2,210,75,85,60,
			  game_state->virtual_1,210,136);
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(4);

       /******BARGE TAKING OFF***********/
   
    vmwLoadPicPacked(0,0,game_state->virtual_2,1,1,
		     tb1_data_file("tbma1.tb1",game_state->path_to_data));
 
    vmwSmallTextXY("MY WIFE AND I FOUNDED",212,3,14,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("THIS BASE IN 2008.",212,9,14,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("THE ONLY WAY TO ",212,16,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("FINANCE IT WAS TO",212,22,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("ENGAGE IN A DUBIOUS",212,28,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("BUSINESS.",212,34,13,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("WE LAUNCHED EARTH'S",212,41,12,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("TRASH INTO SPACE",212,47,12,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("FOR A PROFIT.",212,53,12,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("HERE IS FOOTAGE FROM",212,60,11,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("THE LAST LAUNCH EIGHT",212,66,11,0,0,tb1_font,game_state->virtual_2);
    vmwSmallTextXY("YEARS AGO.",212,72,11,0,0,tb1_font,game_state->virtual_2);
 
    vmwFlipVirtual(game_state->virtual_1,game_state->virtual_2,320,200);
   
/*  putshape(bargeoff,vaddr,16,18,141,157);*/
  
  /*  vmwCrossBlit(plb_vis->write,plb_vaddr->read,plb_vis->stride,200);*/
    vmwBlitMemToDisplay(game_state->graph_state,game_state->virtual_1);
    pauseawhile(7);

#if 0
/*   

 
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
*/
  pauseawhile(5);
  

       /****SECOND CHIEF*******/
   ggiSetGCForeground(vis,0]);
   ggiDrawBox(vis,0,0,320,200);   
   GGILoadPicPacked(0,0,vaddr2,1,1,
		    tb1_data_file("tbcobj.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
   GGILoadPicPacked(0,0,vis,1,1,
		    tb1_data_file("tbchief.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
/*    vmwArbitraryCrossBlit(plb_vaddr2->read,7,104,90,21,
		    plb_vis->write,6,174,plb_vis->stride,
		    stride_factor);
  */ 
    vmwTextXY("You might wonder why this is important.",1,1,15],0],1,tb1_font,virtual_1); 
    pauseawhile(6);
    vmwTextXY("Last week we received a message.       ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("It is of extra-terrestrial origin.     ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("Watch the screen.                      ",1,1,15],0],1,tb1_font,virtual_1);
    
    /*vmwArbitraryCrossBlit(plb_vaddr2->read,210,136,85,59,
			  plb_vis->write,210,136,plb_vis->stride,
			  stride_factor);*/
    pauseawhile(4);
   
       /******ALIEN DELEGATION*****/
    GGILoadPicPacked(0,0,vaddr2,1,1,
		    tb1_data_file("tbcrash.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
     
   /*alienchar:=34;
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
    */ 

       /****ALIEN MESSAGE*****/
    GGILoadPicPacked(0,0,vis,1,1,
		     tb1_data_file("tbgorg.tb1",(char *)tempst),
		     (ggi_color *)&eight_bit_pal,
		     (ggi_pixel *)&tb1_pal,color_depth);
    vmwTextXY("GREETINGS EARTHLINGS.",0,162,12],0],0,tb1_font,virtual_1);  
    vmwTextXY("I AM GORGONZOLA THE REPULSIVE.",0,171,12],0],0,tb1_font,virtual_1);
    vmwTextXY("YOU HAVE MADE A BIG MISTAKE.",0,180,12],0],0,tb1_font,virtual_1);
    pauseawhile(7);
    vmwTextXY("YOUR SHIP FULL OF REFUSE HAS",0,162,12],0],1,tb1_font,virtual_1);
    vmwTextXY("DAMAGED OUR OFFICIAL PEACE    ",0,171,12],0],1,tb1_font,virtual_1);
    vmwTextXY("ENVOY.  IT WAS ON ITS WAY TO ",0,180,12],0],1,tb1_font,virtual_1);
    vmwTextXY("YOUR PLANET.                  ",0,189,12],0],1,tb1_font,virtual_1);
    pauseawhile(7);
    vmwTextXY("IN AN IRONIC FORM OF RETALLIATION",0,162,12],0],1,tb1_font,virtual_1);
    vmwTextXY("WE HAVE MADE YOUR TRASH EVIL AND",0,171,12],0],1,tb1_font,virtual_1);
    vmwTextXY("TURNED IT AGAINST YOU.          ",0,180,12],0],1,tb1_font,virtual_1);
    vmwTextXY("        DIE EARTH SCUM!         ",0,189,12],0],1,tb1_font,virtual_1);
    pauseawhile(7);
 
       /****** THIRD CHIEF *******/
   GGILoadPicPacked(0,0,vaddr2,1,1,
		    tb1_data_file("tbcobj.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
   GGILoadPicPacked(0,0,virtual_1,1,1,
		    tb1_data_file("tbchief.tb1",(char *)tempst),
		    (ggi_color *)&eight_bit_pal,
		    (ggi_pixel *)&tb1_pal,color_depth);
    vmwArbitraryCrossBlit(plb_vaddr2->read,7,127,90,21,
		          plb_vis->write,6,174,plb_vis->stride,
		          stride_factor);
   
    vmwTextXY("Tom, our radar detects approaching ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("objects.  They are inside the      ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("orbit of Jupiter.                  ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("You are our only hope!             ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("Will you fly our only spaceship    ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("and save the human race?           ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwArbitraryCrossBlit(plb_vaddr2->read,5,16,39,82,
			  plb_vis->write,146,59,plb_vis->stride,
			  stride_factor);
    pauseawhile(1);
    vmwArbitraryCrossBlit(plb_vaddr2->read,46,16,39,82,
			  plb_vis->write,146,59,plb_vis->stride,
			  stride_factor);
    pauseawhile(1);
    vmwArbitraryCrossBlit(plb_vaddr2->read,87,16,39,82,
			  plb_vis->write,146,59,plb_vis->stride,
			  stride_factor);
    vmwTextXY("Scratch.  Scratch. <Ow that itches>",1,1,9],0],1,tb1_font,virtual_1);
    pauseawhile(5); 
    vmwTextXY("I knew you'd do it.  Good Luck!     ",1,1,15],0],1,tb1_font,virtual_1);
    pauseawhile(6);
    vmwTextXY("<Huh?>                             ",1,1,9],0],1,tb1_font,virtual_1);
    pauseawhile(10);
#endif
}
