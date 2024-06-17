

#ifndef _COLORS_INCLUDED_
#define _COLORS_INCLUDED_

//Colors

COLORREF BLACK = RGB(0, 0, 0); 
COLORREF WHITE = RGB(255, 255, 255); 
COLORREF RED = RGB(0, 0, 255);
COLORREF BLUE = RGB(255, 0, 0);
COLORREF GREEN = RGB(0, 255, 0);
COLORREF PINK = RGB(255, 0, 255);
COLORREF main_color = BLACK;
COLORREF background_color = WHITE;

COLORREF diffusion;
COLORREF mirror;
COLORREF ambience;

COLORREF rc(){ return RGB(rand()%255, rand()%255, rand()%255); };

void ExtractRGBComponents(COLORREF color, int *red, int *green, int *blue) {
    *red = GetRValue(color);
    *green = GetGValue(color);
    *blue = GetBValue(color);
}

float minz;
float maxz;
float minx, maxx, miny, maxy;
float scaledownVar = 5;

COLORREF colorByHeight(float value){
	float range = maxz - minz;
    float normalizedValue = (value - minz) / range;
    int red, green, blue;
    if(normalizedValue < 0.5){
    	red = static_cast<int>(255 * normalizedValue);
    	green = static_cast<int>(255 * (0.5 - normalizedValue));
    	blue = 0;
	} else {
		red = 0;
    	green = static_cast<int>(255 * (1 - normalizedValue));
    	blue = static_cast<int>(255 * normalizedValue);
	}
     
    return RGB(red, green, blue);
}

#endif // COLORS_INCLUDED