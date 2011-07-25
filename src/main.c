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
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "config.h"

#ifdef HAVE_LIBUTOUCH_GEIS
	#include <geis/geis.h>
#endif

#include "main.h"

char prefPath[100];

/* Quit main window and eveything */
static void on_window_destroy (GObject * object, gpointer user_data)
{
	printf("Quit everything\nGood Bye!\n");
	if (sFile != NULL){
		g_free(sFile);
		}
	onStop();
	gtk_main_quit ();
}

/* Get the name of the file to be played */
void getFileName()
{
	if (filenames != NULL) {
		g_print ("Getting the file name from GSList");
		sFile = (gchar*) filenames->data;
		g_print ("%s was selected.\n", sFile);
		}
	else {
		g_print ("No file to read - GSList seems to be empty \nYou have to select a file");
		}
}

 /* Select audio file if "Audio File" Source is choosen" */
void selectFile(GtkWidget *pWidget, gpointer data) 
{
	
	if (typeSource == 1) {
		printf("Opening file selection dialog\n");
		GtkWidget *pFileSelection;
		GtkWidget *pParent;
		pParent = GTK_WIDGET(data);

		pFileSelection = gtk_file_chooser_dialog_new("Select File",
		GTK_WINDOW(pParent),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_OK,
		NULL);
		gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (pFileSelection), TRUE);
		gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);

		switch(gtk_dialog_run(GTK_DIALOG(pFileSelection)))
		{
		case GTK_RESPONSE_OK:
		       	filenames = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (pFileSelection));
			getFileName();
		default:
		    break;
		}
		gtk_widget_destroy(pFileSelection);
	}
	else {
		printf("Select a file is only possible if source is Audio file\n");
		}
}

void compareValues(){
	int error = 0;
	printf("Checking values for constistency...");
	if (width < 700 || width > 1200) {
		ERROR("width")
		DEFAULT_VALUES
		}
	char *lookforttf = NULL;
	lookforttf = strstr(fontPreference, ".ttf");
	if (lookforttf == NULL){
		printf("Path of the font seems not valid.\n");
		DEFAULT_VALUES
		}
	else if (realtime != 0 && realtime != 1) {
		ERROR("realtime")
		DEFAULT_VALUES	
		} 
	else if ((strcmp(policyName, "SCHED_RR") != 0) && (strcmp(policyName, "SCHED_FIFO") != 0)){
		printf("Realtime policy name seems not valid\n");
		DEFAULT_VALUES
		}
	else if (priority < 50 || priority > 80) {
		ERROR("priority")
		DEFAULT_VALUES	
		}
	else if (enableTouch != 0 && enableTouch != 1) {
		ERROR("enableTouch")
		DEFAULT_VALUES	
		}
	else {
		printf(" OK\n");
		}
}

void makeDefaultPreferencesFile() {
	printf("Writing default values in 'preferences' file\n");
	fprintf(pref, "%d\n", width);
	fprintf(pref, "%s\n", fontPreference);
	fprintf(pref, "%f\n", presetX);
	fprintf(pref, "%f\n", presetY);
	fprintf(pref, "%f\n", presetZ);
	fprintf(pref, "%f\n", presetAngleH);
	fprintf(pref, "%f\n", presetAngleV);
	fprintf(pref, "%f\n", presetAngleZ);
	fprintf(pref, "%d\n", realtime);
	fprintf(pref, "%s\n", policyName);
	fprintf(pref, "%d\n", priority);
	fprintf(pref, "%d\n", enableTouch);
}

void defaultValues(){
	printf("Setting values to default\n");
	struct stat fileStat;
	char fontHomePath[100];
	sprintf(fontHomePath, "%s/.%s/fonts/FreeSans.ttf", getenv("HOME"), PACKAGE);
	width = 850;
	if (stat("/usr/local/share/fonts/FreeSans.ttf", &fileStat) == 0) {
		sprintf(fontPreference, "/usr/local/share/fonts/FreeSans.ttf");
		}
	else if (stat("/usr/share/fonts/FreeSans.ttf", &fileStat) == 0) {
		sprintf(fontPreference, "/usr/share/fonts/FreeSans.ttf");
		}
	else if (stat(fontHomePath, &fileStat) == 0) {
		sprintf(fontPreference, "%s/.%s/fonts/FreeSans.ttf", getenv("HOME"), PACKAGE);
		}
	else {
		printf("WARNING : font file was not found; please select a '.ttf' font file via 'Menu->Edit->Preferences->Select Font'.\n");
		errorMessageWindow("WARNING : font file was not found \nPlease select a '.ttf' font file via 'Menu->Edit->Preferences->Select Font'.");
		sprintf(fontPreference, "null");
		} 
	printf("Font path is %s\n", fontPreference);
	presetX = -0.495833; 
	presetY = -0.070833;
	presetZ = -1.050000;
	presetAngleH = -16.000000;
	presetAngleV = 10.000000;
	presetAngleZ = 0.000000;
	realtime = 0;
	sprintf(policyName, "SCHED_RR");
	priority = 50;
	enableTouch = 1;
}

void openPreferenceFile(){
	char confString[105]="";
	/* Get the saved values from the configuration file */
	sprintf(prefPath, "%s/.spectrum3d/spectrum3d.pref", getenv("HOME"));
	pref = fopen(prefPath, "r+"); 
	if (pref != NULL){
		printf("Opening the 'preferences' file and getting the saved values\n");
		char *result = NULL;
		result = fgets(confString, 6, pref); 
			width = strtol(confString, NULL, 10);
		result = fgets(fontPreference, 100, pref);
			fontPreference[strlen(fontPreference)-1] = 0;
				printf("Font path is = %s\n", fontPreference);
		result = fgets(confString, 20, pref); 
			presetX = strtof(confString, NULL);
		result = fgets(confString, 20, pref); 
			presetY = strtof(confString, NULL);
		result = fgets(confString, 20, pref); 
			presetZ = strtof(confString, NULL);
		result = fgets(confString, 20, pref); 
			presetAngleH = strtof(confString, NULL);
		result = fgets(confString, 20, pref); 
			presetAngleV = strtof(confString, NULL);
		result = fgets(confString, 20, pref); 
			presetAngleZ = strtof(confString, NULL);
		result = fgets(confString, 4, pref); 
			realtime = strtol(confString, NULL, 10);
				if (realtime){
					printf("REALTIME mode is enabled : ");
					}
		result = fgets(policyName, 15, pref);
			policyName[strlen(policyName)-1] = 0;
				if (realtime){
					printf("Policy = %s, ", policyName);
					}
		result = fgets(confString, 6, pref); 
			priority = strtol(confString, NULL, 10); 
				if (realtime){
					printf("priority = %d\n", priority);
					}
		result = fgets(confString, 4, pref); 
			enableTouch = strtol(confString, NULL, 10);
#ifdef HAVE_LIBUTOUCH_GEIS
				if (enableTouch){
					printf("Multitouch is enabled\n");
					}
#endif
		fclose(pref);
		compareValues();
		printf(" ");	
		}
	else {  
//If the 'preferences file doesn't exist, create one with default values
		printf ("WARNING : 'Preferences' file doesn't exist or cannot be open; this is normal if you run Spectrum3d for the first time; \n");
		DEFAULT_VALUES
		}
}

int main(int argc, char *argv[])
{
	printf("%s \nPlease report any bug to %s\n", PACKAGE_STRING, PACKAGE_BUGREPORT);	
	
	typeSource = 0; 
	playing = 0;
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
	intervalTimeout = 100;
	
	openPreferenceFile();
				
	GtkWidget *pVBox[4], *pHBox[10], *pMenuBar, *pMenu, *pMenuItem, *pButton[7], *pButtonSelect, *pRadio[3], *pComboRange, *pComboZoom, *pComboSpeed, *pSpinSpeed, *pFrame, *pScaleGain, *pCheckTextScale, *pCheckLineScale;
	int i = 0;
	
	gtk_init (&argc, &argv); // create main window, vertical and horizontal boxes
	mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (mainWindow, 550, 380);
	gtk_window_set_title(GTK_WINDOW(mainWindow), "Spectrum3d : Real-Time Spectral Analysis");
	g_signal_connect (G_OBJECT (mainWindow), "destroy", G_CALLBACK (on_window_destroy), NULL);

	for (i = 0; i < 4; i++) {
		pVBox[i] = gtk_vbox_new(TRUE, 0);
		}
	for (i = 0; i < 10; i++) {
		pHBox[i] = gtk_hbox_new(TRUE, 0);
		}
	
	gtk_container_add(GTK_CONTAINER(mainWindow), pVBox[1]); 

	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[0], FALSE, TRUE, 0);
		
/* Menu */
	pMenuBar = gtk_menu_bar_new();

	pMenu = gtk_menu_new(); // 'Quit' submenu
	pMenuItem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);    
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(on_window_destroy), (GtkWidget*) mainWindow); 
	pMenuItem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);

	pMenu = gtk_menu_new(); // 'Edit' submenu
        pMenuItem = gtk_menu_item_new_with_label("Preferences");
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(onPreferences), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	pMenuItem = gtk_menu_item_new_with_label("Edit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);

	pMenu = gtk_menu_new(); // 'Help...' submenu
	pMenuItem = gtk_menu_item_new_with_label("Shortcuts"); 
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(onShortcuts), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
#ifdef HAVE_LIBUTOUCH_GEIS
	pMenuItem = gtk_menu_item_new_with_label("Gestures Shortcuts");
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(onGesturesShortcuts), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
#endif 
	pMenuItem = gtk_menu_item_new_with_label("About...");
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(onAbout), (GtkWidget*) mainWindow);
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
	g_signal_connect(G_OBJECT(pButton[0]), "clicked", G_CALLBACK(playFromSource), NULL);
	
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
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[7], TRUE, TRUE, 0);	
	gtk_box_pack_start(GTK_BOX(pHBox[7]), pFrame, TRUE, TRUE, 0);	
	pRadio[0] = gtk_radio_button_new_with_label(NULL, "Mic");
	gtk_container_add(GTK_CONTAINER(pFrame), pHBox[8]);
    	gtk_box_pack_start(GTK_BOX (pHBox[8]), pRadio[0], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[0]), "toggled", G_CALLBACK(onSource), pRadio[0]);
	pRadio[1] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio[0]), "Audio File");
    	gtk_box_pack_start(GTK_BOX (pHBox[8]), pRadio[1], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[1]), "toggled", G_CALLBACK(onSource), pRadio[0]);
#ifdef HAVE_LIBJACK
    	pRadio[2] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio[0]), "Jack");
    	gtk_box_pack_start(GTK_BOX (pHBox[8]), pRadio[2], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[0]), "toggled", G_CALLBACK(onSource), pRadio[0]);
#endif

/* 'Reset', 'Front' and 'Preset' buttons */
	gtk_box_pack_start(GTK_BOX(pHBox[7]), pHBox[9], FALSE, FALSE, 0);
	pButton[3] = gtk_button_new_with_label("Reset");
	gtk_box_pack_start(GTK_BOX(pHBox[9]), pButton[3], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton[3]), "clicked", G_CALLBACK(onReset), NULL);

	pButton[4] = gtk_button_new_with_label("Front");
	gtk_box_pack_start(GTK_BOX(pHBox[9]), pButton[4], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton[4]), "clicked", G_CALLBACK(onFrontView), NULL);

	pButton[5] = gtk_button_new_with_label("Preset");
	gtk_box_pack_start(GTK_BOX(pHBox[9]), pButton[5], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton[5]), "clicked", G_CALLBACK(onPreset), NULL);

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
	g_signal_connect( GTK_OBJECT(pComboZoom), "changed", G_CALLBACK( cb_zoom_changed ), NULL );

/* "Speed of display" combo box */
        pFrame = gtk_frame_new("Interval before \nnew frame (msec)");
	pComboSpeed = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), pComboSpeed);
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, TRUE, TRUE, 0);
	//gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "50");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "60");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "70");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "80");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "90");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "100");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "150");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "200");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "250");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "300");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "350");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "400");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "450");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "500");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pComboSpeed), 1);
	g_signal_connect( G_OBJECT(pComboSpeed), "changed", G_CALLBACK( cb_speed_changed ), NULL );
	
/* 'Text' and 'Lines' */
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[5], TRUE, TRUE, 0);
	pCheckTextScale = gtk_check_button_new_with_label("Text");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pCheckTextScale, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(pCheckTextScale), TRUE);
	g_signal_connect(G_OBJECT(pCheckTextScale), "toggled", G_CALLBACK(onCheckTextScale), NULL);

	pCheckLineScale = gtk_check_button_new_with_label("Lines");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pCheckLineScale, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(pCheckLineScale), TRUE);
	g_signal_connect(G_OBJECT(pCheckLineScale), "toggled", G_CALLBACK(onCheckLineScale), NULL);

/* 'Depth' Gtk Scale */
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


