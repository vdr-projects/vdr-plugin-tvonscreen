/*
 * config.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: config.h,v 1.5 2004/07/08 10:46:44 schmitzj Exp $
 *
 */

#ifndef TVONSCREEN_CONFIG_H
#define TVONSCREEN_CONFIG_H

#include <vdr/config.h>
#include <vdr/menuitems.h>

class tvonscreenConfig
{
public:

	tvonscreenConfig(void);
	~tvonscreenConfig();
	bool SetupParse(const char *Name, const char *Value);
	bool ProcessArgs(int argc, char *argv[]);
	const char *CommandLineHelp(void);

//  char showChannels[];
	int showLogos;
	int XLfonts;
	int noInfoLine;
	int showChannels;
	int bwlogos;
	int colorworkaround;
	int usertime1;
	int usertime2;
	int usertime3;
	int thenshownextday;

	char *logos;
};

extern tvonscreenConfig tvonscreenCfg;

class tvonscreenConfigPage : public cMenuSetupPage
{
private:
	tvonscreenConfig m_NewConfig;
	
protected:
	virtual void Store(void);

public:
	tvonscreenConfigPage(void);
	virtual ~tvonscreenConfigPage();
};

#endif 


