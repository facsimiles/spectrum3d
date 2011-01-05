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

#include <stdlib.h>
#include <stdio.h>
#include <gst/gst.h>
#include <gtk/gtk.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "functions.h"

void printLine()
{
        int x = 0;
        for (x = 0 ; x< HEIGHT_WINDOW ; x++) // vertical line between the display and the scale
        {
            setPixel(screen, WIDTH_WINDOW + 1 , x, SDL_MapRGB(screen->format, 70, 70, 70));
        }
	for (x = 0 ; x< WIDTH_WINDOW ; x++) // horizontal line between the display and the scale
        {
            setPixel(screen, x , HEIGHT_WINDOW + 1, SDL_MapRGB(screen->format, 70, 70, 70));
        }
	if (display == 0){
		for (x = WIDTH_WINDOW + 1 ; x< WIDTH_WINDOW + 20 ; x++)//horizontal graduations; the higher 
		{
		    setPixel(screen, x, HEIGHT_WINDOW-2 , SDL_MapRGB(screen->format, 70, 70, 70));
		}

		for (x = WIDTH_WINDOW + 1 ; x< WIDTH_WINDOW + 10 ; x++) // then from the lowest to the higher
		{
		    setPixel(screen, x, ((HEIGHT_WINDOW/8)*7) , SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = WIDTH_WINDOW + 1 ; x< WIDTH_WINDOW + 15 ; x++)
		{
		    setPixel(screen, x, ((HEIGHT_WINDOW/4)*3) , SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = WIDTH_WINDOW + 1 ; x< WIDTH_WINDOW + 10 ; x++)
		{
		    setPixel(screen, x, ((HEIGHT_WINDOW/8)*5) , SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = WIDTH_WINDOW + 1 ; x< WIDTH_WINDOW + 20 ; x++)
		{
		    setPixel(screen, x, ((HEIGHT_WINDOW/2)*1) , SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = WIDTH_WINDOW + 1 ; x< WIDTH_WINDOW + 10 ; x++)
		{
		    setPixel(screen, x, ((HEIGHT_WINDOW/8)*3) , SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = WIDTH_WINDOW + 1 ; x< WIDTH_WINDOW + 15 ; x++)
		{
		    setPixel(screen, x, ((HEIGHT_WINDOW/4)*1) , SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = WIDTH_WINDOW + 1 ; x< WIDTH_WINDOW + 10 ; x++)
		{
		    setPixel(screen, x, ((HEIGHT_WINDOW/8)*1) , SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = WIDTH_WINDOW + 1 ; x< WIDTH_WINDOW + 20 ; x++) // the lowest graduation
		{
		    setPixel(screen, x, 1, SDL_MapRGB(screen->format, 70, 70, 70));
		}
	}
	
	else if (display == 1){
		for (x = HEIGHT_WINDOW + 1 ; x< HEIGHT_WINDOW + 20 ; x++)//horizontal graduations; the higher 
		{
		    setPixel(screen, WIDTH_WINDOW, x, SDL_MapRGB(screen->format, 70, 70, 70));
		}

		for (x = HEIGHT_WINDOW + 1 ; x< HEIGHT_WINDOW + 10 ; x++) // then from the lowest to the higher
		{
		    setPixel(screen, ((WIDTH_WINDOW/8)*7), x, SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = HEIGHT_WINDOW + 1 ; x< HEIGHT_WINDOW + 15 ; x++)
		{
		    setPixel(screen, ((WIDTH_WINDOW/4)*3), x, SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = HEIGHT_WINDOW + 1 ; x< HEIGHT_WINDOW + 10 ; x++)
		{
		    setPixel(screen, ((WIDTH_WINDOW/8)*5), x, SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = HEIGHT_WINDOW + 1 ; x< HEIGHT_WINDOW + 20 ; x++)
		{
		    setPixel(screen, ((WIDTH_WINDOW/2)*1), x, SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = HEIGHT_WINDOW + 1 ; x< HEIGHT_WINDOW + 10 ; x++)
		{
		    setPixel(screen, ((WIDTH_WINDOW/8)*3), x, SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = HEIGHT_WINDOW + 1 ; x< HEIGHT_WINDOW + 15 ; x++)
		{
		    setPixel(screen, ((WIDTH_WINDOW/4)*1), x, SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = HEIGHT_WINDOW + 1 ; x< HEIGHT_WINDOW + 10 ; x++)
		{
		    setPixel(screen, ((WIDTH_WINDOW/8)*1), x, SDL_MapRGB(screen->format, 70, 70, 70));
		}
		for (x = HEIGHT_WINDOW + 1 ; x< HEIGHT_WINDOW + 20 ; x++) // the lowest graduation
		{
		    setPixel(screen, 1, x, SDL_MapRGB(screen->format, 70, 70, 70));
		}
	}
}	

void printScale() // the scale of graduations
{
    int i = 0, f = 0, posY = 0, posX = 0;
    SDL_Surface *texteEchelle[7], *texteEchelleHaut, *texteEchelleBas;
    char texte[12][7];
    char texteHaut[12], texteBas[12];
    TTF_Init();
    TTF_Font *police = TTF_OpenFont("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf", 12);
    SDL_Rect position;
    SDL_Color blanc = {255, 255, 255};
    if (display == 0){
	    sprintf(texteHaut, "%d Hz", (spinZoom + stringInt));
	    texteEchelleHaut=TTF_RenderText_Blended(police, texteHaut, blanc);
	    position.x = WIDTH_WINDOW + 25; position.y = 1;
	    SDL_BlitSurface(texteEchelleHaut, NULL, screen, &position);

	    for (i=0; i<7 ; i++)
	    {
		f = ((spinZoom) + (stringInt *(i+1)/8));
		sprintf(texte[i], "%d Hz", f);
		posY = ((HEIGHT_WINDOW * (7-i)) /8) - 7;
		position.x = WIDTH_WINDOW + 22; position.y = posY;
		texteEchelle[i] = TTF_RenderText_Blended(police, texte[i], blanc);
		SDL_BlitSurface(texteEchelle[i], NULL, screen, &position);
	    }

	    sprintf(texteBas, "%d Hz", spinZoom);
	    texteEchelleBas=TTF_RenderText_Blended(police, texteBas, blanc);
	    position.x = WIDTH_WINDOW + 25; position.y = HEIGHT_WINDOW - 15;
	    SDL_BlitSurface(texteEchelleBas, NULL, screen, &position);
	}
    else if (display == 1){
	    sprintf(texteHaut, "%d Hz", (spinZoom + stringInt));
	    texteEchelleHaut=TTF_RenderText_Blended(police, texteHaut, blanc);
	    position.x = WIDTH_WINDOW - 10; position.y = HEIGHT_WINDOW + 25;
	    SDL_BlitSurface(texteEchelleHaut, NULL, screen, &position);

	    for (i=0; i<7 ; i++)
	    {
		f = ((spinZoom) + (stringInt *(i+1)/8));
		sprintf(texte[i], "%d ", f);
		posX = ((WIDTH_WINDOW * (i+1)) /8) - 12;
		position.x = posX; position.y = HEIGHT_WINDOW + 25;
		texteEchelle[i] = TTF_RenderText_Blended(police, texte[i], blanc);
		SDL_BlitSurface(texteEchelle[i], NULL, screen, &position);
	    }

	    sprintf(texteBas, "%d Hz", spinZoom);
	    texteEchelleBas=TTF_RenderText_Blended(police, texteBas, blanc);
	    position.x = 1; position.y = HEIGHT_WINDOW + 25;
	    SDL_BlitSurface(texteEchelleBas, NULL, screen, &position);
	}
}

void eraseScale()
{
    int i = 0, f = 0, posY = 0, posX = 0;
    SDL_Surface *blackSurface = NULL;
    blackSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, 80, 40, 32, 0, 0, 0, 0);
    SDL_Rect position;

	    position.x = WIDTH_WINDOW + 25; position.y = 1;
	    SDL_BlitSurface(blackSurface, NULL, screen, &position);

	    for (i=0; i<7 ; i++)
	    {
		posY = ((HEIGHT_WINDOW * (7-i)) /8) - 7;
		position.x = WIDTH_WINDOW + 22; position.y = posY;
		SDL_BlitSurface(blackSurface, NULL, screen, &position);
	    }

	    position.x = WIDTH_WINDOW + 25; position.y = HEIGHT_WINDOW - 15;
	    SDL_BlitSurface(blackSurface, NULL, screen, &position);
	
	    position.x = WIDTH_WINDOW + 25; position.y = 1;
	    SDL_BlitSurface(blackSurface, NULL, screen, &position);

	    for (i=0; i<8 ; i++)
	    {
		posX = ((WIDTH_WINDOW * (7-i)) /8) - 15;
		position.x = posX; position.y = WIDTH_WINDOW + 25;
		SDL_BlitSurface(blackSurface, NULL, screen, &position);
	    }

	    position.x = WIDTH_WINDOW -10; position.y = HEIGHT_WINDOW + 25;
	    SDL_BlitSurface(blackSurface, NULL, screen, &position);
}


