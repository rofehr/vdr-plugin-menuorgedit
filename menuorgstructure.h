/*
 * menuorgstructure.h: Data model for menuorg structure
 */

#ifndef __MENUORGSTRUCTURE_H
#define __MENUORGSTRUCTURE_H

#include <vdr/tools.h>
#include <string>
#include <vector>

// Menu item types
enum eMenuItemType {
  mitSystem,      // VDR system menu
  mitPlugin,      // Plugin entry
  mitCommand,     // External command
  mitMenu,        // Submenu
  mitSeparator    // Separator line
};

// Single menu item
class cMenuorgItem : public cListObject {
private:
  eMenuItemType type;
  std::string name;
  std::string title;
  std::string execute;
  bool confirmExec;
  cList<cMenuorgItem> *subItems;
  
public:
  cMenuorgItem(eMenuItemType Type, const char *Name);
  virtual ~cMenuorgItem();
  
  eMenuItemType Type(void) const { return type; }
  const char *Name(void) const { return name.c_str(); }
  const char *Title(void) const { return title.empty() ? name.c_str() : title.c_str(); }
  const char *Execute(void) const { return execute.c_str(); }
  bool ConfirmExec(void) const { return confirmExec; }
  
  void SetTitle(const char *Title) { title = Title ? Title : ""; }
  void SetExecute(const char *Exec) { execute = Exec ? Exec : ""; }
  void SetConfirmExec(bool Confirm) { confirmExec = Confirm; }
  int Index(void);

  // Submenu management
  bool HasSubItems(void) const { return subItems != NULL; }
  cList<cMenuorgItem> *SubItems(void);
  void AddSubItem(cMenuorgItem *Item);
  void DeleteSubItem(cMenuorgItem *Item);
  
  // Helper methods
  const char *TypeString(void) const;
  cString GetDisplayText(int Indent = 0) const;
};

// Complete menu structure
class cMenuorgStructure {
private:
  cList<cMenuorgItem> rootItems;
  bool modified;
  
public:
  cMenuorgStructure(void);
  virtual ~cMenuorgStructure();
  
  cList<cMenuorgItem> *RootItems(void) { return &rootItems; }
  
  void Clear(void);
  void AddItem(cMenuorgItem *Item);
  void DeleteItem(cMenuorgItem *Item);
  bool MoveItemUp(cMenuorgItem *Item);
  bool MoveItemDown(cMenuorgItem *Item);
  
  bool IsModified(void) const { return modified; }
  void SetModified(bool Modified = true) { modified = Modified; }
  
  // Find item by position in flattened list
  cMenuorgItem *GetItemByIndex(int Index, cMenuorgItem *Parent = NULL);
  int GetItemIndex(cMenuorgItem *Item, cMenuorgItem *Parent = NULL);
  
  // Build flattened list for display
  void BuildFlatList(cList<cMenuorgItem> *FlatList, cMenuorgItem *Parent = NULL);
};

#endif // __MENUORGSTRUCTURE_H
