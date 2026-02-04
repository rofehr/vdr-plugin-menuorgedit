/*
 * menuorgedit.c: VDR plugin for editing menuorg.xml via OSD
 *
 * See the README file for copyright information and how to reach the author.
 */

#include "menuorgedit.h"
#include "menuorgmenu.h"
#include <vdr/plugin.h>
#include <getopt.h>

// --- cPluginMenuorgedit ----------------------------------------------------

cPluginMenuorgedit::cPluginMenuorgedit(void)
{
  // Initialize any member variables here.
  menuStructure = NULL;
  xmlParser = NULL;
  configDirectory = NULL;
}

cPluginMenuorgedit::~cPluginMenuorgedit()
{
  // Clean up after yourself!
  delete menuStructure;
  delete xmlParser;
  free(configDirectory);
}

const char *cPluginMenuorgedit::CommandLineHelp(void)
{
  return "  -c DIR,   --config=DIR   use DIR as config directory (default: /etc/vdr/plugins/menuorg)\n";
}

bool cPluginMenuorgedit::ProcessArgs(int argc, char *argv[])
{
  static struct option long_options[] = {
    { "config", required_argument, NULL, 'c' },
    { NULL, 0, NULL, 0 }
  };

  int c;
  while ((c = getopt_long(argc, argv, "c:", long_options, NULL)) != -1) {
    switch (c) {
      case 'c':
        configDirectory = strdup(optarg);
        break;
      default:
        return false;
    }
  }
  return true;
}

bool cPluginMenuorgedit::Initialize(void)
{
  // Initialize plugin - called after command line parsing
  if (!configDirectory)
    configDirectory = strdup("/etc/vdr/plugins/menuorg");

  menuStructure = new cMenuorgStructure();
  xmlParser = new cMenuorgXmlParser(configDirectory);
  
  return true;
}

bool cPluginMenuorgedit::Start(void)
{
  // Start any background activities the plugin needs
  // Load existing XML configuration
  if (!xmlParser->LoadXml(menuStructure)) {
    esyslog("menuorgedit: Failed to load menuorg.xml, using empty structure");
  }
  
  return true;
}

void cPluginMenuorgedit::Stop(void)
{
  // Stop plugin activity
}

void cPluginMenuorgedit::Housekeeping(void)
{
  // Perform any cleanup or regular tasks
}

void cPluginMenuorgedit::MainThreadHook(void)
{
  // Gets called once per second
}

cString cPluginMenuorgedit::Active(void)
{
  // Return message if plugin is currently active
  return NULL;
}

time_t cPluginMenuorgedit::WakeupTime(void)
{
  // Return custom wakeup time
  return 0;
}

cOsdObject *cPluginMenuorgedit::MainMenuAction(void)
{
  // Return a menu object when selected from main menu
  return new cMenuorgEditor(menuStructure, xmlParser);
}

cMenuSetupPage *cPluginMenuorgedit::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one
  return NULL;
}

bool cPluginMenuorgedit::SetupParse(const char *Name, const char *Value)
{
  // Parse setup parameters
  return false;
}

bool cPluginMenuorgedit::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginMenuorgedit::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands
  return NULL;
}

cString cPluginMenuorgedit::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands
  return NULL;
}

VDRPLUGINCREATOR(cPluginMenuorgedit); // Don't touch this!
