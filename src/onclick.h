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

#ifndef DEFINE_ONCLICK
#define DEFINE_ONCLICK

guint displaySpectroTimeout;
int playing, pose, typeSource, zoom, stringInt, range, counterNumber, forward, backward, nSec, scale, zoomFactor, textScale, showPanels, lineScale, change, f, onClickWidth, width, intervalTimeout;
float gain, yPanel, z, X, Y, Z, PROPORTION, AngleH, AngleV, AngleZ, presetX, presetY, presetZ, presetAngleH, presetAngleV, presetAngleZ; 

#define RESIZE width/1200

GMainLoop *loop;
GstElement *pipeline;
void sdlWindow(GtkWidget *pWidget, gpointer data);
void playFromFile(GtkWidget *pWidget, gpointer data);
void displaySpectro();

#endif
