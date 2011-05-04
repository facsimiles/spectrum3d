
#ifndef DEFINE_MAIN
#define DEFINE_MAIN

static GtkWidget *mainWindow;

char *sFile;
GSList *filenames;
char fontPreference[100];
int AUDIOFREQ, interval, pose, spect_bands, playing, typeSource, zoom, stringInt, range, counter, counterNumber, forward, backward, nSec, scale, firstPass3D, zoomFactor, textScale, showPanels, lineScale, change, f, onClickWidth, width;
float gain, yPanel, presetX, presetY, presetZ, presetAngleH, presetAngleV, presetAngleZ; 

FILE* conf;

#define RESIZE width/1200
#define WIDTH_WINDOW 1200 * RESIZE
#define HEIGHT_WINDOW 600

void sdlWindow(GtkWidget *pWidget, gpointer data);
void getFileName();
void onStop();
void onPlay();
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
void changeyPanel(GtkWidget *pWidget, gpointer data);
void changeDepth(GtkWidget *pWidget, gpointer data);
void onCheckLineScale(GtkWidget *pToggle, gpointer data);
void onCheckTextScale(GtkWidget *pToggle, gpointer data);
void onCheckShowPanels(GtkWidget *pToggle, gpointer data);
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

#endif
