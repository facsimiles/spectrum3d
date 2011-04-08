/* This file is part of Spectrum3D.

    Spectrum3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Spectrum3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Spectrum3D.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
//#include <stdarg.h>
#include <string.h>
#include <gst/gst.h>
#include <gtk/gtk.h>
#include <jack/jack.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

gint64 pos;
static GtkWidget *mainWindow;
GstElement *pipeline, *audio;
GLfloat magnitude;

#include "functions.h"

/* Set up OpenGl */
void setupOpengl() {
	glViewport(0, 0, WIDTH_WINDOW, HEIGHT_WINDOW);
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gluPerspective(45, (float)WIDTH_WINDOW/HEIGHT_WINDOW, 0.1, 200);
	glMatrixMode(GL_MODELVIEW);
}

/* auto-decode audio file (decodebin) */
static void cb_newpad (GstElement *decodebin, GstPad *pad, gboolean last, gpointer data)
{
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

/* Detect keyboard and mouse events in the SDL window */
void sdlEvent()
{
	Uint8* keys;
	SDL_Event event;
	int MouseX,MouseY;
	
	/* Keyboard events */
	keys = SDL_GetKeyState(NULL);
		if ( keys [SDLK_ESCAPE] ) {
			onStop();
			}
		else if ( keys[SDLK_s] && keys[SDLK_UP] ) {
			if (zoom < 2500){
				zoom+=5;
				}
			}
		else if ( keys[SDLK_s] && keys[SDLK_DOWN] ) {
			if (zoom > 0){
				zoom-=5;
				}
			}
		else if ( keys[SDLK_x] && keys[SDLK_RIGHT] ) {
			X+=0.004;
			}
		else if ( keys[SDLK_x] && keys[SDLK_LEFT] ) {
			X-=0.004;
			}
		else if ( keys[SDLK_y] && keys[SDLK_UP] ) {
			Y+=0.004;
			}
		else if ( keys[SDLK_y] && keys[SDLK_DOWN] ) {
			Y-=0.004;
			}
		else if ( keys[SDLK_z] && keys[SDLK_UP] ) {
			Z+=0.004;
			}
		else if ( keys[SDLK_z] && keys[SDLK_DOWN] ) {
			Z-=0.004;
			}
		else if ( keys[SDLK_c] && keys[SDLK_UP] ) {
			AngleZ+=0.4;
			}
		else if ( keys[SDLK_c] && keys[SDLK_DOWN] ) {
			AngleZ-=0.4;
			}
		else if ( keys[SDLK_g] && keys[SDLK_UP] ) {
			if (gain < 1.996){
				gain+=0.005;
				}
			}
		else if ( keys[SDLK_g] && keys[SDLK_DOWN] ) {
			if (gain > 0.005){
				gain-=0.005;
				}
			}
		else if ( keys[SDLK_UP] ) {
			AngleV+=0.4;
			}
		else if ( keys[SDLK_DOWN] ) {
			AngleV-=0.4;
			}
		else if ( keys[SDLK_RIGHT] && !keys[SDLK_v] ) {
			AngleH+=0.4;
			}
		else if ( keys[SDLK_LEFT] ) {
			AngleH-=0.4;
			}
		else if ( keys[SDLK_SPACE] ) {
			sdlWindow();
			}
		else if ( keys[SDLK_p] ) {
			zoom+=5;
			}
		
	/* Mouse and combined keyboard and mouse events */
	if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)) {
		SDL_GetRelativeMouseState(&MouseX,&MouseY);
		if ( keys[SDLK_x] && (MouseX < 100 && MouseX > -100) ) {
			if (MouseX > 0) {
				X+=0.02;
				}
			else if (MouseX < 0) {
				X-=0.02;
				}
			}
		else if ( keys[SDLK_y] && (MouseY < 100 && MouseY > -100) ) {
			if (MouseY > 0) {
				Y-=0.02;
				}
			else if (MouseY < 0) {
				Y+=0.02;
				}
			}
		else if ( keys[SDLK_z] && (MouseY < 100 && MouseY > -100) ) {
			if (MouseY > 0) {
				Z-=0.02;
				}
			else if (MouseY < 0) {
				Z+=0.02;
				}
			}
		else if ( keys[SDLK_c] && ((MouseX < 10 && MouseX > -10) || (MouseY < 10 && MouseY > -10))) {
			AngleZ-=MouseY/10;
			}
		else if ((MouseX < 10 && MouseX > -10) || (MouseY < 10 && MouseY > -10)) {
			AngleH+=MouseX/10;
			AngleV+=MouseY/10;
			}
		}

	SDL_PollEvent(&event);
		switch (event.type)
			{
			case SDL_QUIT:
				onStop();
			break;
			case SDL_KEYDOWN:
			    switch(event.key.keysym.sym) {
				case SDLK_ESCAPE:
					onStop();
					break;
				default:
				break;
				}
			default:
			break;
			}
}

/* Detect keyboard and mouse events in the SDL window while paused*/
int sdlEventPaused()
{
	Uint8* keys;
	SDL_Event event;
	int MouseX,MouseY;
	int result = 0;

	SDL_EnableKeyRepeat(10, 10);
	
	/* Keyboard events */
	keys = SDL_GetKeyState(NULL);
		if ( keys [SDLK_ESCAPE] ) {
			onStop();
			}
		else if ( keys[SDLK_s] && keys[SDLK_UP] ) {
			if (zoom < 2500){
				zoom+=5;
				}
			}
		else if ( keys[SDLK_s] && keys[SDLK_DOWN] ) {
			if (zoom > 0){
				zoom-=5;
				}
			}
		else if ( keys[SDLK_x] && keys[SDLK_RIGHT] ) {
			X+=0.004;
			}
		else if ( keys[SDLK_x] && keys[SDLK_LEFT] ) {
			X-=0.004;
			}
		else if ( keys[SDLK_y] && keys[SDLK_UP] ) {
			Y+=0.004;
			}
		else if ( keys[SDLK_y] && keys[SDLK_DOWN] ) {
			Y-=0.004;
			}
		else if ( keys[SDLK_z] && keys[SDLK_UP] ) {
			Z+=0.004;
			}
		else if ( keys[SDLK_z] && keys[SDLK_DOWN] ) {
			Z-=0.004;
			}
		else if ( keys[SDLK_c] && keys[SDLK_UP] ) {
			AngleZ+=0.4;
			}
		else if ( keys[SDLK_c] && keys[SDLK_DOWN] ) {
			AngleZ-=0.4;
			}
		else if ( keys[SDLK_g] && keys[SDLK_UP] ) {
			if (gain < 2){
				gain+=0.001;
				}
			}
		else if ( keys[SDLK_g] && keys[SDLK_DOWN] ) {
			if (gain > 0){
				gain-=0.001;
				}
			}
		else if ( keys[SDLK_UP] ) {
			AngleV+=0.4;
			}
		else if ( keys[SDLK_DOWN] ) {
			AngleV-=0.4;
			}
		else if ( keys[SDLK_RIGHT] && !keys[SDLK_v] ) {
			AngleH+=0.4;
			}
		else if ( keys[SDLK_LEFT] ) {
			AngleH-=0.4;
			}
		else if ( keys[SDLK_SPACE] ) {
			sdlWindow();
			}

if ( keys[SDLK_c] || keys[SDLK_g] || keys[SDLK_s] || keys[SDLK_x] || keys[SDLK_y] || keys[SDLK_z] || keys[SDLK_UP] || keys[SDLK_DOWN] || keys[SDLK_LEFT] || keys[SDLK_RIGHT] || keys[SDLK_SPACE] ) {
			result = 1;
			}
		
	/* Mouse and combined keyboard and mouse events */
	if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)) {
		result = 1;
		SDL_GetRelativeMouseState(&MouseX,&MouseY);
		if ( keys[SDLK_x] && (MouseX < 100 && MouseX > -100) ) {
			if (MouseX > 0) {
				X+=0.02;
				}
			else if (MouseX < 0) {
				X-=0.02;
				}
			}
		else if ( keys[SDLK_y] && (MouseY < 100 && MouseY > -100) ) {
			if (MouseY > 0) {
				Y-=0.02;
				}
			else if (MouseY < 0) {
				Y+=0.02;
				}
			}
		else if ( keys[SDLK_z] && (MouseY < 100 && MouseY > -100) ) {
			if (MouseY > 0) {
				Z-=0.02;
				}
			else if (MouseY < 0) {
				Z+=0.02;
				}
			}
		else if ( keys[SDLK_c] && ((MouseX < 10 && MouseX > -10) || (MouseY < 10 && MouseY > -10))) {
			AngleZ-=MouseY/10;
			}
		else if ((MouseX < 10 && MouseX > -10) || (MouseY < 10 && MouseY > -10)) {
			AngleH+=MouseX/10;
			AngleV+=MouseY/10;
			}
		}

	SDL_PollEvent(&event);
		switch (event.type)
			{
			case SDL_QUIT:
				onStop();
			break;
			case SDL_KEYDOWN:
			    switch(event.key.keysym.sym) {
				case SDLK_ESCAPE:
					onStop();
					break;
				default:
				break;
				}
			default:
			break;
			}
return result;
}

/* Play  a very small part of an audio file every time this is called */
gboolean playSlowly()
{
	SDL_Event event;
	Uint8* keys;
	SDL_PollEvent(&event);
		
	keys = SDL_GetKeyState(NULL);
		if ( keys[SDLK_v] && keys[SDLK_RIGHT] ) {
			gst_element_set_state (pipeline, GST_STATE_PLAYING);
			SDL_Delay(50);
			gst_element_set_state (pipeline, GST_STATE_PAUSED);
			}
return TRUE;
}

/* Gstreamer message handler */
static gboolean message_handler (GstBus * bus, GstMessage * message, gpointer data)
{	
	counter = counter +1;
	sdlEvent();
			
        if (message->type == GST_MESSAGE_ELEMENT && counter > counterNumber) {
		counter = 0;
            const GstStructure *s = gst_message_get_structure (message);
            const gchar *name = gst_structure_get_name (s);
            GstClockTime endtime;

                if (strcmp (name, "spectrum") == 0)
                {
		  if (!gst_structure_get_clock_time (s, "endtime", &endtime))
                  endtime = GST_CLOCK_TIME_NONE;

                  magnitudes = gst_structure_get_value (s, "magnitude");
                  		
		  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
		  glLoadIdentity();
		
		displaySpectro();
		/*if (lineScale == 1) {
			drawScale();
			}
		if (textScale == 1) {
			RenderText();
			}
		SDL_GL_SwapBuffers();*/
		}
	}
return TRUE;
}

/* Print the total duration and the time position in a terminal when playing an audio file */
static gboolean cb_print_position (GstElement *pipeline)
{
	if (playing) {	
		GstFormat fmt = GST_FORMAT_TIME;
		gint64 len;
		if (gst_element_query_position (pipeline, &fmt, &pos)
		&& gst_element_query_duration (pipeline, &fmt, &len)) {
		g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
		GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
		}
		}
	return TRUE;
}

/* Seek backward or forward when playing an audio file */
static gboolean seek_to_time (GstElement *pipeline)
{
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

/* This is the main function where gstreamer handles the audio stream, either from the microphone (typeSource == 0), or from an audio file (typeSource == 1), or from jack (typeSource == 2) */
void sdlWindow()
{
/* play if paused or stopped; pause if playing */
	if (playing == 1)
    	{
		if (pose == 0) {
		        pose = 1;
			gst_element_set_state (pipeline, GST_STATE_PAUSED);
			g_timeout_add (80, (GSourceFunc) displayPausedSpectro, pipeline);
			if (typeSource == 1) {
				g_timeout_add (80, (GSourceFunc) playSlowly, pipeline);
				}
			}
		 else {
		        pose = 0;
			gst_element_set_state (pipeline, GST_STATE_PLAYING);
		        }
    	}

/* Play */
       else if (playing == 0)
	{
	PROPORTION = (width-200);
	PROPORTION = (float)width / 1000;
	x = 1.2 * RESIZE;
	z = PROPORTION;
	X = -0.7 * RESIZE;
	Y = -0.1 * RESIZE;
	Z = -1.05;
	AUDIOFREQ = 44100;
	int interval = 100000000;
	GstElement *src, *dec, *audioconvert, *conv, *spectrum, *sink;
	GstBus *bus;
	GstCaps *caps;
	GstPad *audiopad;
	GstStateChangeReturn ret;
	
	TTF_Init();
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(WIDTH_WINDOW, HEIGHT_WINDOW, 24, SDL_OPENGL | SDL_GL_DOUBLEBUFFER);
	SDL_WM_SetCaption("Spectrum : Real-Time Spectral Analysis",NULL);
	font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 16 * RESIZE);

	setupOpengl();
	
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

	  src_client = jack_client_open ("src_client", JackNoStartServer, &status); // create jack clients
	  if (src_client == NULL) {
	    if (status & JackServerFailed)
		{
		GtkWidget *pAbout;
    		//gchar *sSite = "";
		pAbout = gtk_message_dialog_new (GTK_WINDOW(mainWindow),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			"JACK server not running");
		gtk_dialog_run(GTK_DIALOG(pAbout));
	     	gtk_widget_destroy(pAbout);
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
			"jack_client_open() failed %s",
			sSite);
		gtk_dialog_run(GTK_DIALOG(pAbout));
	     	gtk_widget_destroy(pAbout);
		SDL_Quit();
		exit(1);
		}		      
	  }

	src = gst_element_factory_make ("jackaudiosrc", NULL);
    	sink = gst_element_factory_make ("fakesink", NULL);
    	//g_object_set (src, "client", src_client, NULL);
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
	TTF_CloseFont(font);
        SDL_Quit();
	TTF_Quit();
	}

}


