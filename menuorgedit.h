/*
 * menuorgedit.h: VDR plugin for editing menuorg.xml via OSD
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __MENUORGEDIT_H
#define __MENUORGEDIT_H

#include <vdr/plugin.h>
#include "menuorgstructure.h"
#include "menuorgxml.h"

static const char *VERSION        = "0.1.0";
static const char *DESCRIPTION    = "OSD Editor for menuorg.xml";
static const char *MAINMENUENTRY  = "Menu Editor";

class cPluginMenuorgedit : public cPlugin {
private:
  cMenuorgStructure *menuStructure;
  cMenuorgXmlParser *xmlParser;
  char *configDirectory;
  
public:
  cPluginMenuorgedit(void);
  virtual ~cPluginMenuorgedit();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual time_t WakeupTime(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

#endif // __MENUORGEDIT_H
