/*
 * menuorgmenu.h: OSD menu for editing menuorg structure
 */

#ifndef __MENUORGMENU_H
#define __MENUORGMENU_H

#include <vdr/osdbase.h>
#include "menuorgstructure.h"
#include "menuorgxml.h"

// --- cMenuorgItemEdit ------------------------------------------------------

class cMenuorgItemEdit : public cOsdMenu {
private:
  cMenuorgItem *item;
  eMenuItemType newType;
  char name[256];
  char title[256];
  char execute[512];
  int confirmExec;
  bool isNew;
  
  void SetupMenu(void);
  
public:
  cMenuorgItemEdit(cMenuorgItem *Item = NULL);
  virtual ~cMenuorgItemEdit();
  
  virtual eOSState ProcessKey(eKeys Key);
  
  cMenuorgItem *GetItem(void);
};

// --- cMenuorgEditor --------------------------------------------------------

class cMenuorgEditor : public cOsdMenu {
private:
  cMenuorgStructure *structure;
  cMenuorgXmlParser *xmlParser;
  cList<cMenuorgItem> displayList;
  int currentDepth;
  cMenuorgItem *selectedItem;
  
  void BuildDisplayList(void);
  void UpdateMenu(void);
  cMenuorgItem *GetSelectedItem(void);
  
  eOSState AddNewItem(void);
  eOSState EditItem(void);
  eOSState DeleteItem(void);
  eOSState MoveItemUp(void);
  eOSState MoveItemDown(void);
  eOSState CreateSubMenu(void);
  eOSState Save(void);
  
public:
  cMenuorgEditor(cMenuorgStructure *Structure, cMenuorgXmlParser *XmlParser);
  virtual ~cMenuorgEditor();
  
  virtual eOSState ProcessKey(eKeys Key);
};

#endif // __MENUORGMENU_H
