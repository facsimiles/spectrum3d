
#ifndef DEFINE_MAIN
#define DEFINE_MAIN

static GtkWidget *mainWindow;
GtkWidget *pScaleDepth;

char *sFile;
GSList *filenames;
char fontPreference[100];
gchar policyName[20];
int AUDIOFREQ, pose, playing, typeSource, zoom, stringInt, range, counterNumber, forward, backward, nSec, scale, firstPass3D, zoomFactor, textScale, showPanels, lineScale, change, f, onClickWidth, width, priority, intervalTimeout;
gboolean realtime, enableTouch;
float gain, yPanel, presetX, presetY, presetZ, presetAngleH, presetAngleV, presetAngleZ; 

FILE* pref;

#define RESIZE width/1200
#define WIDTH_WINDOW 1200 * RESIZE
#define HEIGHT_WINDOW 600
#define ERROR(variable) printf("\n'%s' value is out of range.\n", variable);
#define DEFAULT_VALUES 	defaultValues();\
			pref = fopen(prefPath, "w+");\
			if (pref != NULL) {\
				printf("Creating a new 'preferences' file\n");\
				makeDefaultPreferencesFile();\
				fclose(pref);\
				}\
			else {	\
				printf("WARNING : Cannot create Preferences file\n");\
				errorMessageWindow("Cannot open or create 'preferences' file \nUsing default values");\
				defaultValues();\
				}
			
void sdlWindow(GtkWidget *pWidget, gpointer data);
void getFileName();
void defaultValues();
void makeDefaultPreferencesFile();
void onStop();
void onPlay();
void playFromSource();
void playFromFile(GtkWidget *pWidget, gpointer data);
void onSource(GtkWidget *pBtn, gpointer data);
void onReset();
void onFrontView();
void onPreset();
void cb_zoom_changed(GtkComboBox *combo, gpointer data);
void cb_speed_changed(GtkComboBox *combo, gpointer data);
void cb_range_changed(GtkWidget *combo, gpointer data);
void cb_scale_changed(GtkComboBox *combo, gpointer data);
void changeGain(GtkWidget *pWidget, gpointer data);
void changeDepth(GtkWidget *pWidget, gpointer data);
void onCheckLineScale(GtkWidget *pToggle, gpointer data);
void onCheckTextScale(GtkWidget *pToggle, gpointer data);
void timeForward();
void timeBackward();
void stopSeek();
void onPreferences(GtkWidget* widget, gpointer data);
void changeWidth(GtkSpinButton *spinButton, gpointer user_data);
void saveChange(GtkWidget* widget, gpointer data);
void onAbout(GtkWidget* widget, gpointer data);
void onDisplay(GtkWidget *pBtn, gpointer data);
void onShortcuts (GtkWidget* widget, gpointer data);
void onGesturesShortcuts (GtkWidget* widget, gpointer data);
void errorMessageWindow(char *message);

#endif



