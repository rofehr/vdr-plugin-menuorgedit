/*
 * menuorgxml.h: XML parser for menuorg.xml
 */

#ifndef __MENUORGXML_H
#define __MENUORGXML_H

#include "menuorgstructure.h"
#include <string>
#include <libxml/parser.h>
#include <libxml/tree.h>

class cMenuorgXmlParser {
private:
  std::string configDir;
  std::string xmlFilePath;
  
  bool ParseNode(xmlNode *node, cMenuorgItem *parent, cMenuorgStructure *structure);
  xmlNode *CreateXmlNode(cMenuorgItem *item, xmlDoc *doc);
  void BuildXmlTree(xmlNode *parent, cMenuorgItem *item, xmlDoc *doc);
  
  const char *GetNodeAttr(xmlNode *node, const char *attrName);
  
public:
  cMenuorgXmlParser(const char *ConfigDir);
  virtual ~cMenuorgXmlParser();
  
  bool LoadXml(cMenuorgStructure *structure);
  bool SaveXml(cMenuorgStructure *structure);
  
  const char *GetXmlFilePath(void) const { return xmlFilePath.c_str(); }
  bool FileExists(void);
  bool CreateBackup(void);
};

#endif // __MENUORGXML_H
