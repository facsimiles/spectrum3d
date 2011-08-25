
#ifndef DEFINE_MAIN
#define DEFINE_MAIN

gboolean realtime, enableTouch, flatView, pointer;
char *sFile;
GSList *filenames;
char fontPreference[100];
gchar policyName[20];
int AUDIOFREQ, pose, playing, zoom, range, forward, backward, scale, firstPass3D, zoomFactor, textScale, showPanels, lineScale, change, f, onClickWidth, width, presetWidth, priority, intervalTimeout, widthFrame, bandsNumber, hzStep;
guint spect_bands;
guint64 interval;
float showGain, presetX, presetY, presetZ, presetAngleH, presetAngleV, presetAngleZ; 

FILE* pref;
static GtkWidget *mainWindow;
GtkWidget *pScaleDepth, *scaleButtonDepth, *pScaleBands, *pComboRange, *buttonEqualizer;
GtkObject *adjust_bands, *adjust_start;

typedef enum TypeSource TypeSource;
enum TypeSource
{
	MIC, AUDIO_FILE, JACK
};
TypeSource typeSource;

typedef enum ColorType ColorType;
enum ColorType
{
	PURPLE, RAINBOW, RED
};
ColorType colorType;

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
			
void initGstreamer();
void getFileName();
void defaultValues();
void makeDefaultPreferencesFile();
void onStop();
void onPlay();
void playFromSource();
void playTestSound(GtkWidget *pWidget, gpointer data);
void change_freq_test_sound(GtkWidget *widget, gpointer data);
void change_volume_test_sound(GtkWidget *pWidget, gpointer data);
void onSource(GtkWidget *pBtn, gpointer data);
void on_view(GtkWidget *pBtn, gpointer data);
void onReset();
void onFrontView();
void onPreset();
void cb_zoom_changed(GtkComboBox *combo, gpointer data);
void cb_speed_changed(GtkComboBox *combo, gpointer data);
void cb_range_changed(GtkWidget *combo, gpointer data);
void cb_scale_changed(GtkComboBox *combo, gpointer data);
void change_adjust(GtkWidget *pWidget, gpointer data);
void change_bands(GtkWidget *pWidget, gpointer data);
void change_start(GtkWidget *pWidget, gpointer data);
gchar* format_value_start (GtkScale *scale, gdouble value);
gchar* format_value_bands (GtkScale *scale, gdouble value);
gboolean changeGain(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data);
void changeDepth(GtkWidget *pWidget, gpointer data);
void onCheckLineScale(GtkWidget *pToggle, gpointer data);
void onCheckTextScale(GtkWidget *pToggle, gpointer data);
void onCheckPointer(GtkWidget *pToggle, gpointer data);
void effects_window();
void timeForward();
void timeBackward();
void stopSeek();
void onPreferences(GtkWidget* widget, gpointer data);
void changeWidth(GtkSpinButton *spinButton, gpointer user_data);
void saveChange(GtkWidget* widget, gpointer data);
void onQuickStart(GtkWidget* widget, gpointer data);
void onAbout(GtkWidget* widget, gpointer data);
void onDisplay(GtkWidget *pBtn, gpointer data);
void onShortcuts (GtkWidget* widget, gpointer data);
void onGesturesShortcuts (GtkWidget* widget, gpointer data);
void errorMessageWindow(char *message);

#endif



