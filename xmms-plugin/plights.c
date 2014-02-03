/*
 * plights - parallel light visualization plugin
 *
 * Copyright (C) 2006  Joshua D. Henderson
 *
 * Derived from work by:
 * Copyright (C) 1998-2000  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/*
 * This plugin is based on the default spectrum analyzer plugin, with a twist.
 * It groups the frequencies into 8 groups and turns the 8 parallel port outputs
 * on and off with the frequencies.
 */
#include "config.h"

#include <gtk/gtk.h>
#include <math.h>

#include "xmms/plugin.h"
#include "xmms/util.h"
#include "xmms_logo.xpm"
#include "i18n.h"
#include <sys/io.h>
#include <stdio.h>

#define NUM_BANDS 8

static GtkWidget *window = NULL,*area;
static GdkPixmap *bg_pixmap = NULL, *draw_pixmap = NULL, *bar = NULL;
static GdkGC *gc = NULL;
static gint16 bar_heights[NUM_BANDS];
static gint timeout_tag;
static gdouble scale;

static void sanalyzer_init(void);
static void sanalyzer_cleanup(void);
static void sanalyzer_playback_start(void);
static void sanalyzer_playback_stop(void);
static void sanalyzer_render_freq(gint16 data[2][256]);

/*
 * LP1 = 0x278,
 * LP2 = 0x378,
 * LP3 = 0x3BC
 */
static unsigned int pport = 0x378;

VisPlugin sanalyzer_vp =
   {
      NULL,
      NULL,
      0,
      NULL, /* Description */
      0,
      1,
      sanalyzer_init, /* init */
      sanalyzer_cleanup, /* cleanup */
      NULL, /* about */
      NULL, /* configure */
      NULL, /* disable_plugin */
      sanalyzer_playback_start, /* playback_start */
      sanalyzer_playback_stop, /* playback_stop */
      NULL, /* render_pcm */
      sanalyzer_render_freq  /* render_freq */
   };

VisPlugin *get_vplugin_info(void)
{
   sanalyzer_vp.description =
      g_strdup_printf("plight spectrum analyzer %s", VERSION);
   return &sanalyzer_vp;
}

#define WIDTH 250
#define HEIGHT 100


static void sanalyzer_destroy_cb(GtkWidget *w,gpointer data)
{
   sanalyzer_vp.disable_plugin(&sanalyzer_vp);
}

static void sanalyzer_init(void)
{
   GdkColor color;
   int i;
   if(window)
      return;
   window = gtk_window_new(GTK_WINDOW_DIALOG);
   gtk_window_set_title(GTK_WINDOW(window),_("plight analyzer"));
   gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
   gtk_widget_realize(window);
   bg_pixmap = gdk_pixmap_create_from_xpm_d(window->window,NULL,NULL,sanalyzer_xmms_logo_xpm);
   gdk_window_set_back_pixmap(window->window,bg_pixmap,0);
   gtk_signal_connect(GTK_OBJECT(window),"destroy",GTK_SIGNAL_FUNC(sanalyzer_destroy_cb),NULL);
   gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), &window);
   gtk_widget_set_usize(window, WIDTH, HEIGHT);
   gc = gdk_gc_new(window->window);
   draw_pixmap = gdk_pixmap_new(window->window,WIDTH,HEIGHT,gdk_rgb_get_visual()->depth);

   bar = gdk_pixmap_new(window->window,25, HEIGHT, gdk_rgb_get_visual()->depth);
   for(i = 0; i < HEIGHT / 2; i++)
   {
      color.red = 0xFFFF;
      color.green = ((i * 255) / (HEIGHT / 2)) << 8;
      color.blue = 0;

      gdk_color_alloc(gdk_colormap_get_system(),&color);
      gdk_gc_set_foreground(gc,&color);
      gdk_draw_line(bar,gc,0,i,24,i);
   }
   for(i = 0; i < HEIGHT / 2; i++)
   {
      color.red = (255 - ((i * 255) / (HEIGHT / 2))) <<8;
      color.green = 0xFFFF;
      color.blue = 0;

      gdk_color_alloc(gdk_colormap_get_system(),&color);
      gdk_gc_set_foreground(gc,&color);
      gdk_draw_line(bar,gc,0,i + (HEIGHT / 2),24,i + (HEIGHT / 2));
   }
   scale = HEIGHT / log(256);
   gdk_color_black(gdk_colormap_get_system(),&color);
   gdk_gc_set_foreground(gc,&color);

   area = gtk_drawing_area_new();
   gtk_container_add(GTK_CONTAINER(window),area);
   gtk_widget_realize(area);
   gdk_window_set_back_pixmap(area->window,bg_pixmap,0);

   gtk_widget_show(area);
   gtk_widget_show(window);
   gdk_window_clear(window->window);
   gdk_window_clear(area->window);
}

static void sanalyzer_cleanup(void)
{
   if(window)
   {
      gtk_widget_destroy(window);
   }
   if(gc)
   {
      gdk_gc_unref(gc);
      gc = NULL;
   }
   if(bg_pixmap)
   {
      gdk_pixmap_unref(bg_pixmap);
      bg_pixmap = NULL;
   }
   if(draw_pixmap)
   {
      gdk_pixmap_unref(draw_pixmap);
      draw_pixmap = NULL;
   }
   if(bar)
   {
      gdk_pixmap_unref(bar);
      bar = NULL;
   }
}

static gint draw_func(gpointer data)
{
   gint i;

   if(!window)
   {
      timeout_tag = 0;
      return FALSE;
   }

   GDK_THREADS_ENTER();
   gdk_draw_rectangle(draw_pixmap,gc,TRUE,0,0,WIDTH,HEIGHT);


   for(i = 0; i < NUM_BANDS; i++)
   {
      gdk_draw_pixmap(draw_pixmap,gc,bar, 0,HEIGHT - 1 - bar_heights[i], i * (WIDTH / NUM_BANDS), HEIGHT - 1 - bar_heights[i], (WIDTH / NUM_BANDS) - 1, bar_heights[i]);

   }
   gdk_window_clear(area->window);
   GDK_THREADS_LEAVE();

   return TRUE;
}

static void sanalyzer_playback_start(void)
{
   if(window)
   {
      gdk_window_set_back_pixmap(area->window,draw_pixmap,0);
      gdk_window_clear(area->window);
   }

   if (ioperm(pport, 1, 1))
   {
      printf("failed to init port\n");
   }
}


static void sanalyzer_playback_stop(void)
{
   if(GTK_WIDGET_REALIZED(area))
   {
      gdk_window_set_back_pixmap(area->window,bg_pixmap,0);
      gdk_window_clear(area->window);
   }

   ioperm(pport, 3, 0);
}

#define on(v,b) v = v | (1<<b)

#define off(v,b) v = v & ~(1<<b)

static void sanalyzer_render_freq(gint16 data[2][256])
{
   static unsigned int value = 0;
   gint i,c;
   gint y;

   //gint xscale[] = {0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255};
   //gint xscale[] = { 0, 32, 64, 96, 128, 160, 196, 224, 255};
   gint xscale[] =   { 0, 20, 32, 40,  64,  74,  96, 101, 255};

   if(!window)
      return;
   for(i = 0; i < NUM_BANDS; i++)
   {
      unsigned int nval = value;
      for(c = xscale[i], y = 0; c < xscale[i + 1]; c++)
      {
	 if(data[0][c] > y)
	    y = data[0][c];
      }
      y >>= 7;
      if(y != 0)
      {
	 y = (gint)(log(y) * scale);
	 if(y > HEIGHT - 1)
	    y = HEIGHT - 1;
      }

      if(y > bar_heights[i])
	 bar_heights[i] = y;
      else if(bar_heights[i] > 4)
	 bar_heights[i] -= 4;
      else
	 bar_heights[i] = 0;

      if (bar_heights[i] > 4)
      {
	 on(nval,i);
      }
      else
      {
	 off(nval,i);
      }

      if (value != nval)
      {
	 value = nval;
	 outb(~value,pport);
      }
   }
   draw_func(NULL);
   return;
}
