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
#include <gst/gst.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "config.h"

#include "display.h"

int ii = 0, a = 0, b = 0;
float cr = 0, cb = 1, cg = 0, k = 0;
float i = 0, l = 0, q = 0;

void setupSDL()
{
	printf("Init SDL\n");
	if(TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
		}
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
		}
	SDL_SetVideoMode(WIDTH_WINDOW, HEIGHT_WINDOW, 24, SDL_OPENGL | SDL_GL_DOUBLEBUFFER);
	SDL_WM_SetCaption(PACKAGE_NAME, NULL);
	//font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 16 * RESIZE);
	font = TTF_OpenFont(fontPreference, 16 * RESIZE);
}

void sdlQuit()
{
	TTF_CloseFont(font);
        SDL_Quit();
	TTF_Quit();
}

void setupOpengl() {
	printf("Init OpenGL\n");
	glViewport(0, 0, WIDTH_WINDOW, HEIGHT_WINDOW);
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gluPerspective(45, (float)WIDTH_WINDOW/HEIGHT_WINDOW, 0.1, 200);
	glMatrixMode(GL_MODELVIEW);
}

/* Draw the spectrogram while playing */
gboolean displaySpectro(){

glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
glLoadIdentity();

ii = 0, a = 0, b = 0;
cr = 0, cb = 1, cg = 0, k = 0;
i = 0, l = 0, q = 0;

if (magnitudes != NULL) {
		if (firstPass3D) {
			AngleH = -16.0;
			AngleV = 10.0;
			firstPass3D = 0;
			}
		
	glTranslatef(X, Y, Z);

	glRotatef(AngleV, 1, 0, 0);
	glRotatef(AngleH, 0, 1, 0);
	glRotatef(AngleZ, 0, 0, 1);
	glBegin(GL_QUADS);

//then draw
for (ii = zoom; ii < 10000 + zoom; ii+=zoomFactor) {
	l = -z;
	cr = 0.6, cb = 1, cg = 0;
	q = x/500;
	for (b = 100/f * z; b >= 0; b--) {
		k = 0;
		//calculate the mean value to use; after use of each value, store it one step further
		for (a = 0; a < zoomFactor; a++) {
			k = k + prec[b][ii+a];
			prec[b+1][ii+a] = prec[b][ii+a];
			}
		k = (k/zoomFactor) * gain;
		if (i < x) {
			glColor3f(cr ,cg, cb);
			glVertex3f( i, 0, l);
			glVertex3f( i, k ,l);
			glVertex3f( (i+q), k, l);
			glVertex3f( (i+q), 0 ,l);
			}
			if (b > (25/f * z) && b < (90/f * z)) {
				cg+=f * 0.01538/z;
				}
			if (b < (25/f * z)) {
				cg-=f * 0.04/z;
				} 
			if (b < (50/f * z)) {
				cr+=f * 0.02/z;
				} 
			if (b > (70/f * z)) {
				cr-=f * 0.02/z;
				} 
			if (b > (30/f * z) && b < (90/f * z)) {
				cb-=f * 0.01666/z;
				} 
		l+=0.01 * f;
		}
	
		k = 0;
		for (a = 0; a < zoomFactor; a++) {
			k = k + prec[0][ii+a];
			prec[1][ii+a] = prec[0][ii+a];
			}
		k = (k/zoomFactor) * gain;
	
		if (i < x) {
			glColor3f(1, 0.3, 0.5);
			glVertex3f( i, 0, 0);
			glVertex3f( i, k ,0);
			glVertex3f( (i+q), k, 0);
			glVertex3f( (i+q), 0 ,0);
			}
		i += q;			
	}
}
glEnd();

if (lineScale == 1) {
	drawScale();
	}
if (textScale == 1) {
	RenderText();
	}
SDL_GL_SwapBuffers();
if (pose == 0) {
	return TRUE;
	}
else {
	return FALSE;
	}
}

/* Draw the spectrogram while paused (image is frozen) */
gboolean displayPausedSpectro()
{
if (result != 0 || change) {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
	glLoadIdentity();

	ii = 0, a = 0, b = 0;
	cr = 0, cb = 1, cg = 0, k = 0;
	i = 0, l = 0, q = 0;

		if (magnitudes != NULL) {
			if (firstPass3D) {
				AngleH = -16.0;
				AngleV = 10.0;
				firstPass3D = 0;
				}
	
		glTranslatef(X, Y, Z);

		glRotatef(AngleV, 1, 0, 0);
		glRotatef(AngleH, 0, 1, 0);
		glRotatef(AngleZ, 0, 0, 1);
		glBegin(GL_QUADS);

		for (ii = zoom; ii < 10000 + zoom; ii+=zoomFactor) {
			l = -z;
			cr = 0.6, cb = 1, cg = 0;
			q = x/500;
			for (b = 100/f * z; b >= 0; b--) {
				k = 0;
				for (a = 0; a < zoomFactor; a++) {
					k = k + prec[b][ii+a];
					}
				k = (k/zoomFactor) * gain;
				if (i < x) {
					glColor3f(cr ,cg, cb);
					glVertex3f( i, 0, l);
					glVertex3f( i, k ,l);
					glVertex3f( (i+q), k, l);
					glVertex3f( (i+q), 0 ,l);
					}
					if (b > (25/f * z) && b < (90/f * z)) {
						cg+=f * 0.01538/z;
						}
					if (b < (25/f * z)) {
						cg-=f * 0.04/z;
						} 
					if (b < (50/f * z)) {
						cr+=f * 0.02/z;
						} 
					if (b > (70/f * z)) {
						cr-=f * 0.02/z;
						} 
					if (b > (30/f * z) && b < (90/f * z)) {
						cb-=f * 0.01666/z;
						} 
				l+=0.01 * f;
				}

	
				k = 0;
				for (a = 0; a < zoomFactor; a++) {
					k = k + prec[0][ii+a];
					}
				k = (k/zoomFactor) * gain;
				if (i < x) {
					glColor3f(1, 0.3, 0.5);
					glVertex3f( i, 0, 0);
					glVertex3f( i, k ,0);
					glVertex3f( (i+q), k, 0);
					glVertex3f( (i+q), 0 ,0);
					}
				i += q;		
				}
			}
	glEnd();

	if (lineScale == 1) {
		drawScale();
		}
	if (textScale == 1) {
		RenderText();
		}
	SDL_GL_SwapBuffers();
	change = 0;
	}
if (pose == 0) {
	return FALSE;
	}
else {
	return TRUE;
	}

}




