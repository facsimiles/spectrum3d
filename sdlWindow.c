/* This file is part of SpectrumAnalyzer.

    SpectrumAnalyzer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SpectrumAnalyzer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpectrumAnalyzer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <gst/gst.h>
#include <string.h>
#include <gtk/gtk.h>
#include <SDL/SDL.h>
#include <jack/jack.h>

gint64 pos;
static GtkWidget *mainWindow;
GstElement *pipeline, *audio;
#include "functions.h"

static void
cb_newpad (GstElement *decodebin,
	   GstPad     *pad,
	   gboolean    last,
	   gpointer    data)
{  /* auto-decode audio file (decodebin) */
  GstCaps *caps;
  GstStructure *str;
  GstPad *audiopad;

  audiopad = gst_element_get_static_pad (audio, "sink");
  if (GST_PAD_IS_LINKED (audiopad)) {
    g_object_unref (audiopad);
    return;
  }

  caps = gst_pad_get_caps (pad);
  str = gst_caps_get_structure (caps, 0);
  if (!g_strrstr (gst_structure_get_name (str), "audio")) {
    gst_caps_unref (caps);
    gst_object_unref (audiopad);
    return;
  }
  gst_caps_unref (caps);

  gst_pad_link (pad, audiopad);
  g_object_unref (audiopad);
}

static gboolean
message_handler (GstBus * bus, GstMessage * message, gpointer data)
/* Here the results of the "spectrum" function of gstreamer (amplitude of each frequency) are obtained and plotted in a SDL surface */
{ 
    int x = 0, pix = 0, y = 0; 
    SDL_Event event, event_pause;
    SDL_Rect positionBlackScreen;
    positionBlackScreen.x = 0;
    positionBlackScreen.y = 0;
    
    SDL_PollEvent(&event);
    switch(event.type)
    { /* some keyboard shortcuts for the SDL surface */
		case SDL_QUIT:
			if (loop != NULL){
			g_main_loop_quit(loop);
			}
		  SDL_Quit();
		  playing = 0;
		break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
                case SDLK_ESCAPE:
		        g_main_loop_quit(loop);
		        playing = 0;
		        break;
                case SDLK_SPACE:
			if (pose == 0) {
			pose = 1;
			}
			else {
			pose = 0;
			}
		        break;
            	}
	break;
    }
	    counter ++;
            if (message->type == GST_MESSAGE_ELEMENT && pose == 0 && counter > counterNumber)
            {
		counter = 0;
                const GstStructure *s = gst_message_get_structure (message);
                const gchar *name = gst_structure_get_name (s);
                GstClockTime endtime;

                    if (strcmp (name, "spectrum") == 0)
                    {
                      const GValue *magnitudes, *mag;
                      guint i = 0, a = 0, b = 1;
                      float amplitude = 0;
			if (display == 0) { /* increase image of the higher frequencies, only in continous mode */
			      switch (range)
				{
				case 20 :
				b = 6;
				break;
				case 10 :
				b = 5;
				break;
				default :
				b = 4;
				break;
				}
			}

                      if (!gst_structure_get_clock_time (s, "endtime", &endtime))
                      endtime = GST_CLOCK_TIME_NONE;
                      magnitudes = gst_structure_get_value (s, "magnitude");

		      if (display) // when swithching from diplay 0 to 1, the SDL surface has to be "refreshed"
                      	SDL_BlitSurface(blackScreen, NULL, screen, &positionBlackScreen);

      		      SDL_LockSurface(screen);
		      if (display == 0){  
/* When display = 0, it displays the values for the "Continuous" mode, ie frequency vs time; amplitude is roughly approximated by the intensity of red; each new date is plotted on the right of the screen and progress towards the left*/
			      for (i = zoom; i < (HEIGHT_WINDOW + zoom) ; i++)
				    {
				    amplitude = 0;
				    for (a = 0; a < range; a++)
					{ /* utility of the 'range' factor : for example, if gstreamer gets 10000 frequency values that has to be displayed on a 500 pixels-height SDL surface, in place of taking and displaying one frequency value every 20 values, we'll take and display the mean of 20 frequency values; this allows to be much more precise */ 
					mag = gst_value_list_get_value (magnitudes, ((i * range) + a));
					amplitude = amplitude + ((char)g_value_get_float(mag)+80);
					}
					amplitude = amplitude/range;
				if (mag != NULL) {
						setPixel(screen, (WIDTH_WINDOW),(HEIGHT_WINDOW-1-(i-zoom)), SDL_MapRGB(screen->format,((amplitude)*b), 0, 0));
						/* if nCol == n, each new data is plotted on a width of n pixel on the SDL surface; the more nCol, the more the speed of display is fast */
						if (nCol == 2 || nCol == 3){ 
							setPixel(screen, (WIDTH_WINDOW - 1),(HEIGHT_WINDOW-1-(i-zoom)), SDL_MapRGB(screen->format,((amplitude)*b), 0, 0));
							if (nCol == 3){
							setPixel(screen, (WIDTH_WINDOW - 2),(HEIGHT_WINDOW-1-(i-zoom)), SDL_MapRGB(screen->format,((amplitude)*b), 0, 0));
								}
							}
						}
						for (x = 1; x < (WIDTH_WINDOW); x+=nCol) {
						    pix = getPIxel(screen, x+1, i-zoom);
						    setPixel(screen, x, i-zoom, pix);
							if (nCol == 2 || nCol == 3){
						    		setPixel(screen, x - 1, i-zoom, pix);
								if (nCol == 3){
						    		setPixel(screen, x - 2, i-zoom, pix);
									}
							}
						}				     
				     }
		        }
/* When display = 1, it displays the values for the "Snapshot" mode, ie amplitude vs frequency; the SDL surface is refreshed for each new data */
			else {
			       for (i = zoom; i < (WIDTH_WINDOW + zoom) ; i++)
				    {
				    amplitude = 0;
				    for (a = 0; a < range; a++)
					{
					mag = gst_value_list_get_value (magnitudes, ((i * range) + a));
					amplitude = amplitude + ((char)g_value_get_float(mag)+80);
					}
				    amplitude = amplitude/range;					    						     
				     if (mag != NULL)
					{
					for (y = HEIGHT_WINDOW -1; y >= (HEIGHT_WINDOW-1-(amplitude * 6)); y--){
						setPixel(screen, i - zoom , y, SDL_MapRGB(screen->format,255, 0, 0));
						}
					}						   
				     }
			      }
		      SDL_UnlockSurface(screen);
		      SDL_Flip(screen);
		}
            } 
return TRUE;
SDL_FreeSurface(blackScreen);
}

static gboolean cb_print_position (GstElement *pipeline)
{ /* print in a terminal the total time and the time position when playing an audio file */
	if (playing) {	
		GstFormat fmt = GST_FORMAT_TIME;
		gint64 len;
		if (gst_element_query_position (pipeline, &fmt, &pos)
		&& gst_element_query_duration (pipeline, &fmt, &len)) {
		g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
		GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
		}
		return TRUE;
		}
}

static gboolean seek_to_time (GstElement *pipeline)
{ /* if apropriate button is clicked, seek backward or forward when playing an audio file */
if (forward == 1 || backward == 1)
	{ 
	if (!gst_element_seek (pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
	GST_SEEK_TYPE_SET, nSec * GST_SECOND + pos,
	GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
	g_print ("Seek failed!\n");
	}
	}
return TRUE;
}

/* This is the main function where gstreamer handles the audio streams, either from the microphone (typeSource == 0), or from an audio file (typeSource == 1), or from jack (typeSource == 2)*/
void sdlWindow()
{
	if (playing == 1)
    	{ /* play if paused or stopped; pause if playing */
		if (pose == 0) {
		        pose = 1;
			gst_element_set_state (pipeline, GST_STATE_PAUSED);
		        }
		 else {
		        pose = 0;
			gst_element_set_state (pipeline, GST_STATE_PLAYING);
		        }
    	}
       else if (playing == 0)
	{
	AUDIOFREQ = 44100;
	int interval = 50000000;
	GstElement *src, *dec, *audioconvert, *conv, *spectrum, *sink;
	GstBus *bus;
	GstCaps *caps;
	GstPad *audiopad;
	GstStateChangeReturn ret;
	
	SDL_Init(SDL_INIT_VIDEO); // init SDL parameters
	screen = SDL_SetVideoMode(WIDTH_WINDOW+100, HEIGHT_WINDOW+80, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Spectrum Analyzer : Real-Time Spectral Analysis",NULL);
	blackScreen = SDL_CreateRGBSurface(SDL_HWSURFACE, WIDTH_WINDOW, HEIGHT_WINDOW, 32, 0, 0, 0, 0);
	printLine(); // some lines and scale for the SDL window
	printScale();
	SDL_Flip(screen);
	
	gst_init (NULL, NULL);
	loop = g_main_loop_new(NULL, FALSE);
	playing = 1;
            
    if (typeSource == 2) //source == JACK 
    	{
	pipeline = gst_pipeline_new ("pipeline");
    	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch(bus, message_handler, NULL);
	gst_object_unref(bus);
	jack_client_t *src_client;
	jack_status_t status;
	GstStateChangeReturn ret;

	  /* create jack clients */
	  src_client = jack_client_open ("src_client", JackNoStartServer, &status);
	  if (src_client == NULL) {
	    if (status & JackServerFailed)
		{
		GtkWidget *pAbout;
    		gchar *sSite = "";
		pAbout = gtk_message_dialog_new (GTK_WINDOW(mainWindow),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			"JACK server not running");
		gtk_dialog_run(GTK_DIALOG(pAbout));
	     	gtk_widget_destroy(pAbout);
		TTF_Quit();
		SDL_Quit();
		playing = 0;
		}
	    else
		{
		GtkWidget *pAbout;
    		gchar *sSite = "";
		pAbout = gtk_message_dialog_new (GTK_WINDOW(mainWindow),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			"jack_client_open() failed",
			sSite);
		gtk_dialog_run(GTK_DIALOG(pAbout));
	     	gtk_widget_destroy(pAbout);
		TTF_Quit();
		SDL_Quit();
		exit(1);
		}		      
	  }

	src = gst_element_factory_make ("jackaudiosrc", NULL);
    	sink = gst_element_factory_make ("fakesink", NULL);
    	g_object_set (src, "client", src_client, NULL);
    	spectrum = gst_element_factory_make ("spectrum", "spectrum");
	g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -80, "interval", interval, NULL);
	gst_bin_add_many (GST_BIN (pipeline), src, spectrum, sink, NULL);
	
    	if (!gst_element_link (src, spectrum) ||
	    !gst_element_link (spectrum, sink)) {
	fprintf (stderr, "can't link elements\n");
	exit (1);
	}
     	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	g_main_loop_run (loop);
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	}

      else if (typeSource == 0) // source == microphone with ALSA
	{
	pipeline = gst_pipeline_new ("pipeline");
    	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch(bus, message_handler, NULL);
	gst_object_unref(bus);
	src = gst_element_factory_make ("alsasrc", "src");
	audioconvert = gst_element_factory_make ("audioconvert", NULL);
	g_assert (audioconvert);
	spectrum = gst_element_factory_make ("spectrum", "spectrum");
	g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -80, "interval", interval, NULL);
	// "interval" property : 10 exp7 = maximal speed; 10 exp 8 = minimal speed
	sink = gst_element_factory_make ("fakesink", "sink");
	gst_bin_add_many (GST_BIN (pipeline), src, audioconvert, spectrum, sink, NULL);
	caps = gst_caps_new_simple ("audio/x-raw-int", "rate", G_TYPE_INT, AUDIOFREQ, NULL);

	if (!gst_element_link (src, audioconvert) ||
	    !gst_element_link_filtered (audioconvert, spectrum, caps) ||
	    !gst_element_link (spectrum, sink)) {
	    fprintf (stderr, "can't link elements\n");
		exit (1);
	    }
	gst_caps_unref (caps);
	ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_print ("Failed to start up pipeline!\n");
		}

        //gst_element_set_state(pipeline, GST_STATE_PLAYING);
	g_main_loop_run (loop);
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	}

    else if (typeSource == 1) // source == file read from disk, using ALSA without Jack
    {	
	pipeline = gst_pipeline_new ("pipeline");
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, message_handler, NULL);
	gst_object_unref (bus);
	
	src = gst_element_factory_make ("filesrc", "source");
	g_object_set (G_OBJECT (src), "location", sFile, NULL);
	dec = gst_element_factory_make ("decodebin", "decoder");
	g_signal_connect (dec, "new-decoded-pad", G_CALLBACK (cb_newpad), NULL);
	spectrum = gst_element_factory_make ("spectrum", "spectrum");
	g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -80,
	    "message", TRUE, "message-phase", TRUE, "interval", interval, NULL);
	gst_bin_add_many (GST_BIN (pipeline), src, dec, NULL);
	gst_element_link (src, dec);

	audio = gst_bin_new ("audiobin");
	conv = gst_element_factory_make ("audioconvert", "aconv");
	audiopad = gst_element_get_static_pad (conv, "sink");
	sink = gst_element_factory_make ("alsasink", "sink");
	gst_bin_add_many (GST_BIN (audio), conv, spectrum, sink, NULL);
	gst_element_link_many (conv, spectrum, sink, NULL);
	gst_element_add_pad (audio,
	gst_ghost_pad_new ("sink", audiopad));
	gst_object_unref (audiopad);
	gst_bin_add (GST_BIN (pipeline), audio);
        	
	g_timeout_add (200, (GSourceFunc) cb_print_position, pipeline);
	gst_element_set_state (pipeline, GST_STATE_PLAYING);
	g_timeout_add (51, (GSourceFunc) seek_to_time, pipeline);
	
	g_main_loop_run (loop);
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (pipeline));	
	}
    TTF_Quit();
    SDL_Quit();
    }

}


