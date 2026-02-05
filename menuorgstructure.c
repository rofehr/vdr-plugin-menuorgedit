/*
 * menuorgstructure.c: Data model for menuorg structure
 */

#include "menuorgstructure.h"
#include <vdr/tools.h>
#include <stdio.h>

// --- cMenuorgItem ----------------------------------------------------------

cMenuorgItem::cMenuorgItem(eMenuItemType Type, const char *Name)
{
  type = Type;
  name = Name ? Name : "";
  title = "";
  execute = "";
  confirmExec = false;
  subItems = NULL;
}

cMenuorgItem::~cMenuorgItem()
{
  delete subItems;
}

cList<cMenuorgItem> *cMenuorgItem::SubItems(void)
{
  if (!subItems && type == mitMenu)
    subItems = new cList<cMenuorgItem>();
  return subItems;
}

void cMenuorgItem::AddSubItem(cMenuorgItem *Item)
{
  if (type == mitMenu && Item) {
    SubItems()->Add(Item);
  }
}

void cMenuorgItem::DeleteSubItem(cMenuorgItem *Item)
{
  if (subItems && Item) {
    subItems->Del(Item);
  }
}

const char *cMenuorgItem::TypeString(void) const
{
  switch (type) {
    case mitSystem:    return "System";
    case mitPlugin:    return "Plugin";
    case mitApps:      return "Apps";
    case mitCommand:   return "Command";
    case mitMenu:      return "Menu";
    case mitSeparator: return "Separator";
    default:           return "Unknown";
  }
}

cString cMenuorgItem::GetDisplayText(int Indent) const
{
  char indent[100] = "";
  for (int i = 0; i < Indent && i < 99; i++)
    indent[i] = ' ', indent[i+1] = '\0';
  
  if (type == mitSeparator)
    return cString::sprintf("%s────────────", indent);
  
  const char *typeChar = "";
  switch (type) {
    case mitSystem:  typeChar = "S"; break;
    case mitPlugin:  typeChar = "P"; break;
    case mitApps:    typeChar = "A"; break;
    case mitCommand: typeChar = "C"; break;
    case mitMenu:    typeChar = "M"; break;
    default:         typeChar = "?"; break;
  }
  esyslog("%s[%s] %s", indent, typeChar, Title());
  
  return cString::sprintf("%s[%s] %s", indent, typeChar, Title());
}

// --- cMenuorgStructure -----------------------------------------------------

cMenuorgStructure::cMenuorgStructure(void)
{
  modified = false;
}

cMenuorgStructure::~cMenuorgStructure()
{
  Clear();
}

void cMenuorgStructure::Clear(void)
{
  rootItems.Clear();
  modified = false;
}

void cMenuorgStructure::AddItem(cMenuorgItem *Item)
{
  if (Item) {
    rootItems.Add(Item);
    modified = true;
  }
}

void cMenuorgStructure::DeleteItem(cMenuorgItem *Item)
{
  if (Item) {
    rootItems.Del(Item);
    modified = true;
  }
}

bool cMenuorgStructure::MoveItemUp(cMenuorgItem *Item)
{
  if (!Item)
    return false;
  
  // Find the item and its predecessor
  cMenuorgItem *prev = NULL;
  for (cMenuorgItem *item = rootItems.First(); item; item = rootItems.Next(item)) {
    if (item == Item) {
      if (!prev)
        return false; // Already at top
      
      // Remove item from list
      rootItems.Del(Item);
      // Insert before previous item
      rootItems.Ins(Item, prev);
      modified = true;
      return true;
    }
    prev = item;
  }
  
  return false;
}

bool cMenuorgStructure::MoveItemDown(cMenuorgItem *Item)
{
  if (!Item)
    return false;
  
  // Find the item's successor
  cMenuorgItem *next = rootItems.Next(Item);
  
  if (!next)
    return false; // Already at bottom
  
  // Remove item from list
  rootItems.Del(Item);
  // Insert after next item
  rootItems.Add(Item, next);
  modified = true;
  return true;
}

cMenuorgItem *cMenuorgStructure::GetItemByIndex(int Index, cMenuorgItem *Parent)
{
  cList<cMenuorgItem> *list = Parent ? Parent->SubItems() : &rootItems;
  if (!list)
    return NULL;
  
  int count = 0;
  for (cMenuorgItem *item = list->First(); item; item = list->Next(item)) {
    if (count == Index)
      return item;
    count++;
  }
  return NULL;
}

int cMenuorgStructure::GetItemIndex(cMenuorgItem *Item, cMenuorgItem *Parent)
{
  cList<cMenuorgItem> *list = Parent ? Parent->SubItems() : &rootItems;
  if (!list)
    return -1;
  
  int index = 0;
  for (cMenuorgItem *item = list->First(); item; item = list->Next(item)) {
    if (item == Item)
      return index;
    index++;
  }
  
  return -1;
}

void cMenuorgStructure::BuildFlatList(cList<cMenuorgItem> *FlatList, cMenuorgItem *Parent)
{
  if (!FlatList)
    return;
  
  cList<cMenuorgItem> *list = Parent ? Parent->SubItems() : &rootItems;
  if (!list)
    return;
  
  for (cMenuorgItem *item = list->First(); item; item = list->Next(item)) {
    FlatList->Add(item);
    if (item->HasSubItems())
      BuildFlatList(FlatList, item);
  }
}
