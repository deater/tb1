/* ppp.c GIMP plug-in for loading & saving ppp files */
/* by Vince Weaver <vince@deater.net> */

/* This code is based in parts on code by Francisco Bustamante,
   and Nick Lamb njl195@zepler.org.uk */

/* To compile, copy into the gimp source directory and do a */
/*        gimptool --build paintpro.c    */
/* Then copy the resulting paintpro file into ~/.gimp-1.2/plug-ins */

/*#define DEBUG 1 */


#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "libgimp/stdplugins-intl.h"


/* Declare plug-in functions.  */

static void query (void);
static void run   (gchar   *name, 
		   gint     nparams, 
		   GimpParam  *param, 
		   gint    *nreturn_vals,
		   GimpParam **return_vals);

#if G_BYTE_ORDER == G_BIG_ENDIAN
#define qtohl(x) \
        ((unsigned long int)((((unsigned long int)(x) & 0x000000ffU) << 24) | \
                             (((unsigned long int)(x) & 0x0000ff00U) <<  8) | \
                             (((unsigned long int)(x) & 0x00ff0000U) >>  8) | \
                             (((unsigned long int)(x) & 0xff000000U) >> 24)))
#define qtohs(x) \
        ((unsigned short int)((((unsigned short int)(x) & 0x00ff) << 8) | \
                              (((unsigned short int)(x) & 0xff00) >> 8)))
#else
#define qtohl(x) (x)
#define qtohs(x) (x)
#endif
#define htoql(x) qtohl(x)
#define htoqs(x) qtohs(x)

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,  /* init_proc  */
  NULL,  /* quit_proc  */
  query, /* query_proc */
  run,   /* run_proc   */
};

MAIN ()

static void
query (void)
{
  static GimpParamDef load_args[] =
  {
    { GIMP_PDB_INT32, "run_mode", "Interactive, non-interactive" },
    { GIMP_PDB_STRING, "filename", "The name of the file to load" },
    { GIMP_PDB_STRING, "raw_filename", "The name entered" }
  };
  static GimpParamDef load_return_vals[] =
  {
    { GIMP_PDB_IMAGE, "image", "Output image" }
  };
  static gint nload_args = sizeof (load_args) / sizeof (load_args[0]);
  static gint nload_return_vals = (sizeof (load_return_vals) /
				   sizeof (load_return_vals[0]));

  static GimpParamDef save_args[] =
  {
    { GIMP_PDB_INT32, "run_mode", "Interactive, non-interactive" },
    { GIMP_PDB_IMAGE, "image", "Input image" },
    { GIMP_PDB_DRAWABLE, "drawable", "Drawable to save" },
    { GIMP_PDB_STRING, "filename", "The name of the file to save the image in" },
    { GIMP_PDB_STRING, "raw_filename", "The name entered" }
  };
  static gint nsave_args = sizeof (save_args) / sizeof (save_args[0]);

  gimp_install_procedure ("file_ppp_load",
                          "Loads files in VMW Paintpro file format",
			  "Paintpro",
			  "Vince Weaver",
			  "vince@deater.net",
			  "2001",
                          "<Load>/PPP",
			  NULL,
                          GIMP_PLUGIN,
                          nload_args, nload_return_vals,
                          load_args, load_return_vals);

  gimp_install_procedure ("file_ppp_save",
                          "Saves files in VMW Paintprofile format",
			  "Paintpro",
			  "Vince Weaver",
			  "vince@deater.net",
			  "2001",
                          "<Save>/PPP",
			  "INDEXED, RGB, GRAY",
                          GIMP_PLUGIN,
                          nsave_args, 0,
                          save_args, NULL);

  gimp_register_magic_load_handler ("file_ppp_load",
				    "ppp,amg",
				    "",
				    "0,string,PAINTPROV");
  gimp_register_save_handler       ("file_ppp_save",
				    "ppp,amg",
				    "");
}

/* Declare internal functions. */

static gint32 load_image (gchar  *filename);

static void   load_8     (FILE   *fp,
			  gint    width,
			  gint    height,
			  gchar  *buffer);

static gint   save_image (gchar  *filename,
			  gint32  image,
			  gint32  layer);
static void   save_8     (FILE   *fp,
			  gint    width,
			  gint    height,
			  guchar *buffer);


   /* Take a 24bit value [3 bytes] and split the top and bottom */
   /* 12 bits into 2 integers */
#define THREE_BYTES_INT1(a,b)   ( ( ( (int)(a))<<4)+ ((b)>>4) )
#define THREE_BYTES_INT2(b,c)   ( ( ( ( (int)(b))&0x0f)<<8)+ (c))




/* Plug-in implementation */

static void
run (gchar   *name, 
     gint     nparams, 
     GimpParam  *param, 
     gint    *nreturn_vals,
     GimpParam **return_vals) 
{
  static GimpParam values[2];
  GimpRunModeType  run_mode;
  GimpPDBStatusType   status = GIMP_PDB_SUCCESS;
  gint32        image_ID;
  gint32        drawable_ID;
  GimpExportReturnType export = GIMP_EXPORT_CANCEL;

  run_mode = param[0].data.d_int32;

  *nreturn_vals = 1;
  *return_vals  = values;
  values[0].type          = GIMP_PDB_STATUS;
  values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;

  if (strcmp (name, "file_ppp_load") == 0)
    {
      INIT_I18N();
      image_ID = load_image (param[1].data.d_string);

      if (image_ID != -1)
	{
	  *nreturn_vals = 2;
	  values[1].type = GIMP_PDB_IMAGE;
	  values[1].data.d_image = image_ID;
	}
      else
	{
	  status = GIMP_PDB_EXECUTION_ERROR;
	}
    }
  else if (strcmp (name, "file_ppp_save") == 0)
    {
      image_ID    = param[1].data.d_int32;
      drawable_ID = param[2].data.d_int32;

      /*  eventually export the image */ 
      switch (run_mode)
	{
	case GIMP_RUN_INTERACTIVE:
	case GIMP_RUN_WITH_LAST_VALS:
	  INIT_I18N_UI();
	  gimp_ui_init ("ppp", FALSE);
	  export = gimp_export_image (&image_ID, &drawable_ID, "ppp", 
				      (GIMP_EXPORT_CAN_HANDLE_RGB |
				       GIMP_EXPORT_CAN_HANDLE_GRAY |
				       GIMP_EXPORT_CAN_HANDLE_INDEXED));
	  if (export == GIMP_EXPORT_CANCEL)
	    {
	      values[0].data.d_status = GIMP_PDB_CANCEL;
	      return;
	    }
	  break;
	default:
	  INIT_I18N();
	  break;
	}

      switch (run_mode)
	{
	case GIMP_RUN_INTERACTIVE:
	  break;

	case GIMP_RUN_NONINTERACTIVE:
	  if (nparams != 5)
	    status = GIMP_PDB_CALLING_ERROR;
	  break;

	case GIMP_RUN_WITH_LAST_VALS:
	  break;

	default:
	  break;
	}

      if (status == GIMP_PDB_SUCCESS)
	{
	  if (! save_image (param[3].data.d_string, image_ID, drawable_ID))
	    {
	      status = GIMP_PDB_EXECUTION_ERROR;
	    }
	}
      
      if (export == GIMP_EXPORT_EXPORT)
	gimp_image_delete (image_ID);
    }
  else
    {
      status = GIMP_PDB_CALLING_ERROR;
    }

  values[0].data.d_status = status;
}

guchar mono[6]= { 0, 0, 0, 255, 255, 255 };

static gint32 load_image (gchar *filename)  {
   
  FILE *fd;
  GimpDrawable *drawable;
  GimpPixelRgn pixel_rgn;
  gchar *message;
  gint offset_x, offset_y, height, width;
  gint32 image, layer;
  guchar *dest, cmap[768];

  unsigned char buffer[18];
  int num_colors,i;
   
  message = g_strdup_printf (_("Loading %s:"), filename);
  gimp_progress_init (message);
  g_free (message);

  fd = fopen (filename, "rb");
  
    if (!fd) {
       g_message ("ppp: Can't open\n%s", filename);
       return -1;
    }

    if (fread (buffer,1,18, fd) != 18) {
       g_message ("ppp: Can't read header from\n%s", filename);
       return -1;
    }
#ifdef DEBUG
    for(i=0;i<11;i++) putchar(buffer[i]);
    putchar('\n');
#endif   
    offset_x=0;
    offset_y=0;
    width = THREE_BYTES_INT1(buffer[12],buffer[13]);
    height = THREE_BYTES_INT2(buffer[13],buffer[14]);
    num_colors=THREE_BYTES_INT1(buffer[15],buffer[16]);

#ifdef DEBUG
    printf("Loading %ix%i file with %i colors\n",width,height,num_colors);
#endif
   
   
    if (num_colors>256) {
       g_message("ppp: only <=256 color ppp images supported!\n");
       return FALSE;
    }
   
   
    image= gimp_image_new (width, height, GIMP_INDEXED);
    layer= gimp_layer_new (image, _("Background"), width, height,
			     GIMP_INDEXED_IMAGE, 100, GIMP_NORMAL_MODE);
   
    gimp_image_set_filename (image, filename);
    gimp_image_add_layer (image, layer, 0);
    gimp_layer_set_offsets (layer, offset_x, offset_y);
    drawable = gimp_drawable_get (layer);

       /* load color map */
   
    for(i=0;i<num_colors;i++) {   
       fread(&cmap[i*3],3,1,fd);
    }
    gimp_image_set_cmap (image, cmap, num_colors);
   
       /* load picture */
    dest = (guchar *) g_malloc (width * height);
    load_8(fd, width, height, dest);

    gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0, width, height, TRUE, FALSE);
    gimp_pixel_rgn_set_rect (&pixel_rgn, dest, 0, 0, width, height);

    g_free (dest);

    gimp_drawable_flush (drawable);
    gimp_drawable_detach (drawable);

    return image;
}

static void load_8 (FILE  *fp, gint   width, gint   height, gchar *buffer) {
  

  int x=0,y=0; 
  unsigned char three_bytes[3];
  int color,numacross,i;
   
  while (y<height) {
     fread(three_bytes,3,1,fp);
     color=THREE_BYTES_INT1(three_bytes[0],three_bytes[1]);
     numacross=THREE_BYTES_INT2(three_bytes[1],three_bytes[2]);

#ifdef DEBUG
     printf("%i %i\t",color,numacross); fflush(stdout);
#endif
     
     if (color>2047) {
	buffer[y*width+x]=(unsigned char) (color-2048);
	x++;
	if (x>=width) {
	   x=0; y++;
	}
	buffer[y*width+x]=(unsigned char) (numacross-2048);
	x++;
	if (x>=width) {
	   x=0; y++;
	}
     }
     else {
	while ((x+numacross)>=(width)) {
	   for(i=x;i<width;i++) {
	      buffer[y*width+i]=(unsigned char)color;
	   }
	   numacross=numacross-(width-x);
	
	   x=0;
	   y++;
	}
	
	if (numacross!=0) {
	   if (y>=height) {
	      printf("trying to draw past end of buffer\n");
	   }
	   if (numacross > width) {
	      printf("x too big\n");
	   }
	   
	   for(i=0;i<numacross;i++) {
	      buffer[y*width+x+i]=(unsigned char)color;
	   }
	   x+=numacross;
	}
     }
  }
	   
}

#define TWO_INTS_THREE_BYTES(a,b)  \
        three_bytes[0]=((unsigned char)(((a)>>4)&0xff)); \
        three_bytes[1]=((unsigned char) ( (((a)&0x0f)<<4)+(((b)>>8)&0x0f))); \
        three_bytes[2]=((unsigned char)((b)&0xff));


static gint save_image (gchar *filename, gint32  image, gint32  layer)  {
  
    FILE *fp;
    GimpPixelRgn pixel_rgn;
    GimpDrawable *drawable;
    GimpImageType drawable_type;
    guchar *cmap= 0, *pixels;
    gint offset_x, offset_y, width, height;
    gchar *message;
    int colors;
    unsigned char three_bytes[3];
   
    char ppro_string[]="PAINTPRO";
    char ppro_version[]="V6.1";
   
   
    drawable = gimp_drawable_get (layer);
    drawable_type = gimp_drawable_type (layer);
    gimp_drawable_offsets (layer, &offset_x, &offset_y);
    width = drawable->width;
    height = drawable->height;
    gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0, width, height, FALSE, FALSE);

    message = g_strdup_printf (_("Saving %s:"), filename);
    gimp_progress_init (message);
    g_free (message);



   
    switch (drawable_type) {
    
       case GIMP_INDEXED_IMAGE:
          cmap = gimp_image_get_cmap (image, &colors);
#ifdef DEBUG
          printf("Saving picture %ix%i with %i colors\n",width,height,colors);
#endif
          break;

       default:
          g_message ("ppp Can't save this image type\nFlatten your image");
          return FALSE;
          break;
    }
   

    if ((fp = fopen(filename, "wb")) == NULL) {
       g_message ("ppp Can't open \n%s", filename);
       return FALSE;
    }

    fwrite(ppro_string,8,1,fp);
    fwrite(ppro_version,4,1,fp);
   
    pixels = (guchar *) g_malloc (width * height);
    gimp_pixel_rgn_get_rect (&pixel_rgn, pixels, 0, 0, width, height);

    TWO_INTS_THREE_BYTES(width,height);
    fwrite(three_bytes,3,1,fp);
   
    TWO_INTS_THREE_BYTES(colors,0);
    fwrite(three_bytes,3,1,fp);
   

    switch (drawable_type) {
       case GIMP_INDEXED_IMAGE:
            fwrite (cmap, colors, 3, fp);
            save_8 (fp, width, height, pixels);
            break;
       default:
          g_message ("Can't save this image as ppp\nFlatten your image");
          return FALSE;
          break;
    }

    gimp_drawable_detach (drawable);
    g_free (pixels);

    fclose (fp);
    return TRUE;
}

static void save_8 (FILE *fp, gint width, gint height, guchar *buffer) {
  
   int oldcolor,x=0,y=0,color=0,numacross=0;
   int already_have_single_pixel=0;
   unsigned char three_bytes[3],save_pixel=0;
   
   oldcolor=buffer[0];
   
    while(y<height) {
       color=buffer[y*width+x];
       if ((color==oldcolor)&&(numacross<2046)) numacross++;
       else {
	  if (numacross==1) {
	     if (!already_have_single_pixel) {
		save_pixel=oldcolor;
		already_have_single_pixel=1;
	     }
	     else {
		TWO_INTS_THREE_BYTES(save_pixel+2048,oldcolor+2048);
		fwrite(three_bytes,3,1,fp);
		already_have_single_pixel=0;
	     }
	  }
	  else {
	     if (already_have_single_pixel) {
		TWO_INTS_THREE_BYTES(save_pixel,1);
		fwrite(three_bytes,3,1,fp);
		already_have_single_pixel=0;
	     }
	     TWO_INTS_THREE_BYTES(oldcolor,numacross);
	     fwrite(three_bytes,3,1,fp);
	  }   
	  oldcolor=color;
	  numacross=1;
       }
       x++;
       if (x>=width) {
	  x=0;
	  y++;
       }
    }
       
    if (already_have_single_pixel) {
       TWO_INTS_THREE_BYTES(save_pixel,1);
       fwrite(three_bytes,3,1,fp);
    }
       
    TWO_INTS_THREE_BYTES(color,numacross);
    fwrite(three_bytes,3,1,fp);
}
