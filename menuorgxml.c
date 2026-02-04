/*
 * menuorgxml.c: XML parser for menuorg.xml
 */

#include "menuorgxml.h"
#include <vdr/tools.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

// --- cMenuorgXmlParser -----------------------------------------------------

cMenuorgXmlParser::cMenuorgXmlParser(const char *ConfigDir)
{
  configDir = ConfigDir ? ConfigDir : "/etc/vdr/plugins/menuorg";
  xmlFilePath = configDir + "/menu.xml";
}

cMenuorgXmlParser::~cMenuorgXmlParser()
{
}

bool cMenuorgXmlParser::FileExists(void)
{
  struct stat st;
  return stat(xmlFilePath.c_str(), &st) == 0;
}

bool cMenuorgXmlParser::CreateBackup(void)
{
  if (!FileExists())
    return true;
  
  std::string backupPath = xmlFilePath + ".backup";
  char cmd[1024];
  snprintf(cmd, sizeof(cmd), "cp \"%s\" \"%s\"", xmlFilePath.c_str(), backupPath.c_str());
  
  if (system(cmd) != 0) {
    esyslog("menuorgedit: Failed to create backup of menuorg.xml");
    return false;
  }
  
  isyslog("menuorgedit: Created backup: %s", backupPath.c_str());
  return true;
}

const char *cMenuorgXmlParser::GetNodeAttr(xmlNode *node, const char *attrName)
{
  if (!node || !attrName)
    return NULL;
  
  xmlChar *attr = xmlGetProp(node, (const xmlChar *)attrName);
  if (!attr)
    return NULL;
  
  static char buffer[1024];
  strncpy(buffer, (const char *)attr, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';
  xmlFree(attr);
  
  return buffer;
}

bool cMenuorgXmlParser::ParseNode(xmlNode *node, cMenuorgItem *parent, cMenuorgStructure *structure)
{
  for (xmlNode *cur = node; cur; cur = cur->next) {
    if (cur->type != XML_ELEMENT_NODE)
      continue;
    
    const char *nodeName = (const char *)cur->name;
    cMenuorgItem *item = NULL;
    
    if (strcmp(nodeName, "system") == 0) {
      const char *name = GetNodeAttr(cur, "name");
      if (name) {
        item = new cMenuorgItem(mitSystem, name);
      }
    }
    else if (strcmp(nodeName, "plugin") == 0) {
      const char *name = GetNodeAttr(cur, "name");
      if (name) {
        item = new cMenuorgItem(mitPlugin, name);
        const char *title = GetNodeAttr(cur, "title");
        if (title)
          item->SetTitle(title);
      }
    }
    else if (strcmp(nodeName, "command") == 0) {
      const char *name = GetNodeAttr(cur, "name");
      const char *execute = GetNodeAttr(cur, "execute");
      if (name && execute) {
        item = new cMenuorgItem(mitCommand, name);
        item->SetExecute(execute);
        const char *confirm = GetNodeAttr(cur, "confirm");
        if (confirm && strcmp(confirm, "yes") == 0)
          item->SetConfirmExec(true);
      }
    }
    else if (strcmp(nodeName, "menu") == 0) {
      const char *name = GetNodeAttr(cur, "name");
      if (name) {
        item = new cMenuorgItem(mitMenu, name);
        // Parse children
        if (cur->children)
          ParseNode(cur->children, item, structure);
      }
    }
    else if (strcmp(nodeName, "separator") == 0) {
      item = new cMenuorgItem(mitSeparator, "---");
      const char *title = GetNodeAttr(cur, "title");
      if (title)
        item->SetTitle(title);
    }
    
    if (item) {
      if (parent)
        parent->AddSubItem(item);
      else
        structure->AddItem(item);
    }
  }
  
  return true;
}

bool cMenuorgXmlParser::LoadXml(cMenuorgStructure *structure)
{
  if (!structure)
    return false;
  
  if (!FileExists()) {
    isyslog("menuorgedit: menuorg.xml not found at %s", xmlFilePath.c_str());
    return false;
  }
  
  LIBXML_TEST_VERSION
  
  xmlDoc *doc = xmlReadFile(xmlFilePath.c_str(), NULL, 0);
  if (!doc) {
    esyslog("menuorgedit: Failed to parse %s", xmlFilePath.c_str());
    return false;
  }
  
  xmlNode *root = xmlDocGetRootElement(doc);
  if (!root || strcmp((const char *)root->name, "menus") != 0) {
    esyslog("menuorgedit: Invalid root element in menuorg.xml");
    xmlFreeDoc(doc);
    return false;
  }
  
  structure->Clear();
  
  bool result = ParseNode(root->children, NULL, structure);
  
  xmlFreeDoc(doc);
  xmlCleanupParser();
  
  structure->SetModified(false);
  isyslog("menuorgedit: Loaded menuorg.xml from %s", xmlFilePath.c_str());
  
  return result;
}

xmlNode *cMenuorgXmlParser::CreateXmlNode(cMenuorgItem *item, xmlDoc *doc)
{
  if (!item || !doc)
    return NULL;
  
  xmlNode *node = NULL;
  
  switch (item->Type()) {
    case mitSystem:
      node = xmlNewNode(NULL, (const xmlChar *)"system");
      xmlNewProp(node, (const xmlChar *)"name", (const xmlChar *)item->Name());
      break;
      
    case mitPlugin:
      node = xmlNewNode(NULL, (const xmlChar *)"plugin");
      xmlNewProp(node, (const xmlChar *)"name", (const xmlChar *)item->Name());
      if (strcmp(item->Title(), item->Name()) != 0)
        xmlNewProp(node, (const xmlChar *)"title", (const xmlChar *)item->Title());
      break;
      
    case mitCommand:
      node = xmlNewNode(NULL, (const xmlChar *)"command");
      xmlNewProp(node, (const xmlChar *)"name", (const xmlChar *)item->Name());
      xmlNewProp(node, (const xmlChar *)"execute", (const xmlChar *)item->Execute());
      if (item->ConfirmExec())
        xmlNewProp(node, (const xmlChar *)"confirm", (const xmlChar *)"yes");
      break;
      
    case mitMenu:
      node = xmlNewNode(NULL, (const xmlChar *)"menu");
      xmlNewProp(node, (const xmlChar *)"name", (const xmlChar *)item->Name());
      break;
      
    case mitSeparator:
      node = xmlNewNode(NULL, (const xmlChar *)"separator");
      if (strcmp(item->Title(), "---") != 0)
        xmlNewProp(node, (const xmlChar *)"title", (const xmlChar *)item->Title());
      break;
  }
  
  return node;
}

void cMenuorgXmlParser::BuildXmlTree(xmlNode *parent, cMenuorgItem *item, xmlDoc *doc)
{
  if (!parent || !item || !doc)
    return;
  
  if (item->HasSubItems()) {
    cList<cMenuorgItem> *subItems = item->SubItems();
    for (cMenuorgItem *subItem = subItems->First(); subItem; subItem = subItems->Next(subItem)) {
      xmlNode *childNode = CreateXmlNode(subItem, doc);
      if (childNode) {
        xmlAddChild(parent, childNode);
        if (subItem->HasSubItems())
          BuildXmlTree(childNode, subItem, doc);
      }
    }
  }
}

bool cMenuorgXmlParser::SaveXml(cMenuorgStructure *structure)
{
  if (!structure)
    return false;
  
  // Create backup before saving
  if (!CreateBackup())
    return false;
  
  LIBXML_TEST_VERSION
  
  xmlDoc *doc = xmlNewDoc((const xmlChar *)"1.0");
  xmlNode *root = xmlNewNode(NULL, (const xmlChar *)"menus");
  xmlDocSetRootElement(doc, root);
  
  // Build XML tree from structure
  cList<cMenuorgItem> *rootItems = structure->RootItems();
  for (cMenuorgItem *item = rootItems->First(); item; item = rootItems->Next(item)) {
    xmlNode *node = CreateXmlNode(item, doc);
    if (node) {
      xmlAddChild(root, node);
      if (item->HasSubItems())
        BuildXmlTree(node, item, doc);
    }
  }
  
  // Save to file
  int result = xmlSaveFormatFileEnc(xmlFilePath.c_str(), doc, "UTF-8", 1);
  
  xmlFreeDoc(doc);
  xmlCleanupParser();
  
  if (result < 0) {
    esyslog("menuorgedit: Failed to save menuorg.xml to %s", xmlFilePath.c_str());
    return false;
  }
  
  structure->SetModified(false);
  isyslog("menuorgedit: Saved menuorg.xml to %s", xmlFilePath.c_str());
  
  return true;
}
