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

/* Print text scale */
void RenderText() {
	int X = 0, Y = 0, Z = 0, i = 0;
	char textScale[200];
	
	glColor3f(0.7, 0, 0.8);
	glTranslatef(0, 0, 0.05);
	glRotatef(180, 1, 0, 0);
	glScalef(0.002, 0.002, 0.002);
	SDL_Color Color = {150, 0, 240};
	
	for (i = (zoom * 2); i <= ((zoom * 2) + (zoomFactor * 1000)); i+= (zoomFactor * 100)) {
	sprintf(textScale, "%d", i);
			
			SDL_Surface *Message = TTF_RenderText_Blended(font, textScale, Color);
			unsigned Texture = 0;

			glGenTextures(1, &Texture);
			glBindTexture(GL_TEXTURE_2D, Texture);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Message->w, Message->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, Message->pixels);
			
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3d(X, Y, Z);
			glTexCoord2d(1, 0); glVertex3d(X+Message->w, Y, Z);
			glTexCoord2d(1, 1); glVertex3d(X+Message->w, Y+Message->h, Z);
			glTexCoord2d(0, 1); glVertex3d(X, Y+Message->h, Z);
			glEnd();

			glDeleteTextures(1, &Texture);
			SDL_FreeSurface(Message);
			X +=59 * RESIZE;
		}
}


