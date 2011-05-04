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

#include "config.h"

#include "menu.h"

/* Select font */
void selectFont(GtkWidget *pWidget, gpointer data)
{
	GtkWidget *pFileSelection;
	GtkWidget *pParent;
	pParent = GTK_WIDGET(data);
	gchar *selectedFont;

	pFileSelection = gtk_file_chooser_dialog_new("Select File",
	GTK_WINDOW(pParent),
	GTK_FILE_CHOOSER_ACTION_OPEN,
	GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	GTK_STOCK_OPEN, GTK_RESPONSE_OK,
	NULL);
	gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);

	switch(gtk_dialog_run(GTK_DIALOG(pFileSelection))) {
		case GTK_RESPONSE_OK:
			selectedFont = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(pFileSelection));
			sprintf(fontPreference, "%s", selectedFont);
			printf("Selected font is %s\n", fontPreference);
		    break;
		default:
		    break;
		}
	gtk_widget_destroy(pFileSelection);
}

/* Preferences window */
void onPreferences(GtkWidget* widget, gpointer data)
{
	GtkWidget *pPreferences;
    	GtkWidget *pSpinWidth, *pSpinInterval, *pLabel, *pCheckPreset;
	GtkWidget *pFrame, *pHBox[10], *pButtonSelectFont;
	gboolean bState;
	int i = 0;
	for (i = 0; i < 10; i++) {
		pHBox[i] = gtk_hbox_new(TRUE, 0);
		}
		
        pPreferences = gtk_dialog_new_with_buttons("Preferences",
        GTK_WINDOW(mainWindow),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,GTK_RESPONSE_OK,
        GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
        NULL);
	gtk_window_set_default_size(GTK_WINDOW(pPreferences), 300, 300);

	/* SpinButton for choosing the width of the main window */
	pFrame = gtk_frame_new("Width of the window");
	pSpinWidth = gtk_spin_button_new_with_range(700, 1200, 50);
	gtk_container_add(GTK_CONTAINER(pFrame), pSpinWidth);
	gtk_box_pack_start(GTK_BOX(pHBox[0]), pFrame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[0], FALSE, FALSE, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(pSpinWidth), (float)width); 
	
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[1], TRUE, FALSE, 0);

	/* Button to select the font */
	pButtonSelectFont = gtk_button_new_with_mnemonic("Select Font"); 
	gtk_box_pack_start(GTK_BOX(pHBox[2]), pButtonSelectFont, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[2], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButtonSelectFont), "clicked", G_CALLBACK(selectFont), NULL);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[3], TRUE, FALSE, 0);

	/* SpinButton for choosing the speed */
	pFrame = gtk_frame_new("Interval");
	pSpinInterval = gtk_spin_button_new_with_range(10, 20, 1);
	gtk_container_add(GTK_CONTAINER(pFrame), pSpinInterval);
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[4], FALSE, FALSE, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(pSpinInterval), interval);

	/* Warning label */
	pLabel=gtk_label_new("Warning : too low interval can cause overload of your system.");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[5], FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(pHBox[5]), pLabel);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[6], TRUE, FALSE, 0);

	/* Check button to save the current position as 'Preset' */
	pCheckPreset = gtk_check_button_new_with_label("Save current position as preset");
	gtk_box_pack_start(GTK_BOX(pHBox[7]), pCheckPreset, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[7], FALSE, FALSE, 0);
		
	gtk_widget_show_all(GTK_DIALOG(pPreferences)->vbox);
	
    	switch (gtk_dialog_run(GTK_DIALOG(pPreferences)))
		{
		case GTK_RESPONSE_OK:
	    		width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pSpinWidth));
			interval = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pSpinInterval));
			conf = fopen(configFile, "w+");
			if (conf != NULL) {
				fprintf(conf, "%d\n", width);
				fprintf(conf, "%s\n", fontPreference);
				fprintf(conf, "%d\n", interval);
				bState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pCheckPreset));
					if (bState) {
						presetX = X;
							fprintf(conf, "%f\n", X);
						presetY = Y;
							fprintf(conf, "%f\n", Y);
						presetZ = Z;
							fprintf(conf, "%f\n", Z);
						presetAngleH = AngleH;
							fprintf(conf, "%f\n", AngleH);
						presetAngleV = AngleV;
							fprintf(conf, "%f\n", AngleV);
						presetAngleZ = AngleZ;
							fprintf(conf, "%f\n", AngleZ);
					}
					else {
						fprintf(conf, "%f\n", presetX);
						fprintf(conf, "%f\n", presetY);
						fprintf(conf, "%f\n", presetZ);
						fprintf(conf, "%f\n", presetAngleH);
						fprintf(conf, "%f\n", presetAngleV);
						fprintf(conf, "%f\n", presetAngleZ);
						}
				fclose(conf);
			    }
			else {
				printf("*** WARNING ***  Cannot open configuration file\n");
				}
		//printf("Main window's width is %d\n ", width);
		//printf("Used font is %s\n ", fontPreference);
	    	break;
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_NONE:
		default:
		    break;
	}
	gtk_widget_destroy(pPreferences);	
}

/* Keyboard and Mouse shortcuts menu */
void onShortcuts (GtkWidget* widget, gpointer data)
{
	GtkWidget *shortcutsWindow, *pListView, *pScrollbar;
	GtkListStore *pListStore;
	GtkTreeViewColumn *pColumn;
	GtkCellRenderer *pCellRenderer;
	gchar *action[15] = {"Play/Pause", "Stop", "Rotate around X axis", "Rotate around Y axis", "Rotate around Z axis", "Translate along X axis", "Translate along Y axis", "Translate along Z axis", "Increase/decrease Gain", "Change starting value of the zoom", "Read small 50 msec samples	of a paused audio file" };
	gchar *keyboard[15] = {"Space bar", "Escape", "Up/Down", "Right/Left", "'c' + Up/Down", "'x' + Left/Right", "'y' + Up/Down", "'z' + Up/Down", "'g' + Up/Down", "'s' + Up/Down", "'v' + Left"};
	gchar *mouse[15] = {"", "", "Mouse Up/Down", "Mouse Right/Left", "'c' + mouse Up/Down", "'x' + mouse Right/Left", "'y' + mouse Up/Down", "'z' + mouse Up/Down", "'g' + mouse Up/Down"};
	gint i;

    	shortcutsWindow = gtk_dialog_new_with_buttons("Keyboard and mouse shortcuts",
        GTK_WINDOW(mainWindow),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,GTK_RESPONSE_OK,
        NULL);
	gtk_window_set_default_size(GTK_WINDOW(shortcutsWindow), 700, 400);
	
	pListStore = gtk_list_store_new(N_COLUMN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	
	for(i = 0 ; i < 15 ; ++i){
		GtkTreeIter pIter;
		gtk_list_store_append(pListStore, &pIter);
		gtk_list_store_set(pListStore, &pIter,
		    ACTION_COLUMN, action[i],
		    KEYBOARD_COLUMN, keyboard[i],
		    MOUSE_COLUMN, mouse[i],
		    -1);
	}
	
	pListView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(pListStore));

	/* 1st column */
        pCellRenderer = gtk_cell_renderer_text_new();
    	pColumn = gtk_tree_view_column_new_with_attributes("ACTION",
        	pCellRenderer,
		"text", ACTION_COLUMN,
		NULL);

    	gtk_tree_view_append_column(GTK_TREE_VIEW(pListView), pColumn);

	/* 2d column */
	pCellRenderer = gtk_cell_renderer_text_new();
   	pColumn = gtk_tree_view_column_new_with_attributes("KEYBORD SHORTCUT",
		pCellRenderer,
		"text", KEYBOARD_COLUMN,
		NULL);

        gtk_tree_view_append_column(GTK_TREE_VIEW(pListView), pColumn);

	/* 3d column */
	pCellRenderer = gtk_cell_renderer_text_new();
   	pColumn = gtk_tree_view_column_new_with_attributes("KEYBORD SHORTCUT",
		pCellRenderer,
		"text", MOUSE_COLUMN,
		NULL);

        gtk_tree_view_append_column(GTK_TREE_VIEW(pListView), pColumn);

	pScrollbar = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pScrollbar),
		GTK_POLICY_AUTOMATIC,
		GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(pScrollbar), pListView);

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(shortcutsWindow)->vbox), pScrollbar);


	gtk_widget_show_all(GTK_DIALOG(shortcutsWindow)->vbox);
	
    	switch (gtk_dialog_run(GTK_DIALOG(shortcutsWindow)))
		{
		case GTK_RESPONSE_OK:
	    		break;
		default:
		    break;
	}
	gtk_widget_destroy(shortcutsWindow);
}
 
/* Gestures menu */
void onGesturesShortcuts (GtkWidget* widget, gpointer data)
{
	GtkWidget *shortcutsWindow, *pListView, *pScrollbar;
	GtkListStore *pListStore;
	GtkTreeViewColumn *pColumn;
	GtkCellRenderer *pCellRenderer;
	gchar *action[8] = {"Play/Pause", "Rotate around X axis                   ", "Rotate around Y axis", "Rotate around Z axis", "Translate along X axis", "Translate along Y axis", "Translate along Z axis"};
	gchar *keyboard[8] = {"Double Tap", "1 Finger Drag Up/Down", "1 Finger Drag Right/Left", "2 Fingers Rotate", "2 Fingers Drag Right/Left", "2 Fingers Drag Up/Down", "2 Fingers Pinch"};
	gint i;

    	shortcutsWindow = gtk_dialog_new_with_buttons("Keyboard and mouse shortcuts",
        GTK_WINDOW(mainWindow),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,GTK_RESPONSE_OK,
        NULL);
	gtk_window_set_default_size(GTK_WINDOW(shortcutsWindow), 500, 300);
	
	pListStore = gtk_list_store_new(N_COLUMN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	
	for(i = 0 ; i < 8 ; ++i){
		GtkTreeIter pIter;
		gtk_list_store_append(pListStore, &pIter);
		gtk_list_store_set(pListStore, &pIter,
		    ACTION_COLUMN, action[i],
		    KEYBOARD_COLUMN, keyboard[i],
		    -1);
	}
	
	pListView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(pListStore));

	/* 1st column */
        pCellRenderer = gtk_cell_renderer_text_new();
    	pColumn = gtk_tree_view_column_new_with_attributes("ACTION",
        	pCellRenderer,
		"text", ACTION_COLUMN,
		NULL);

    	gtk_tree_view_append_column(GTK_TREE_VIEW(pListView), pColumn);

	/* 2d column */
	pCellRenderer = gtk_cell_renderer_text_new();
   	pColumn = gtk_tree_view_column_new_with_attributes("GESTURE",
		pCellRenderer,
		"text", KEYBOARD_COLUMN,
		NULL);

        gtk_tree_view_append_column(GTK_TREE_VIEW(pListView), pColumn);

	pScrollbar = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pScrollbar),
		GTK_POLICY_AUTOMATIC,
		GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(pScrollbar), pListView);

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(shortcutsWindow)->vbox), pScrollbar);


	gtk_widget_show_all(GTK_DIALOG(shortcutsWindow)->vbox);
	
    	switch (gtk_dialog_run(GTK_DIALOG(shortcutsWindow)))
		{
		case GTK_RESPONSE_OK:
	    		break;
		default:
		    break;
	}
	gtk_widget_destroy(shortcutsWindow);
}

/* About menu */
void onAbout(GtkWidget* widget, gpointer data)
{
    GtkWidget *pAbout;
    pAbout = gtk_message_dialog_new (GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Spectrum3D-0.1.2\n"
        "http://www.presences.org");
    gtk_dialog_run(GTK_DIALOG(pAbout));
    gtk_widget_destroy(pAbout);
}


