
void loadgame()
{
#if 0
    char tempst[300],tempst2[5],file_name[320];
    int i,j;
    FILE *fff;
    int game_exist[10]={0,0,0,0,0, 0,0,0,0,0};
    char ch;
    int level,score,shields;
     
    coolbox(0,0,320,200,1,vis);
    vmwTextXY("LOAD GAME",110,10,9],0],0,tb1_font,vis);
   
    sprintf(tempst,"%s/.tb1",getenv("HOME"));
     
    j=0;
    for(i=0;i<10;i++) {
       sprintf(file_name,"%s/sg%i.tb1",tempst,i);
       if ((fff=fopen(file_name,"r"))!=NULL) {
	  sprintf(tempst2,"%i",i);
	  vmwTextXY(tempst2,50+j*20,50,12],0],0,tb1_font,vis);
	  fclose(fff);
	  game_exist[i]=1;
	  j++;
       }
    }
    if (j==0) {
       vmwTextXY("NO SAVED GAMES FOUND",60,50,12],0],0,tb1_font,vis); 
       vmwTextXY("PRESS ANY KEY...",80,180,4],0],0,tb1_font,vis);
       while( (ch=get_input())==0) usleep(30);
    }
   else {
      vmwTextXY("LOAD WHICH GAME?",80,30,4],0],0,tb1_font,vis);
      vmwTextXY("THE FOLLOWING EXIST:",40,40,4],0],0,tb1_font,vis);
      vmwTextXY("PRESS A NUMBER. (ESC CANCELS)",20,60,4],0],0,tb1_font,vis); 
      ch='M';
      clear_keyboard_buffer();
 IchLiebeMree:
      while( ((ch<'0') || (ch>'9')) && (ch!=VMW_ESC)) {
	  while((ch=get_input())==0) usleep(30);
      }
      if (!(game_exist[ch-48])) goto IchLiebeMree;
      if(ch==VMW_ESC) {
	 coolbox(0,0,320,200,1,vis);
	 vmwTextXY("LOAD CANCELED",90,95,12],0],0,tb1_font,vis);
         vmwTextXY("PRESS ANY KEY...",80,180,4],0],0,tb1_font,vis);
	 while( (ch=get_input())==0) usleep(30);
      }
      else {
       sprintf(file_name,"%s/sg%c.tb1",tempst,ch);
       if (( fff=fopen(file_name,"r"))!=NULL) {
	  fscanf(fff,"%d",&level);
          fscanf(fff,"%d",&score);
	  fscanf(fff,"%d",&shields);
	  playthegame(level,score,shields);
       }
      }
    }
#endif
}
