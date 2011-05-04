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
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "config.h"

#include "scale.h"

char textToRender[200];
int xX = 0, yY = 0, zZ = 0;

/* Draw line scale and panels */
void drawScale() {
	GLfloat i = 0, alpha = 0;

if (z == 0) {
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

/* Print text scale */
void RenderText() {
	int i = 0;
	xX = 0, yY = 0, zZ = 0;
	
	glColor3f(0.7, 0, 0.8);
	glTranslatef(0, 0, 0.05);
	glRotatef(180, 1, 0, 0);
	glScalef(0.002, 0.002, 0.002);
	SDL_Color Color = {150, 0, 240};
	
	for (i = (zoom * 2); i <= ((zoom * 2) + (zoomFactor * 1000)); i+= (zoomFactor * 100)) {
	sprintf(textToRender, "%d", i);
			
			SDL_Surface *Message = TTF_RenderText_Blended(font, textToRender, Color);
			unsigned Texture = 0;

			glGenTextures(1, &Texture);
			glBindTexture(GL_TEXTURE_2D, Texture);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Message->w, Message->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, Message->pixels);
			
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3d(xX, yY, zZ);
			glTexCoord2d(1, 0); glVertex3d(xX+Message->w, yY, zZ);
			glTexCoord2d(1, 1); glVertex3d(xX+Message->w, yY+Message->h, zZ);
			glTexCoord2d(0, 1); glVertex3d(xX, yY+Message->h, zZ);
			glEnd();

			glDeleteTextures(1, &Texture);
			SDL_FreeSurface(Message);
			xX +=59 * RESIZE;
		}
}



