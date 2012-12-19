/* The following code is UGLY.  Read at your own risk.  --vmw */

#include <stdio.h>
#include <string.h> /* strncpy() */
#include <ctype.h>  /* isdigit() */
#include <fcntl.h>  /* open() */
#include <unistd.h> /* close() */

#include "lzss.h"

int write_ascii(char *string,int output_fd) {

    int pointer=0,count=0,octal=0;
    unsigned char byte;
   
    while(string[pointer]!='\"') pointer++;
    pointer++;

    while(string[pointer]!='\"') {
       if (string[pointer]=='\\') {
          pointer++;
	 
          if (isdigit(string[pointer])) {
             octal=0;
             while(isdigit(string[pointer])) {
                octal=8*octal+(string[pointer]-'0');
	        pointer++;
             }
	     if (octal>255) {
		fprintf(stderr,"BYTE OUT OF RANGE\n");
	     }
	     else {
		byte=octal;
	     }
	     pointer--;
          }
	 
          else {	      
	     switch(string[pointer]) {
	        case 'n': byte='\n'; break;
	        case 't': byte='\t'; break;
	        case '\\': byte='\\'; break; 	 
	        case '\"': byte='\"'; break;
	     }  
	  }
       }
       
       else byte=string[pointer];
       
       write(output_fd,&byte,1);
      
       pointer++;
       count++;
    }
   
   return count;
   
}

int get_number(char *string, int *pointer) {

    int temp_number;
 
    if (string[*pointer]=='0') {
       
          /* Hexadecimal */
       if (string[*pointer+1]=='x') {
	  (*pointer)++;
	  (*pointer)++;
	  temp_number=0;
	  while(isxdigit(string[*pointer])) {
	     if ((string[*pointer]>='a') && (string[*pointer]<='f')) 
		temp_number=16*temp_number+(10+(string[*pointer]-'a'));
	     if ((string[*pointer]>='A') && (string[*pointer]<='F')) 
		temp_number=16*temp_number+(10+(string[*pointer]-'A'));
	     if ((string[*pointer]>='0') && (string[*pointer]<='9')) 
		temp_number=16*temp_number+(string[*pointer]-'0');
	     (*pointer)++;
	  }
       }
       else {     
	  /* Octal */
          temp_number=0;
          while(isdigit(string[*pointer])) {
             temp_number=8*temp_number+(string[*pointer]-'0');
	     (*pointer)++;
	  }  
       }
       
    }
    else {
       /* Decimal */
       temp_number=0;
       while(isdigit(string[*pointer])) {
          temp_number=10*temp_number+(string[*pointer]-'0');
          (*pointer)++;
       }
    }
     
    return temp_number;
   
}

int write_bytes(char *string, int output_fd) {

    int pointer=0,count=0,number=0;
    unsigned char temp_byte; 
   
    while(1) {
       
       while(!isdigit(string[pointer])) {
	  if (string[pointer]=='\n') goto done_byte;
	  pointer++;
       }

       number=get_number(string,&pointer);
       
       if (number>255) {
	  fprintf(stderr,"Byte too big!\n");
       }
       else {
	  temp_byte=number;
	  write(output_fd,&temp_byte,1);
	  count++;
       }
  
    }
   
done_byte:
    return count;
   
}

int write_32bits(char *string, int output_fd) {

    int pointer=0,count=0,number=0;
//    unsigned char temp_int[4]; 
   
    while(1) {
       
       while(!isdigit(string[pointer])) {
	  if (string[pointer]=='\n') goto done_32;
	  pointer++;
       }

       number=get_number(string,&pointer);

       write(output_fd,&number,4);
       count+=4;
    }
done_32:  
    return count;
   
}

   


int main(int argc, char **argv) {

    FILE *input,*header,*labels,*output;
    int output_fd,byte_count;
    char input_filename[]="data.inc";
    char output_raw_filename[]="data.raw";
    char output_lzss_filename[]="data.lzss";
    char output_header_filename[]="data.header";
    char output_labels_filename[]="data.labels";
    int offset=0,pointer,temp_pointer;
    char input_line[BUFSIZ];
    char temp_label_string[BUFSIZ];    
   
    input=fopen(input_filename,"r");
    if (input==NULL) goto file_error;
   
    output_fd=open(output_raw_filename,O_WRONLY|O_CREAT|O_TRUNC,0666);
    if (output_fd<0) goto file_error;
   
    labels=fopen(output_labels_filename,"w");
    if (labels==NULL) goto file_error;
   
    while(1) {
	
       if ( fgets(input_line,BUFSIZ,input) ==NULL) goto close_file;
       
       pointer=0;
       
       while(pointer<strlen(input_line)) {
	     /* Comment, we are done */
	  if (input_line[pointer]=='#') goto done_with_string;
	     /* end of line, we are done */
	  if (input_line[pointer]=='\n') goto done_with_string;
	  
	       
	  
	     /* directive */
	  if (input_line[pointer]=='.') {
	     if (!strncmp("byte",input_line+pointer+1,4)) {
		offset+=write_bytes(input_line+pointer+1,output_fd);
		goto done_with_string;
	     }
	     
	     else if (!strncmp("ascii",input_line+pointer+1,5)) {
		offset+=write_ascii(input_line+pointer+1,output_fd);
		goto done_with_string;
	     }
	     
	     else if (!strncmp("long",input_line+pointer+1,4)) {
		offset+=write_32bits(input_line+pointer+1,output_fd);
		goto done_with_string;
	     }
	     else if (!strncmp("int",input_line+pointer+1,3)) {
		offset+=write_32bits(input_line+pointer+1,output_fd);
		offset++;
		goto done_with_string;
	     }
	     else {
		printf("Unknown directive!\n");
		goto close_file;
	     }
	     	     
	  }
	     /* end of label */
	  if (input_line[pointer]==':') {
	     temp_pointer=pointer;
	     while( (temp_pointer>0) &&
		    (input_line[temp_pointer]!='\t') &&
		    (input_line[temp_pointer]!=' ')) temp_pointer--;
	     
	     memcpy(temp_label_string,input_line+temp_pointer,
		     pointer-temp_pointer);
	     temp_label_string[pointer-temp_pointer]='\0';
	     fprintf(labels,".equ %s,%i+DATA_OFFSET\n",temp_label_string,offset);
	  }
	  
	       
	  
	  
	  pointer++;
       }
done_with_string:       ;
       
       
    }
   

   
close_file:   
    fclose(labels);
    close(output_fd);
    fclose(input);
    
    printf("Original size of data segment = %i bytes\n",offset);

    input=fopen(output_raw_filename,"r");
    if (input==NULL) goto file_error;
    
    output=fopen(output_lzss_filename,"w");
    if (output==NULL) goto file_error;
    
    header=fopen(output_header_filename,"w");
    if (header==NULL) goto file_error;

    byte_count=lzss_encode_better(input,header,output,'\0',1024,2);
   
    printf("Compressed data segment = %i bytes\n",byte_count);

    fprintf(header,".equ TB_DATA_SIZE,%i\n",offset);
    fclose(header);
    fclose(input);
    fclose(output);
      
file_error:   
    return 0;
}
