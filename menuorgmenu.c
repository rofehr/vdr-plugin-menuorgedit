/*
 * menuorgmenu.c: OSD menu for editing menuorg structure
 */

#include "menuorgmenu.h"
#include <vdr/interface.h>
#include <vdr/plugin.h>

// System menu names available in VDR
static const char *SystemMenuNames[] = {
  "Schedule",
  "Channels",
  "Timers",
  "Recordings",
  "Setup",
  "Commands",
  NULL
};

// --- cMenuorgItemEdit ------------------------------------------------------

cMenuorgItemEdit::cMenuorgItemEdit(cMenuorgItem *Item)
:cOsdMenu(tr("Edit Menu Item"), 20)
{
  item = Item;
  isNew = (Item == NULL);
  
  if (isNew) {
    newType = mitPlugin;
    strcpy(name, "");
    strcpy(title, "");
    strcpy(execute, "");
    confirmExec = 0;
  } else {
    newType = Item->Type();
    strncpy(name, Item->Name(), sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    strncpy(title, Item->Title(), sizeof(title) - 1);
    title[sizeof(title) - 1] = '\0';
    strncpy(execute, Item->Execute(), sizeof(execute) - 1);
    execute[sizeof(execute) - 1] = '\0';
    confirmExec = Item->ConfirmExec() ? 1 : 0;
  }
  
  SetupMenu();
}

cMenuorgItemEdit::~cMenuorgItemEdit()
{
}

void cMenuorgItemEdit::SetupMenu(void)
{
  int current = Current();
  Clear();
  
  static const char *itemTypes[] = {
    "System Menu",
    "Plugin",
    "Command",
    "Submenu",
    "Separator"
  };
  
  Add(new cMenuEditStraItem(tr("Type"), (int *)&newType, 5, itemTypes));
  
  if (newType != mitSeparator) {
    if (newType == mitSystem) {
      // For system menus, provide selection list
      static int systemMenuIndex = 0;
      Add(new cMenuEditStraItem(tr("System Menu"), &systemMenuIndex, 6, SystemMenuNames));
      // Update name based on selection
      if (SystemMenuNames[systemMenuIndex])
        strncpy(name, SystemMenuNames[systemMenuIndex], sizeof(name) - 1);
    } else {
      Add(new cMenuEditStrItem(tr("Name"), name, sizeof(name)));
    }
    
    if (newType != mitSystem)
      Add(new cMenuEditStrItem(tr("Title"), title, sizeof(title)));
    
    if (newType == mitCommand) {
      Add(new cMenuEditStrItem(tr("Execute"), execute, sizeof(execute)));
      Add(new cMenuEditBoolItem(tr("Confirm"), &confirmExec));
    }
  } else {
    Add(new cMenuEditStrItem(tr("Title (optional)"), title, sizeof(title)));
  }
  
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuorgItemEdit::ProcessKey(eKeys Key)
{
  eMenuItemType oldType = newType;
  eOSState state = cOsdMenu::ProcessKey(Key);
  
  if (newType != oldType)
    SetupMenu();
  
  if (state == osUnknown) {
    switch (Key) {
      case kOk:
        // Save changes
        if (strlen(name) > 0 || newType == mitSeparator) {
          if (isNew) {
            item = new cMenuorgItem(newType, name);
          } else {
            // Note: Cannot change type of existing item easily
            // Would need to recreate item
          }
          
          if (item) {
            if (strlen(title) > 0)
              item->SetTitle(title);
            if (newType == mitCommand) {
              item->SetExecute(execute);
              item->SetConfirmExec(confirmExec != 0);
            }
          }
          return osBack;
        }
        Skins.Message(mtError, tr("Name is required"));
        return osContinue;
        
      case kBack:
        return osBack;
        
      default:
        break;
    }
  }
  
  return state;
}

cMenuorgItem *cMenuorgItemEdit::GetItem(void)
{
  return item;
}

// --- cMenuorgEditor --------------------------------------------------------

cMenuorgEditor::cMenuorgEditor(cMenuorgStructure *Structure, cMenuorgXmlParser *XmlParser)
:cOsdMenu(tr("Menu Organization Editor"))
{
  structure = Structure;
  xmlParser = XmlParser;
  currentDepth = 0;
  selectedItem = NULL;
  
  SetHelp(tr("Add"), tr("Edit"), tr("Move"), tr("Delete"));
  
  BuildDisplayList();
  UpdateMenu();
}

cMenuorgEditor::~cMenuorgEditor()
{
}

void cMenuorgEditor::BuildDisplayList(void)
{
  displayList.Clear();
  
  // Build flattened list with proper indentation
  cList<cMenuorgItem> *rootItems = structure->RootItems();
  
  for (cMenuorgItem *item = rootItems->First(); item; item = rootItems->Next(item)) {
    displayList.Add(item);
    
    // Add subitems recursively (simplified - would need depth tracking)
    if (item->HasSubItems()) {
      cList<cMenuorgItem> *subItems = item->SubItems();
      for (cMenuorgItem *sub = subItems->First(); sub; sub = subItems->Next(sub)) {
        displayList.Add(sub);
      }
    }
  }
}

void cMenuorgEditor::UpdateMenu(void)
{
  int current = Current();
  Clear();
  
  if (structure->RootItems()->Count() == 0) {
    Add(new cOsdItem(tr("--- Empty Menu ---")));
  } else {
    int indent = 0;
    for (cMenuorgItem *item = displayList.First(); item; item = displayList.Next(item)) {
      cString text = item->GetDisplayText(indent);
      Add(new cOsdItem(*text));
    }
  }
  
  // Add status info
  if (structure->IsModified()) {
    Add(new cOsdItem("", osUnknown, false));
    Add(new cOsdItem(tr("*** Unsaved changes ***"), osUnknown, false));
  }
  
  SetCurrent(Get(current));
  Display();
}

cMenuorgItem *cMenuorgEditor::GetSelectedItem(void)
{
  int index = Current();
  if (index < 0 || index >= displayList.Count())
    return NULL;
  
  int count = 0;
  for (cMenuorgItem *item = displayList.First(); item; item = displayList.Next(item)) {
    if (count == index)
      return item;
    count++;
  }
  
  return NULL;
}

eOSState cMenuorgEditor::AddNewItem(void)
{
  cMenuorgItemEdit *editMenu = new cMenuorgItemEdit();
  eOSState state = AddSubMenu(editMenu);
  
  if (state == osBack) {
    cMenuorgItem *newItem = editMenu->GetItem();
    if (newItem) {
      structure->AddItem(newItem);
      BuildDisplayList();
      UpdateMenu();
      Skins.Message(mtInfo, tr("Item added"));
    }
  }
  
  return osContinue;
}

eOSState cMenuorgEditor::EditItem(void)
{
  cMenuorgItem *item = GetSelectedItem();
  if (!item) {
    Skins.Message(mtError, tr("No item selected"));
    return osContinue;
  }
  
  cMenuorgItemEdit *editMenu = new cMenuorgItemEdit(item);
  eOSState state = AddSubMenu(editMenu);
  
  if (state == osBack) {
    BuildDisplayList();
    UpdateMenu();
    Skins.Message(mtInfo, tr("Item updated"));
  }
  
  return osContinue;
}

eOSState cMenuorgEditor::DeleteItem(void)
{
  cMenuorgItem *item = GetSelectedItem();
  if (!item) {
    Skins.Message(mtError, tr("No item selected"));
    return osContinue;
  }
  
  if (Interface->Confirm(tr("Delete this item?"))) {
    structure->DeleteItem(item);
    BuildDisplayList();
    UpdateMenu();
    Skins.Message(mtInfo, tr("Item deleted"));
  }
  
  return osContinue;
}

eOSState cMenuorgEditor::MoveItemUp(void)
{
  cMenuorgItem *item = GetSelectedItem();
  if (!item) {
    Skins.Message(mtError, tr("No item selected"));
    return osContinue;
  }
  
  if (structure->MoveItemUp(item)) {
    BuildDisplayList();
    UpdateMenu();
    CursorUp();
  } else {
    Skins.Message(mtError, tr("Cannot move up"));
  }
  
  return osContinue;
}

eOSState cMenuorgEditor::MoveItemDown(void)
{
  cMenuorgItem *item = GetSelectedItem();
  if (!item) {
    Skins.Message(mtError, tr("No item selected"));
    return osContinue;
  }
  
  if (structure->MoveItemDown(item)) {
    BuildDisplayList();
    UpdateMenu();
    CursorDown();
  } else {
    Skins.Message(mtError, tr("Cannot move down"));
  }
  
  return osContinue;
}

eOSState cMenuorgEditor::Save(void)
{
  if (!structure->IsModified()) {
    Skins.Message(mtInfo, tr("No changes to save"));
    return osContinue;
  }
  
  if (xmlParser->SaveXml(structure)) {
    Skins.Message(mtInfo, tr("Menu configuration saved"));
    UpdateMenu();
  } else {
    Skins.Message(mtError, tr("Failed to save configuration"));
  }
  
  return osContinue;
}

eOSState cMenuorgEditor::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  
  if (state == osUnknown) {
    switch (Key) {
      case kRed:
        return AddNewItem();
      case kGreen:
        return EditItem();
      case kYellow:
        return MoveItemUp();
      case kBlue:
        return DeleteItem();
      case k0:
        return MoveItemDown();
      case kOk:
        return Save();
      case kBack:
        if (structure->IsModified()) {
          if (Interface->Confirm(tr("Discard unsaved changes?")))
            return osBack;
          return osContinue;
        }
        return osBack;
      default:
        break;
    }
  }
  
  return state;
}
