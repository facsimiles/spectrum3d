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
#include <string.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "config.h"

#ifdef JACK
	#include <jack/jack.h>
#endif

#include "mainwindow.h"

const GValue *magni;
GstElement *audio;
gint64 pos;

#ifdef JACK
	static GtkWidget *mainWindow;
#endif

#ifdef REALTIME
static GstTaskPool *pool;

static GstBusSyncReply
sync_bus_handler (GstBus * bus, GstMessage * message, GstElement * bin)
{
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_STREAM_STATUS:
    {
      GstStreamStatusType type;
      GstElement *owner;
      const GValue *val;
      gchar *path;
      GstTask *task = NULL;

      //g_message ("received STREAM_STATUS");
      gst_message_parse_stream_status (message, &type, &owner);

      val = gst_message_get_stream_status_object (message);

      //g_message ("type:   %d", type);
      path = gst_object_get_path_string (GST_MESSAGE_SRC (message));
      //g_message ("source: %s", path);
      g_free (path);
      path = gst_object_get_path_string (GST_OBJECT (owner));
      //g_message ("owner:  %s", path);
      g_free (path);

      if (G_VALUE_HOLDS_OBJECT (val)) {
        //g_message ("object: type %s, value %p", G_VALUE_TYPE_NAME (val),
            //g_value_get_object (val));
      } else if (G_VALUE_HOLDS_POINTER (val)) {
        //g_message ("object: type %s, value %p", G_VALUE_TYPE_NAME (val),
            //g_value_get_pointer (val));
      } else if (G_IS_VALUE (val)) {
        //g_message ("object: type %s", G_VALUE_TYPE_NAME (val));
      } else {
        //g_message ("object: (null)");
        break;
      }

      /* see if we know how to deal with this object */
      if (G_VALUE_TYPE (val) == GST_TYPE_TASK) {
        task = g_value_get_object (val);
      }

      switch (type) {
        case GST_STREAM_STATUS_TYPE_CREATE:
          if (task) {
            //g_message ("created task %p, setting pool", task);
            gst_task_set_pool (task, pool);
          }
          break;
        case GST_STREAM_STATUS_TYPE_ENTER:
          break;
        case GST_STREAM_STATUS_TYPE_LEAVE:
          break;
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
  /* pass all messages on the async queue */
  return GST_BUS_PASS;
}
#endif


/* Gstreamer message handler */
gboolean message_handler (GstBus * bus, GstMessage * message, gpointer data)
{	
	counter = counter +1;
	int ii = 0;
	sdlEvent();
			
        if (message->type == GST_MESSAGE_ELEMENT && counter > counterNumber) {
		counter = 0;
		const GstStructure *s = gst_message_get_structure (message);
		const gchar *name = gst_structure_get_name (s);
		GstClockTime endtime;
		

                if (strcmp (name, "spectrum") == 0) {
			if (!gst_structure_get_clock_time (s, "endtime", &endtime))
			endtime = GST_CLOCK_TIME_NONE;
			magnitudes = gst_structure_get_value (s, "magnitude");
			for (ii = 0; ii < 10000 ; ii++) {
				magni = gst_value_list_get_value (magnitudes, ii);
				prec[0][ii] = ((80 + (g_value_get_float(magni)))/40);
				}
			displaySpectro();
			}
		}
	if (message->type == GST_MESSAGE_EOS) {
		g_main_loop_quit (loop);
		}
return TRUE;
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

/* Print the total duration and the time position in a terminal when playing an audio file */
static gboolean cb_print_position (GstElement *pipeline)
{

		GstFormat fmt = GST_FORMAT_TIME;
		gint64 len; 
		if (gst_element_query_position (pipeline, &fmt, &pos)
		&& gst_element_query_duration (pipeline, &fmt, &len)) {
		g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
		GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
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

void playFromMic(GtkWidget *pWidget, gpointer data)
{
	int AUDIOFREQ = 44100;
	GstElement *alsasrc, *audioconvert, *spectrum, *alsasink;
#ifdef REALTIME
	GstBus *bus;
#endif
	GstBus *busMH;
	GstStateChangeReturn ret;
	GstCaps *caps;

  	gst_init (NULL, NULL);

#ifdef REALTIME
  	pool = test_rt_pool_new ();
#endif
	loop = g_main_loop_new(NULL, FALSE);
	playing = 1;
  
	pipeline = gst_pipeline_new ("pipeline");
	g_assert (pipeline);
	alsasrc = gst_element_factory_make ("alsasrc", "alsasrc");
	g_assert (alsasrc);
//g_object_set (alsasrc, "device", "hw:0", NULL);
//g_object_set (alsasrc, "latency-time", (gint64) 2000, NULL);
//g_object_set (alsasrc, "slave-method", 2, NULL);
	audioconvert = gst_element_factory_make ("audioconvert", NULL);
	g_assert (audioconvert);
	spectrum = gst_element_factory_make ("spectrum", "spectrum");
	g_assert (spectrum);	
	g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -80, "interval", (interval * 10000000), NULL);

	alsasink = gst_element_factory_make ("fakesink", "alsasink");
	g_assert (alsasink);
//g_object_set (alsasink, "device", "hw:0", NULL);
//g_object_set (alsasink, "latency-time", (gint64) 2000, NULL);
//g_object_set (alsasink, "buffer-time", (gint64) 10000, NULL);

	gst_bin_add_many (GST_BIN (pipeline), alsasrc, audioconvert, spectrum, alsasink, NULL);
	caps = gst_caps_new_simple ("audio/x-raw-int", "rate", G_TYPE_INT, AUDIOFREQ, NULL);
	if (!gst_element_link (alsasrc, audioconvert) ||
	    !gst_element_link_filtered (audioconvert, spectrum, caps) ||
	    !gst_element_link (spectrum, alsasink)) {
	    fprintf (stderr, "can't link elements\n");
		exit (1);
	    }
#ifdef REALTIME
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_set_sync_handler (bus, (GstBusSyncHandler) sync_bus_handler, pipeline);
	gst_object_unref (bus);
#endif
	ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE){
		g_print ("Failed to start up pipeline!\n");
		}
	busMH = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch(busMH, message_handler, NULL);
	gst_object_unref(busMH);
	printf ("Now playing\n");
	g_main_loop_run (loop);

	gst_element_set_state (pipeline, GST_STATE_NULL);
	
	gst_object_unref (busMH);
	sdlQuit();
#ifdef GEIS
	geisQuit();
#endif
}

void playFromFile(GtkWidget *pWidget, gpointer data)
{
	int AUDIOFREQ = 44100;
	GstElement *playbin, *conv, *spectrum, *sink;
	GstBus *bus;
#ifdef REALTIME
	GstBus *busrt;
#endif
	GstCaps *caps;
	GstPad *audiopad;
	//GstStateChangeReturn ret;

	gst_init (NULL, NULL);

#ifdef REALTIME
	pool = test_rt_pool_new ();
#endif
	loop = g_main_loop_new(NULL, FALSE);
	playing = 1;
while (filenames != NULL) {
	gchar file[200]; 
	sprintf(file, "file://%s", sFile); 
	gchar *uri;  
	uri = g_strdup (file);
            
	pipeline = gst_bin_new ("pipeline");
	playbin = gst_element_factory_make ("playbin2", "source");
	g_assert (playbin);
	g_object_set (G_OBJECT (playbin), "uri", uri, NULL);
	spectrum = gst_element_factory_make ("spectrum", "spectrum");
	g_assert (spectrum);
	g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -80,
	    "message", TRUE, "interval", (interval * 10000000), NULL);
	conv = gst_element_factory_make ("audioconvert", "aconv");
	g_assert (conv);
	sink = gst_element_factory_make("autoaudiosink", "sink");
	g_assert(sink);
	gst_bin_add_many (GST_BIN (pipeline), conv, spectrum, sink, NULL);
	caps = gst_caps_new_simple ("audio/x-raw-int", "rate", G_TYPE_INT, AUDIOFREQ, NULL);

	if (!gst_element_link_filtered (conv, spectrum, caps) ||
	    !gst_element_link (spectrum, sink)){
	    fprintf (stderr, "can't link elements\n");
		exit (1);
	    }
	gst_caps_unref (caps);
		
	audiopad = gst_element_get_static_pad (conv, "sink");
	gst_element_add_pad (pipeline, gst_ghost_pad_new (NULL, audiopad));
	g_object_set(G_OBJECT(playbin), "audio-sink", pipeline, NULL);
	gst_object_unref (audiopad);
	bus = gst_pipeline_get_bus (GST_PIPELINE (playbin));
	gst_bus_add_watch (bus, message_handler, NULL);
	gst_object_unref (bus);
#ifdef REALTIME
	busrt = gst_pipeline_get_bus (GST_PIPELINE (playbin));
  	gst_bus_set_sync_handler (busrt, (GstBusSyncHandler) sync_bus_handler, playbin);
	gst_object_unref (busrt);
#endif
	g_timeout_add (200, (GSourceFunc) cb_print_position, pipeline);
	gst_element_set_state (playbin, GST_STATE_PLAYING);
	g_timeout_add (101, (GSourceFunc) seek_to_time, pipeline);
	printf ("Now playing\n");
	g_main_loop_run (loop);
	gst_element_set_state (playbin, GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (pipeline));
	filenames = filenames->next;
	getFileName();
	}
	sdlQuit();
#ifdef GEIS
	geisQuit();
#endif
}

#ifdef JACK
void playFromJack(GtkWidget *pWidget, gpointer data)  
{
	int interval = 100000000;
	GstElement *src, *spectrum, *sink;
	GstBus *bus;
	//GstStateChangeReturn ret;

	gst_init (NULL, NULL);
	loop = g_main_loop_new(NULL, FALSE);
	playing = 1;

	pipeline = gst_pipeline_new ("pipeline");
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch(bus, message_handler, NULL);
	gst_object_unref(bus);
	jack_client_t *src_client;
	jack_status_t status;

	src_client = jack_client_open ("src_client", JackNoStartServer, &status); // create jack clients
	if (src_client == NULL) {
		if (status & JackServerFailed){
			GtkWidget *pAbout;
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
		else {
			GtkWidget *pAbout;
			pAbout = gtk_message_dialog_new (GTK_WINDOW(mainWindow),
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_OK,
				"jack_client_open() failed");
			gtk_dialog_run(GTK_DIALOG(pAbout));
		     	gtk_widget_destroy(pAbout);
			SDL_Quit();
			exit(1);
			}		      
		}

	src = gst_element_factory_make ("jackaudiosrc", NULL);
	g_assert(src);
	sink = gst_element_factory_make ("fakesink", NULL);
	g_assert(sink);
	spectrum = gst_element_factory_make ("spectrum", "spectrum");
	g_assert(spectrum);
	g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -80, "interval", interval, NULL);
	gst_bin_add_many (GST_BIN (pipeline), src, spectrum, sink, NULL);

		if (!gst_element_link (src, spectrum) ||
		    !gst_element_link (spectrum, sink)) {
		fprintf (stderr, "can't link elements\n");
		exit (1);
		}
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	printf ("Now playing\n");
	g_main_loop_run (loop);
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);

	sdlQuit();
#ifdef GEIS
	geisQuit();
#endif
}
#endif

/* This is the main function where gstreamer handles the audio stream, either from the microphone (typeSource == 0), or from an audio file (typeSource == 1), or from jack (typeSource == 2) */
void sdlWindow(GtkWidget *pWidget, gpointer data)
{
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
	
/* init everything */
	setupSDL();	
	
	setupOpengl();

#ifdef GEIS
	setupGeis();
#endif
	
/* print the audio source */
if (typeSource == 0){
	printf ("Source is Microphone.\n");
	}
else if (typeSource == 1){
	printf ("Source is Audio File.\n");
	}
else if (typeSource == 2){
	printf ("Source is Jack.\n");
	}

/* play */

if (typeSource == 0) {
playFromMic(pWidget, NULL);
	}

else if (typeSource == 1) {
playFromFile(pWidget, NULL);
	}

#ifdef JACK
else if (typeSource == 2) {
playFromJack(pWidget, NULL);
	}
#endif

printf("Stop playing\n");

}
}


