void savegame(int level,int begin_score,int begin_shields)
{
   
#if 0
    char ch;
    struct stat buf;
    char tempst[300],save_game_name[320],tempst2[5];
    FILE *fff;
    int dot_tb1_exists=0;
    
    int i,j;
   
    coolbox(0,0,319,199,1,vis);
    vmwTextXY("SAVE GAME",110,10,9],0],0,tb1_font,vis);
    if (read_only_mode) {
       vmwTextXY("SORRY!  CANNOT SAVE GAME",40,40,4],0],0,tb1_font,vis);
       vmwTextXY("WHEN IN READ ONLY MODE",50,50,4],0],0,tb1_font,vis);
    }
    else {
       sprintf(tempst,"%s/.tb1",getenv("HOME"));
       stat(tempst,&buf);
       if (!S_ISDIR(buf.st_mode)) {
	  vmwTextXY("DIRECTORY:",20,20,4],0],0,tb1_font,vis);
	  if (strlen(tempst)>18)
	     vmwTextXY("$HOME/.tb1",120,20,4],0],0,tb1_font,vis);
	  else 
	     vmwTextXY(tempst,120,20,4],0],0,tb1_font,vis);
	  vmwTextXY("WHERE TB1 STORES SAVED GAMES",20,30,4],0],0,tb1_font,vis);
	  vmwTextXY("DOES NOT EXIST.  DO YOU WANT",20,40,4],0],0,tb1_font,vis);
	  vmwTextXY("TO CREATE THIS DIRECTORY?",20,50,4],0],0,tb1_font,vis); 
	  vmwTextXY("   PLEASE ANSWER [Y or N]",20,60,4],0],0,tb1_font,vis);
	  vmwTextXY("NOTE IF YOU ANSWER \"N\" YOU",20,80,12],0],0,tb1_font,vis);
	  vmwTextXY("WILL NOT BE ABLE TO SAVE",20,90,12],0],0,tb1_font,vis);
	  vmwTextXY("GAMES",20,100,12],0],0,tb1_font,vis);
	  ch='M';
	  clear_keyboard_buffer();
	  while( (ch!='Y') && (ch!='y') && (ch!='N') && (ch!='n')) {
	     while((ch=get_input())==0) usleep(30);
	  }
	  if (toupper(ch)=='Y')
	     if (!mkdir(tempst,744)) dot_tb1_exists=1;
	     else {
		coolbox(0,0,319,199,1,vis);
		vmwTextXY("WARNING! COULD NOT CREATE",30,30,4],0],0,tb1_font,vis);
		vmwTextXY("DIRECTORY!  ABANDONING SAVE!",20,40,4],0],0,tb1_font,vis);	
	     }
	     
       }
       else dot_tb1_exists=1;
    }
    if (dot_tb1_exists) {
       coolbox(0,0,319,199,1,vis);
       vmwTextXY("SAVE GAME",110,20,9],0],0,tb1_font,vis);
       vmwTextXY("NOTE: THIS ONLY SAVES THE GAME",10,40,4],0],0,tb1_font,vis);
       vmwTextXY("AT THE BEGINNING OF THE LEVEL!",10,50,4],0],0,tb1_font,vis);
       vmwTextXY("ALREADY EXISTING GAMES",10,70,4],0],0,tb1_font,vis);
       j=0;
       for(i=0;i<10;i++) {
	  sprintf(save_game_name,"%s/sg%i.tb1",tempst,i);
	  if ((fff=fopen(save_game_name,"r"))!=NULL) {
	     sprintf(tempst2,"%i",i);
	     vmwTextXY(tempst2,50+j*20,80,12],0],0,tb1_font,vis);
	     fclose(fff);  
	     j++;
	  }
       }	  
       if (j==0) vmwTextXY("NONE",140,90,12],0],0,tb1_font,vis);
       vmwTextXY("PRESS NUMBER OF GAME TO SAVE",20,110,4],0],0,tb1_font,vis);
       vmwTextXY("(0-9) [ESC CANCELS]",60,120,4],0],0,tb1_font,vis);
       ch='M';
       clear_keyboard_buffer();
       while( ((ch<'0') || (ch>='9')) && (ch!=VMW_ESC)) {
	  while((ch=get_input())==0) usleep(30);
       }
       if (ch==VMW_ESC) {
	  coolbox(0,0,320,200,1,vis);
	  vmwTextXY("SAVE CANCELED",90,95,12],4],0,tb1_font,vis);
       }
       else {
	  sprintf(save_game_name,"%s/sg%c.tb1",tempst,ch);
	  if ((fff=fopen(save_game_name,"w"))!=NULL) {
	     fprintf(fff,"%i\n%i\n%i\n",level,begin_score,begin_shields);
	     fclose(fff);
	     coolbox(0,0,320,200,1,vis);
	     sprintf(tempst,"GAME %c SAVED",ch);
	     vmwTextXY(tempst,90,95,12],0],0,tb1_font,vis);
	  }
	  else {
	     coolbox(0,0,320,200,1,vis);
	     vmwTextXY("ERROR SAVING FILE!",70,90,12],0],0,tb1_font,vis);
             vmwTextXY("GAME NOT SAVED!",80,100,12],0],0,tb1_font,vis);
	  }
       }
    }
    vmwTextXY("PRESS ANY KEY...",80,180,4],0],0,tb1_font,vis);
    while( (ch=get_input())==0) usleep(30);
#endif
}
