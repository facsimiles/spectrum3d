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
#include <stdarg.h>
#include <string.h>
#include <gst/gst.h>
#include <gtk/gtk.h>
#include <jack/jack.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "functions.h"

/* Draw line scale and panels */
void drawScale() {
	GLfloat i = 0, alpha = 0;

if (z == 0) {
		//x = 1.25;
	glBegin(GL_LINES);
	glColor3f(0.5, 0.5, 0.5);
	glVertex3f( 1.25, 0 * x, 0); 
	glVertex3f( 0, 0 * x, 0);
	glVertex3f(x, 0, 0);
	glVertex3f(0, 0, 0);
	glColor3f(0.9, 0.9, 0.9);
	glVertex3f( 0 * x, 0.6, 0); 
	glVertex3f( 0 * x, -0.05, 0);
	glVertex3f( 0.5 * x, 0.6, 0); 
	glVertex3f( 0.5 * x, -0.05, 0);
	glVertex3f( 1 * x, 0.6, 0); 
	glVertex3f( 1 * x, -0.05, 0);
	for (i = 0; i < 1; i+=0.1) {
		glColor3f(0.5, 0.5, 0.5);
		glVertex3f( i * x, 0.6, 0);
		glVertex3f( i * x, -0.03, 0);
		}
	for (i = 0; i < 1; i+=0.02) {
		glColor3f(0.2, 0.2, 0.2);
		glVertex3f( i * x, 0.6, 0);
		glVertex3f( i * x, -0.02, 0);
		}
glEnd();
		}

else {
	//x = 1.25;
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f( 0 * x, 0, -z); 
	glVertex3f( 0 * x, 0, 0.05);
	glVertex3f( 0.5 * x, 0, -z);
	glVertex3f( 0.5 * x, 0, 0.05);
	glVertex3f( 1 * x, 0, -z);
	glVertex3f( 1 * x, 0, 0.05);
	for (i = 0; i < 1; i+=0.1) {
		glColor3f(0.5, 0.5, 0.5);
		glVertex3f( i * x, 0, -z);
		glVertex3f( i * x, 0, 0.03);
		}
	for (i = 0; i < 1; i+=0.02) {
		glColor3f(0.2, 0.2, 0.2);
		glVertex3f( i * x, 0, -z);
		glVertex3f( i * x, 0, 0.02);
		}
glEnd();
}

	if (showPanels == 1) {
		alpha = 0.2;
		glBegin(GL_QUADS);
		glColor4f(1, 1, 1, alpha);
		for (i = 0; i <= 1; i+=0.1) {
			glVertex3f( i * x, 0, 0.05); 
			glVertex3f( i * x, 0, -z);
			glVertex3f( i * x, yPanel, -z);
			glVertex3f( i * x, yPanel, 0.05);
			}
		glVertex3f( 1 * x, 0, 0.05); 
		glVertex3f( 1 * x, 0, -z);
		glVertex3f( 1 * x, yPanel, -z);
		glVertex3f( 1 * x, yPanel, 0.05);
		glEnd();
		}
}


