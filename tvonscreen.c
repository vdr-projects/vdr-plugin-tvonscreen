/*
 * tvonscreen.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: tvonscreen.c,v 1.7 2004/03/23 09:28:47 schmitzj Exp $
 *
 */

#include <vdr/plugin.h>
#include "magazine.h"
#include "i18n.h"

static const char *VERSION        = "0.5.1";
static const char *DESCRIPTION    = "Shows the EPG info in form of a typical TV magazine";
static const char *MAINMENUENTRY  = "TV-OnScreen";

class cPluginTvOnscreen : public cPlugin {
private:

public:
  cPluginTvOnscreen(void);
  virtual ~cPluginTvOnscreen();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginTvOnscreen::cPluginTvOnscreen(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginTvOnscreen::~cPluginTvOnscreen()
{
  // Clean up after yourself!
}

const char *cPluginTvOnscreen::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginTvOnscreen::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginTvOnscreen::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
	RegisterI18n(tvoPhrases);
  	return true;
}

bool cPluginTvOnscreen::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginTvOnscreen::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginTvOnscreen::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.

	return new magazine(this);
}

cMenuSetupPage *cPluginTvOnscreen::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginTvOnscreen::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
/*  if      (!strcasecmp(Name, "TimeMorning"))           config.TimeMorning = atoi(Value);
  else if (!strcasecmp(Name, "TimeAfternoon"))         config.TimeAfternoon = atoi(Value);
  else if (!strcasecmp(Name, "TimeEvening"))           config.TimeEvening = atoi(Value);
  else if (!strcasecmp(Name, "NameUser"))              strn0cpy(config.NameUser, Value, MaxFileName);
  else if (!strcasecmp(Name, "TimeUser"))              config.TimeUser = atoi(Value);
  else if (!strcasecmp(Name, "ShowTimers"))            config.ShowTimers = (eShowTimers)atoi(Value);

  else
     return false;
*/
  return true;
}

VDRPLUGINCREATOR(cPluginTvOnscreen); // Don't touch this!
