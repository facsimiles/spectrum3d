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

#ifndef DEF_FUNCTIONS
#define DEF_FUNCTIONS
char *sFile;
SDL_Surface *screen, *blackScreen;;
int AUDIOFREQ, pose, spect_bands, playing, typeSource, zoom, stringInt, range, counter, counterNumber, nCol, spinZoom, forward, backward, nSec, display, n;
GMainLoop *loop;
#define WIDTH_WINDOW 500
#define HEIGHT_WINDOW 500

void sdlWindow();
void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 getPIxel(SDL_Surface *surface, int x, int y);
void printLine();
void printScale();
void eraseScale();
void payPause();
void onSource(GtkWidget *pBtn, gpointer data);
void onDisplay();
void onStop();
void cb_zoom_changed(GtkComboBox *combo, gpointer data);
void cb_speed_changed(GtkComboBox *combo, gpointer data);
void cb_range_changed(GtkComboBox *combo, gpointer data);
void onDisplay(GtkWidget *pBtn, gpointer data);
void timeForward();
void timeBackward();
void stopSeek();
#endif
