/*
 * anyfont.h: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: anyfont.h,v 1.1 2004/03/03 00:22:47 schmitzj Exp $
 *
 */

#ifndef ANYFONT_H
#define ANYFONT_H

#include <vdr/plugin.h>

class anyFont
{
	cOsdBase *osd;
	const cFont::tPixelData *FontData;
	int FontHeight;
	int trans;
public:
	anyFont(cOsdBase *o,const cFont::tPixelData *fd,int fheight,int transparent=0);
	int Height(void);
	int Width(const char *txt);
	int LargeWidth(const char *txt);
	int Width(char c);
	int LargeWidth(char c);
	int Text(int x, int y, const char *txt, eDvbColor fg, eDvbColor bg, tWindowHandle wh);
	int LargeText(int x, int y, const char *txt, eDvbColor fg, eDvbColor bg, tWindowHandle wh);

	int Text(int x, int y, int w, int h, const char *txt, eDvbColor fg, eDvbColor bg, tWindowHandle wh);
	int TextHeight(int w, const char *txt);
};

#endif
