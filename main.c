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
#include <string.h>
gint64 pos;
static GtkWidget *mainWindow;
GtkWidget *pStatusTime;
#include "functions.h"

static void on_window_destroy (GObject * object, gpointer user_data)
{ /* Quit main window and eveything */
  if (sFile != NULL){
	g_free(sFile);
	}
  mainWindow = NULL;
  if (loop != NULL){
	g_main_loop_quit(loop);
	}
  SDL_Quit();
  gtk_main_quit ();
}

void selectFile(GtkWidget *pWidget, gpointer data) 
{ /* Select audio file if "Audio File" Source is choosen" */
	if (typeSource == 1) {
		GtkWidget *pFileSelection;
		GtkWidget *pDialog;
		GtkWidget *pParent;
		
		pParent = GTK_WIDGET(data);

		pFileSelection = gtk_file_chooser_dialog_new("Select File",
		GTK_WINDOW(pParent),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_OK,
		NULL);
		gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);

		switch(gtk_dialog_run(GTK_DIALOG(pFileSelection)))
		{
		case GTK_RESPONSE_OK:
		       sFile = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(pFileSelection));
		    break;
		default:
		    break;
		}
		gtk_widget_destroy(pFileSelection);
	}
}

int main(int argc, char *argv[])
{
    typeSource = 0; // all the default values of the analysis or display parameters
    playing = 0;
    spect_bands = 11025;
    zoom = 0;
    stringInt = 6000;
    spinZoom = 0;
    range = 6;
    forward = 0;
    backward = 0;
    display = 0;
    pose = 0;
    nCol = 1;
    counterNumber = 1;	

	GtkWidget *pVBox1, *pHBox[5], *pButton[3], *pButtonSelect, *pRadio[3], *pRadioDisplay[2], *pButtonFreq[10], *pComboRange, *pComboZoom, *pComboSpeed, *pFrame;
	gint timeId;

	gtk_init (&argc, &argv); // create main window, vertical and horizontal boxes
	mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (mainWindow, 550, 300);
	gtk_window_set_title(GTK_WINDOW(mainWindow), "Spectrum Analyzer : Real-Time Spectral Analysis");
	g_signal_connect (G_OBJECT (mainWindow), "destroy",
	G_CALLBACK (on_window_destroy), NULL);

	pVBox1 = gtk_vbox_new(TRUE, 0);
	pHBox[0] = gtk_hbox_new(TRUE, 0);
	pHBox[1] = gtk_hbox_new(TRUE, 0);
	pHBox[2] = gtk_hbox_new(TRUE, 0);
	pHBox[3] = gtk_hbox_new(TRUE, 0);
	pHBox[4] = gtk_hbox_new(TRUE, 0);
	
	/* "Play/Pause" button */
	gtk_container_add(GTK_CONTAINER(mainWindow), pVBox1); 
	gtk_box_pack_start(GTK_BOX(pVBox1), pHBox[0], TRUE, TRUE, 0);
	pButton[0] = gtk_button_new_with_label("                            Play/Pause                             ");
	gtk_box_pack_start(GTK_BOX(pHBox[0]), pButton[0], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton[0]), "clicked", G_CALLBACK(sdlWindow), NULL);
 
	/* "Select File" button */
	pButtonSelect = gtk_button_new_with_mnemonic("Select File"); 
	gtk_box_pack_start(GTK_BOX(pHBox[0]), pButtonSelect, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButtonSelect), "clicked", G_CALLBACK(selectFile), NULL);

	/* "Seek Backward" button */
	gtk_box_pack_start(GTK_BOX(pVBox1), pHBox[1], TRUE, TRUE, 0);
	pButton[0] = gtk_button_new_with_label("Seek Backward (5 sec)");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), pButton[0], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton[0]), "button-press-event", G_CALLBACK(timeBackward), NULL);
	g_signal_connect(G_OBJECT(pButton[0]), "button-release-event", G_CALLBACK(stopSeek), NULL);

	/* "Stop" button */
	pButton[1] = gtk_button_new_with_label("   STOP   ");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), pButton[1], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton[1]), "clicked", G_CALLBACK(onStop), NULL);

	/* "Seek Forward" button */
	pButton[2] = gtk_button_new_with_label("Seek Forward (5 sec)");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), pButton[2], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton[2]), "button-press-event", G_CALLBACK(timeForward), NULL);
	g_signal_connect(G_OBJECT(pButton[2]), "button-release-event", G_CALLBACK(stopSeek), NULL);

	/* "Mic", "Audio File" and "Jack" radio buttons */
	gtk_box_pack_start(GTK_BOX(pVBox1), pHBox[2], TRUE, TRUE, 0);	
	pRadio[0] = gtk_radio_button_new_with_label(NULL, "Mic");
    	gtk_box_pack_start(GTK_BOX (pHBox[2]), pRadio[0], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[0]), "toggled", G_CALLBACK(onSource), pRadio[0]);
	pRadio[1] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio[0]), "Audio File");
    	gtk_box_pack_start(GTK_BOX (pHBox[2]), pRadio[1], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[1]), "toggled", G_CALLBACK(onSource), pRadio[0]);
    	pRadio[2] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio[0]), "Jack");
    	gtk_box_pack_start(GTK_BOX (pHBox[2]), pRadio[2], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[2]), "toggled", G_CALLBACK(onSource), pRadio[0]);

	/* "Speed of display" combo box */
        gtk_box_pack_start(GTK_BOX(pVBox1), pHBox[3], TRUE, TRUE, 0);
	pFrame = gtk_frame_new("Speed of display");
	pComboSpeed = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), pComboSpeed);
	gtk_box_pack_start(GTK_BOX(pHBox[3]), pFrame, FALSE, FALSE, 0);
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 1 (fastest)");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 3");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 4");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 5");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 6 (slowest)");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pComboSpeed), 3);
	g_signal_connect( G_OBJECT(pComboSpeed), "changed", G_CALLBACK( cb_speed_changed ), NULL );

	/* "Continuous" and "Snapshot" (mode of display) radio buttons; */
	pRadioDisplay[0] = gtk_radio_button_new_with_label(NULL, "Continuous");
    	gtk_box_pack_start(GTK_BOX (pHBox[3]), pRadioDisplay[0], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadioDisplay[0]), "toggled", G_CALLBACK(onDisplay), pRadioDisplay[0]);
	pRadioDisplay[1] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadioDisplay[0]), "Snapshot");
    	gtk_box_pack_start(GTK_BOX (pHBox[3]), pRadioDisplay[1], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadioDisplay[1]), "toggled", G_CALLBACK(onDisplay), pRadioDisplay[0]);
    	
	/* "Range of the zoom" combo box */
	gtk_box_pack_start(GTK_BOX(pVBox1), pHBox[4], TRUE, TRUE, 0);
	pFrame = gtk_frame_new("Range of the zoom (in Hz)");
	pComboRange = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), pComboRange);
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, FALSE, FALSE, 0);
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "1000");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "2000");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "4000");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "6000");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "10000");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "20000");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pComboRange), 3);
	g_signal_connect( G_OBJECT(pComboRange), "changed", G_CALLBACK( cb_range_changed ), NULL );

	/* "Starting value of the zoom" combo box */
	pFrame = gtk_frame_new("Starting Value of the zoom (in Hz)");
	pComboZoom = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), pComboZoom);
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, FALSE, FALSE, 0);
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboZoom), "0");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboZoom), "500");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboZoom), "1000");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboZoom), "1500");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboZoom), "2000");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboZoom), "2500");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboZoom), "3000");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboZoom), "3500");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboZoom), "4000");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pComboZoom), 0);
	g_signal_connect( G_OBJECT(pComboZoom), "changed", G_CALLBACK( cb_zoom_changed ), NULL );

	gtk_widget_show_all (mainWindow);
	gtk_main ();

	return 0;
}


