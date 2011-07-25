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

#ifndef DEF_MAINWINDOW
#define DEF_MAINWINDOW

char *sFile;
gchar *uri, file[200];
GSList *filenames;
guint displaySpectroTimeout;
gint64 pos;
int AUDIOFREQ, pose, spect_bands, playing, typeSource, zoom, stringInt, range, counterNumber, forward, backward, nSec, scale, firstPass3D, zoomFactor, textScale, showPanels, lineScale, change, f, onClickWidth, width, intervalTimeout;
gboolean realtime;
float z, X, Y, Z, PROPORTION; 
GLfloat x;
GLfloat prec[205][10005];
GstElement *pipeline;
GMainLoop *loop;
const GValue *magnitudes;

GtkWidget *pScaleDepth;

#define RESIZE width/1200
GstElement *pipeline;

int displayPausedSpectro();
gboolean playSlowly();
void getFileName();
void displaySpectro();
gboolean sdlEvent();
void setupSDL();
void setupOpengl();
void setupGeis();
void sdlQuit();
void geisQuit();
void playFromJack();
void errorMessageWindow(char *message);
gboolean message_handler (GstBus * bus, GstMessage * message, gpointer data);
GstTaskPool *
test_rt_pool_new ();

#endif



