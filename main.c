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
#include <gst/gst.h>
#include <string.h>
#include <gtk/gtk.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

gint64 pos;
static GtkWidget *mainWindow;
GtkWidget *pStatusTime, *preferencesWindow;
#include "functions.h"

/* Quit main window and eveything */
static void on_window_destroy (GObject * object, gpointer user_data)
{
  if (sFile != NULL){
	g_free(sFile);
	}
  mainWindow = NULL;
  onStop();
  gtk_main_quit ();
}

 /* Select audio file if "Audio File" Source is choosen" */
void selectFile(GtkWidget *pWidget, gpointer data) 
{
	if (typeSource == 1) {
		GtkWidget *pFileSelection;
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

void onPreferences(GtkWidget* widget, gpointer data)
{
	GtkWidget *pPreferences;
    	GtkWidget *pSpinWidth;
	GtkWidget *pFrame, *pHBox[2];
	pHBox[0] = gtk_vbox_new(TRUE, 0);
	//pHBox[1] = gtk_vbox_new(TRUE, 0);
	
        pPreferences = gtk_dialog_new_with_buttons("Preferences",
        GTK_WINDOW(mainWindow),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,GTK_RESPONSE_OK,
        GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
        NULL);

	/* SpinButton for choosing the width of the main window */
	pFrame = gtk_frame_new("Width of the window");
	pSpinWidth = gtk_spin_button_new_with_range(700, 1200, 50);
	gtk_container_add(GTK_CONTAINER(pFrame), pSpinWidth);
	gtk_box_pack_start(GTK_BOX(pHBox[0]), pFrame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[0], FALSE, FALSE, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(pSpinWidth), (float)width); 
	g_signal_connect(G_OBJECT(pSpinWidth), "value-changed", G_CALLBACK(changeWidth), NULL);

	gtk_widget_show_all(GTK_DIALOG(pPreferences)->vbox);
	
    	switch (gtk_dialog_run(GTK_DIALOG(pPreferences)))
		{
		case GTK_RESPONSE_OK:
	    		width = onClickWidth;
			conf = fopen("spectrum3d.conf", "w+");
			if (conf != NULL) {
				char confWidthWrite[4];
				sprintf(confWidthWrite, "%d", width);
				fputs(confWidthWrite, conf); 
				fclose(conf);
			    }
			else {
				printf("Cannot open 'spectrum3d.conf' configuration file");
				}
		printf("%d ", width);
	    	break;
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_NONE:
		default:
		    break;
	}
	gtk_widget_destroy(pPreferences);	
}

void OnAbout(GtkWidget* widget, gpointer data)
{
    GtkWidget *pAbout;
    pAbout = gtk_message_dialog_new (GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Spectrum3D-0.1.1\n"
        "http://www.presences.org");
    gtk_dialog_run(GTK_DIALOG(pAbout));
    gtk_widget_destroy(pAbout);
}

int main(int argc, char *argv[])
{
	char confWidth[5]="";

	typeSource = 0; // all the default values of the analysis or display parameters
	playing = 0;
	spect_bands = 11025 ;
	zoom = 0;
	stringInt = 6000;
	forward = 0;
	backward = 0;
	pose = 0;
	counterNumber = 1;
	scale = 0;
	firstPass3D = 1;
	zoomFactor = 6;
	gain = 0.2;
	yPanel = 0.4;
	textScale = 1;
	lineScale = 1;
	f = 2;
	conf = fopen("spectrum3d.conf", "r+"); // get the value of width from the 'spectrum3d.conf' file
	if (conf != NULL) {
		fgets(confWidth, 5, conf); 
		width = strtol(confWidth, NULL, 10);
		fclose(conf);
	    }
	else {
		printf("Cannot open 'spectrum3d.conf' configuration file : Window width set to default value (700)");
		width = 700;
	    }
		
	GtkWidget *pVBox[4], *pHBox[10], *pMenuBar, *pMenu, *pMenuItem, *pButton[3], *pButtonSelect, *pRadio[3], *pComboRange, *pComboZoom, *pComboSpeed, *pFrame, *pScaleGain, *pScalePanelHeight, *pCheckTextScale, *pCheckShowPanels, *pCheckLineScale, *pScaleDepth;
	
	gtk_init (&argc, &argv); // create main window, vertical and horizontal boxes
	mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (mainWindow, 550, 340);
	gtk_window_set_title(GTK_WINDOW(mainWindow), "Spectrum : Real-Time Spectral Analysis");
	g_signal_connect (G_OBJECT (mainWindow), "destroy", G_CALLBACK (on_window_destroy), NULL);

	pVBox[0] = gtk_vbox_new(TRUE, 0);
	pVBox[1] = gtk_vbox_new(TRUE, 0);
	pVBox[2] = gtk_vbox_new(TRUE, 0);
	pVBox[3] = gtk_vbox_new(TRUE, 0);

	pHBox[0] = gtk_hbox_new(TRUE, 0);
	pHBox[1] = gtk_hbox_new(TRUE, 0);
	pHBox[2] = gtk_hbox_new(TRUE, 0);
	pHBox[3] = gtk_hbox_new(TRUE, 0);
	pHBox[4] = gtk_hbox_new(TRUE, 0);
	pHBox[5] = gtk_hbox_new(TRUE, 0);
	pHBox[6] = gtk_hbox_new(TRUE, 0);
	pHBox[7] = gtk_hbox_new(TRUE, 0);
	pHBox[8] = gtk_hbox_new(TRUE, 0);
	pHBox[9] = gtk_hbox_new(TRUE, 0);

	gtk_container_add(GTK_CONTAINER(mainWindow), pVBox[1]); 

	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[0], FALSE, TRUE, 0);
	pMenuBar = gtk_menu_bar_new();
	
	pMenu = gtk_menu_new(); // 'Quit' submenu
	pMenuItem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);    
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(on_window_destroy), (GtkWidget*) mainWindow); 
	pMenuItem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);

	pMenu = gtk_menu_new();
        pMenuItem = gtk_menu_item_new_with_label("Preferences");
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(onPreferences), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	pMenuItem = gtk_menu_item_new_with_label("Edit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);

	pMenu = gtk_menu_new(); // 'About...' submenu
	pMenuItem = gtk_menu_item_new_with_label("About...");
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(OnAbout), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	pMenuItem = gtk_menu_item_new_with_label("Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);

	gtk_box_pack_start(GTK_BOX(pHBox[0]), pMenuBar, FALSE, TRUE, 0);

	/* "Seek Backward" button */
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[1], TRUE, TRUE, 0);
	pButton[0] = gtk_button_new_with_label("     << \n (5 sec)");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), pButton[0], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton[0]), "button-press-event", G_CALLBACK(timeBackward), NULL);
	g_signal_connect(G_OBJECT(pButton[0]), "button-release-event", G_CALLBACK(stopSeek), NULL);
	
	/* "Play/Pause" button */
	pButton[0] = gtk_button_new_with_label("Play/Pause");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), pButton[0], TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(pButton[0]), "clicked", G_CALLBACK(sdlWindow), NULL);

	/* "Stop" button */
	pButton[1] = gtk_button_new_with_label("STOP");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), pButton[1], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton[1]), "clicked", G_CALLBACK(onStop), NULL);

	/* "Seek Forward" button */
	pButton[2] = gtk_button_new_with_label("     >> \n (5 sec)");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), pButton[2], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton[2]), "button-press-event", G_CALLBACK(timeForward), NULL);
	g_signal_connect(G_OBJECT(pButton[2]), "button-release-event", G_CALLBACK(stopSeek), NULL);
 
	/* "Select File" button */
	pButtonSelect = gtk_button_new_with_mnemonic("Select File"); 
	gtk_box_pack_start(GTK_BOX(pHBox[1]), pButtonSelect, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButtonSelect), "clicked", G_CALLBACK(selectFile), NULL);

	/* "Mic", "Audio File" and "Jack" radio buttons */
	pFrame = gtk_frame_new("Source");
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pFrame, TRUE, TRUE, 0);	
	pRadio[0] = gtk_radio_button_new_with_label(NULL, "Mic");
	gtk_container_add(GTK_CONTAINER(pFrame), pHBox[7]);
    	gtk_box_pack_start(GTK_BOX (pHBox[7]), pRadio[0], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[0]), "toggled", G_CALLBACK(onSource), pRadio[0]);
	pRadio[1] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio[0]), "Audio File");
    	gtk_box_pack_start(GTK_BOX (pHBox[7]), pRadio[1], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[1]), "toggled", G_CALLBACK(onSource), pRadio[0]);
    	pRadio[2] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio[0]), "Jack");
    	gtk_box_pack_start(GTK_BOX (pHBox[7]), pRadio[2], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[0]), "toggled", G_CALLBACK(onSource), pRadio[0]);

	/* "Range of the zoom" combo box */
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[4], TRUE, TRUE, 0);
	pFrame = gtk_frame_new("Zoom Range");
	pComboRange = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), pComboRange);
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, TRUE, TRUE, 0);
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "1000 Hz");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "2000 Hz");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "4000 Hz");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "6000 Hz");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "10000 Hz");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), "20000 Hz");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pComboRange), 3);
	g_signal_connect( G_OBJECT(pComboRange), "changed", G_CALLBACK( cb_range_changed ), NULL );

	/* "Starting value of the zoom" combo box */
	pFrame = gtk_frame_new("Zoom Start (in Hz)");
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

	/* "Speed of display" combo box */
        pFrame = gtk_frame_new("Speed of display");
	pComboSpeed = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), pComboSpeed);
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, TRUE, TRUE, 0);
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 1 (fastest)");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 3");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 4");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 5");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "Speed 6 (slowest)");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pComboSpeed), 1);
	g_signal_connect( G_OBJECT(pComboSpeed), "changed", G_CALLBACK( cb_speed_changed ), NULL );

	/* 'Text', 'Lines' and 'Panels' check boxes */
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[5], TRUE, TRUE, 0);
	pCheckTextScale = gtk_check_button_new_with_label("Text");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pCheckTextScale, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(pCheckTextScale), TRUE);
	g_signal_connect(G_OBJECT(pCheckTextScale), "toggled", G_CALLBACK(onCheckTextScale), NULL);

	pCheckLineScale = gtk_check_button_new_with_label("Lines");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pCheckLineScale, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(pCheckLineScale), TRUE);
	g_signal_connect(G_OBJECT(pCheckLineScale), "toggled", G_CALLBACK(onCheckLineScale), NULL);

	pCheckShowPanels = gtk_check_button_new_with_label("Panels");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pCheckShowPanels, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pCheckShowPanels), "toggled", G_CALLBACK(onCheckShowPanels), NULL);

	/* 'Panel height' and 'Depth' Gtk Scale */
	pFrame = gtk_frame_new("Panel Height");
	pScalePanelHeight = gtk_hscale_new_with_range(0, 0.6, 0.01);
	gtk_container_add(GTK_CONTAINER(pFrame), pScalePanelHeight);
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pFrame, FALSE, TRUE, 0);
	gtk_range_set_value(GTK_RANGE(pScalePanelHeight), 0.4);
	g_signal_connect(G_OBJECT(pScalePanelHeight), "value-changed", G_CALLBACK(changeyPanel), NULL);

	pFrame = gtk_frame_new("Depth");
	pScaleDepth = gtk_hscale_new_with_range(0, 2, 0.1);
	gtk_container_add(GTK_CONTAINER(pFrame), pScaleDepth);
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pFrame, FALSE, TRUE, 0);
	gtk_range_set_value(GTK_RANGE(pScaleDepth), 1);
	g_signal_connect(G_OBJECT(pScaleDepth), "value-changed", G_CALLBACK(changeDepth), NULL);
	
	/* 'Gain' Gtk Scale */
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[6], FALSE, TRUE, 0);
	pFrame = gtk_frame_new("Gain");
	pScaleGain = gtk_hscale_new_with_range(0, 0.8, 0.001);
	gtk_container_add(GTK_CONTAINER(pFrame), pScaleGain);
	gtk_box_pack_start(GTK_BOX(pHBox[6]), pFrame, TRUE, TRUE, 0);
	gtk_range_set_value(GTK_RANGE(pScaleGain), 0.2);
	g_signal_connect(G_OBJECT(pScaleGain), "value-changed",
	G_CALLBACK(changeGain), NULL);

	gtk_widget_show_all (mainWindow);
	gtk_main ();

	return 0;
}


