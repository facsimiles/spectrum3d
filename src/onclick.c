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
#include <gst/gst.h>
#include <stdbool.h>
#include <gtk/gtk.h>

#include "config.h"

#include "onclick.h"

void onPlay(GtkWidget *pWidget, gpointer data)
{
	sdlWindow(pWidget, NULL);
}

/* Get the type of source (microphone, audio file or jack) */
void onSource(GtkWidget *pBtn, gpointer data)
{  
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

/* Reset position */
void onReset()
{
	X = -0.7 * RESIZE;
	Y = -0.1 * RESIZE;
	Z = -1.05;
	AngleH = -16.0;
	AngleV = 10.0;
	AngleZ = 0;
}

/* Front position */
void onFrontView()
{
	X = -0.64 * RESIZE;
	Y = -0.36 * RESIZE;
	Z = -0.95;
	AngleH = 0;
	AngleV = 0;
	AngleZ = 0;
}

/* Preset */
void onPreset()
{
	X = presetX;
	Y = presetY;
	Z = presetZ;
	AngleH = presetAngleH;
	AngleV = presetAngleV;
	AngleZ = presetAngleZ;
}

/* stop playing audio and displaying SDL surface */
void onStop() 
{ 
	if (loop != NULL){
		g_main_loop_quit(loop);
		}
	playing = 0;
}

/* Get startin value of the zoom (every 500 Hz, from 0 to 4000 Hz) */
void cb_zoom_changed(GtkComboBox *combo, gpointer data)
{
  	gchar *string = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
	int spinZoom = strtol(string, NULL, 10);
	zoom = spinZoom / 2;
	g_free( string );

	if (pose == 1) {
		change = 1;
		}
}

/* Get the range of the zoom (range : 1000 to 20000 Hz) */
void cb_range_changed(GtkWidget *combo, gpointer data)
{
    gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
    if (index == 0) {
	zoomFactor = 1;
	}
    else if (index == 1) {
	zoomFactor = 2;
	}
    else if (index == 2) {
	zoomFactor = 4;
	}
    else if (index == 3) {
	zoomFactor = 6;
	}
    else if (index == 4) {
	zoomFactor = 10;
	}
    else if (index == 5) {
	zoomFactor = 20;
	zoom = 0;
	}    

	if (pose == 1) {
		change = 1;
		}
}

/* Change the speed of display */
void cb_speed_changed(GtkComboBox *combo, gpointer data)
{
	gint index = gtk_combo_box_get_active( combo );
	counterNumber = index;
}

/* Seek 5 sec forward in audio file while playing */
void timeForward() 
{
forward = 1;
nSec = 5;
}

/* Seek 5 sec backward in audio file while playing */
void timeBackward() 
{
backward = 1;
nSec = -5;
}

/* Stop seeking (when "Seek" button is released) */
void stopSeek() 
{
forward = 0;
backward = 0;
}

/* Change scale */
void cb_scale_changed(GtkComboBox *combo, gpointer data) {
	gint index = gtk_combo_box_get_active( combo );
	if (index ==0 ) {
		scale = 0;
	}
	else if (index == 1) {
		scale = 1;
	}
	else if (index == 2) {
		scale = 2;
	}
	else if (index == 3) {
		scale = 3;
	}
}

/* Enable lines */
void onCheckLineScale(GtkWidget *pToggle, gpointer data)
{
	gboolean bState;
	bState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pToggle));
	if (bState) {
		lineScale = 1;
	}
	else {
		lineScale = 0;
		}

	if (pose == 1) {
		change = 1;
		}
}

/* Enable text scale */
void onCheckTextScale(GtkWidget *pToggle, gpointer data)
{
	gboolean bState;
	bState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pToggle));
	if (bState) {
		textScale = 1;
	}
	else {
		textScale = 0;
		}

	if (pose == 1) {
		change = 1;
		}
}

/* Enable panels */
void onCheckShowPanels(GtkWidget *pToggle, gpointer data)
{
	gboolean bState;
	bState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pToggle));
	if (bState) {
		showPanels = 1;
	}
	else {
		showPanels = 0;
		}
	
	if (pose == 1) {
		change = 1;
		}
}

/* Enable gain change */
void changeGain(GtkWidget *pWidget, gpointer data)
{
	gfloat iValue;
	iValue = gtk_range_get_value(GTK_RANGE(pWidget));
	gain = iValue;

	if (pose == 1) {
		change = 1;
		}
}

/* Enable panel height change */
void changeyPanel(GtkWidget *pWidget, gpointer data)
{
	gfloat iValue;
	iValue = gtk_range_get_value(GTK_RANGE(pWidget));
	yPanel = iValue;

	if (pose == 1) {
		change = 1;
		}
}

/* Enable 'Depth' change */
void changeDepth(GtkWidget *pWidget, gpointer data)
{
	gfloat iValue;
	iValue = gtk_range_get_value(GTK_RANGE(pWidget));
	z = iValue * PROPORTION;

	if (pose == 1) {
		change = 1;
		}
}

/* Change 'Width' of main window */
void changeWidth(GtkSpinButton *spinButton, gpointer user_data)
{
	onClickWidth = gtk_spin_button_get_value_as_int(spinButton);	
}



