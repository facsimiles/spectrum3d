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

#ifndef DEF_FUNCTIONS
#define DEF_FUNCTIONS
char *sFile;
int AUDIOFREQ, pose, spect_bands, playing, typeSource, zoom, stringInt, range, counter, counterNumber, forward, backward, nSec, scale, firstPass3D, zoomFactor, textScale, showPanels, lineScale, change, f, onClickWidth, width;
float gain, yPanel, z, X, Y, Z, PROPORTION; 
float AngleH, AngleV, AngleZ;
GLfloat x;
GMainLoop *loop;
GLfloat prec[205][10005];
const GValue *magnitudes;

#define RESIZE width/1200
#define WIDTH_WINDOW 1200 * RESIZE
#define HEIGHT_WINDOW 600

TTF_Font *font;
FILE* conf;

void sdlWindow();
void displaySpectro();
gboolean displayPausedSpectro();
void onSource(GtkWidget *pBtn, gpointer data);
void onStop();
void sdlEvent();
int sdlEventPaused();
gboolean playSlowly();
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
void onDisplay(GtkWidget *pBtn, gpointer data);
void timeForward();
void timeBackward();
void stopSeek();
void drawScale();
void RenderText();
void changeWidth(GtkSpinButton *spinButton, gpointer user_data);
void saveChange(GtkWidget* widget, gpointer data);
#endif


