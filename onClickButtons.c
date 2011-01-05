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
#include <string.h>
#include <gst/gst.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include <SDL/SDL.h>

#include "functions.h"

void onSource(GtkWidget *pBtn, gpointer data)
{  /* Get the type of source (microphone, audio file or jack) */
	GSList *pList;
    	const gchar *sLabel;
    	pList = gtk_radio_button_get_group(GTK_RADIO_BUTTON(data));

	while(pList)
	{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pList->data)))
	{
	    sLabel = gtk_button_get_label(GTK_BUTTON(pList->data));
	    pList = NULL;
	}
	else
	{
	    pList = g_slist_next(pList);
	}
	}

	if (strcmp(sLabel, "Jack") == 0) {
		typeSource = 2;
		}
	else if (strcmp(sLabel, "Mic") == 0) {
		typeSource = 0;
		}
	else if (strcmp(sLabel, "Audio File") == 0) {
		typeSource = 1;
		}
}

void onDisplay(GtkWidget *pBtn, gpointer data)
{  /* Get the type of display (continuous or snapshot) */
	GSList *pList;
    	const gchar *sLabel;
    	pList = gtk_radio_button_get_group(GTK_RADIO_BUTTON(data));

	while(pList)
	{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pList->data)))
	{
	    sLabel = gtk_button_get_label(GTK_BUTTON(pList->data));
	    pList = NULL;
	}
	else
	{
	    pList = g_slist_next(pList);
	}
	}

	if (strcmp(sLabel, "Continuous") == 0) {
		display = 0;
		}
	else if (strcmp(sLabel, "Snapshot") == 0) {
		display = 1;
		}
}

	/*GtkWidget *pAbout;
   	pAbout = gtk_message_dialog_new (GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_WARNING,
        GTK_BUTTONS_OK,
        "Uncheck JACK first!");
        gtk_dialog_run(GTK_DIALOG(pAbout));
        gtk_widget_destroy(pAbout);*/

void onStop() 
{ /* stop playing audio and displaying SDL surface */
  if (loop != NULL){
	g_main_loop_quit(loop);
	}
  SDL_Quit();
  playing = 0;
}

void cb_zoom_changed(GtkComboBox *combo, gpointer data)
{ /* Get startin value of the zoom (every 500 Hz, from 0 to 4000 Hz) */
    gchar *string = gtk_combo_box_get_active_text( combo );
    spinZoom = strtol(string, NULL, 10);
    
    switch (range)
	{
	case 1 :
	zoom = spinZoom / 2;
	break;
	case 2 :
	zoom = spinZoom / 4;
	break;
	case 4 :
	zoom = spinZoom / 8;
	break;
	case 6 :
	zoom = spinZoom / 12;
	break;
	case 10 :
	zoom = spinZoom / 20;
	break;
	case 20 :
	zoom = 0;
	break;
	}
    eraseScale();
    printScale();
    g_free( string );
}

void cb_range_changed(GtkComboBox *combo, gpointer data)
{  /* Get the magnitude of the zoom (range : 1000 to 20000 Hz) */
    gchar *string = gtk_combo_box_get_active_text( combo );
    if (strcmp(string, "1000")==0) {
	range = 1;
  	}
    else if (strcmp(string, "2000")==0) {
	range = 2;
	}
    else if (strcmp(string, "4000")==0) {
	range = 4;
	}
    else if (strcmp(string, "6000")==0) {
	range = 6;
	}
    else if (strcmp(string, "10000")==0) {
	range = 10;
	}
    else if (strcmp(string, "20000")==0) {
	range = 20;
	zoom = 0;
	spinZoom = 0;
	}    
    stringInt = strtol(string, NULL, 10);
    eraseScale();
    printScale();
    g_free( string );
}

void cb_speed_changed(GtkComboBox *combo, gpointer data)
{
/* Get the speed of display; two factors are combined : 'nCol' allows to display the date on a 1, 2 or 3 pixels-large column, and 'counterNumber' allows some set od date to be ignored, thereby decreasing the speed of display; the combination of thse 2 factors allows 6 differents speed of display */
    gchar *string = gtk_combo_box_get_active_text( combo );
    if (strcmp(string, "Speed 1 (fastest)")==0) {
	nCol = 3;
	counterNumber = 0;
  	}
    else if (strcmp(string, "Speed 2")==0) {
	nCol = 2;
	counterNumber = 0;
	}
    else if (strcmp(string, "Speed 3")==0) {
	nCol = 1;
	counterNumber = 0;
	}
    else if (strcmp(string, "Speed 4")==0) {
	nCol = 1;
	counterNumber = 1;
	}
    else if (strcmp(string, "Speed 5")==0) {
	nCol = 1;
	counterNumber = 2;
	}
    else if (strcmp(string, "Speed 6 (slowest)")==0) {
	nCol = 1;
	counterNumber = 3;
	}
    g_free( string );
}

void timeForward() // seek 5 sec forward in audio file while playing
{
forward = 1;
nSec = 5;
}

void timeBackward() // seek 5 sec backward in audio file while playing
{
backward = 1;
nSec = -5;
}

void stopSeek() // stop seeking (when "Seek" button is released)
{
forward = 0;
backward = 0;
}


