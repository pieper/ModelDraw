/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Loadable Module
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/branches/tgl_loadable_modules/Libs/LoadableModuleDescriptionParser/LoadableModuleDescriptionParser.cxx $
  Date:      $Date: 2007-12-19 08:55:14 -0500 (Wed, 19 Dec 2007) $
  Version:   $Revision: 5275 $

==========================================================================*/
#include "LoadableModuleDescriptionParser.h"

#include "LoadableModuleDescription.h"

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cctype>
#include <algorithm>
#include "expat.h"

#define argNotUsed(x) 


/*********************
 * Utility procedures for strings
 *********************/

static void
replaceSubWithSub(std::string& s, const char *o, const char  *n)
{
  if (s.size())
    {
    std::string from(o), to(n);
    std::string::size_type start = 0;
    while ((start = s.find(from, start)) != std::string::npos)
      {
      s.replace(start, from.size(), to);
      start += to.size();
      }
    }
}

static void
trimLeading(std::string& s, const char* extraneousChars = " \t\n")
{
  if (s.size())
    {
    std::string::size_type pos = s.find_first_not_of(extraneousChars);
    if (pos != std::string::npos)
      {
      s = s.substr(pos);
      }
    else
      {
      s = "";
      }
    }
}

static void
trimTrailing(std::string& s, const char* extraneousChars = " \t\n")
{
  if (s.size())
    {
    std::string::size_type pos = s.find_last_not_of(extraneousChars);
    if (pos != std::string::npos)
      {
      s = s.substr(0, pos + 1);
      }
    else
      {
      s = "";
      }
    }
}

static void
trimLeadingAndTrailing(std::string& s, const char* extraneousChars = " \t\n")
{
  trimLeading(s, extraneousChars);
  trimTrailing(s, extraneousChars);
}

class LMParserState
{
public:
  XML_Parser Parser;                           /* The XML parser */
  std::vector<std::string> LastData;           /* The last tag
                                                * processed by expat
                                                * at each depth */
  LoadableModuleDescription CurrentDescription;
  std::stack<std::string> OpenTags;            /* The current tag */
  bool Debug;                                  /* Debug flag */
  bool Error;                                  /* Error detected */
  std::string ErrorDescription;                /* Error description */
  int ErrorLine;                               /* Error line number */
  int Depth;                                   /* The depth of the tag */


  LMParserState() : LastData(10),Debug(false),Error(false),Depth(-1){};
};


/***************************
 * expat callbacks to process the XML
 ***************************/
void
lmStartElement(void *userData, const char *element, const char ** argNotUsed(attrs) )
{
  LMParserState *ps = reinterpret_cast<LMParserState *>(userData);
  std::string name(element);

  ps->Depth++;
  // Clear the last tag for this depth
  ps->LastData[ps->Depth].clear();

  // Check for a valid module description file
  //  
  if (ps->Depth == 0 && (name != "loadable") )
    {
    std::string error("LoadableModuleDescriptionParser Error: <loadable> must be the outer most tag. <" + name + std::string("> was found instead."));
    if (ps->ErrorDescription.size() == 0)
      {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
      }
    ps->OpenTags.push(name);
    return;
    }
  else if (ps->Depth != 0 && (name == "loadable"))
    {
    std::string error("LoadableModuleDescriptionParser Error: <loadable> was found inside another tag <" + ps->OpenTags.top() + ">.");
    if (ps->ErrorDescription.size() == 0)
      {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
      }
    ps->OpenTags.push(name);
    return;
    }

  if (name == "name")
    {
    if (ps->OpenTags.top() != "loadable")
      {
      std::string error("LoadableModuleDescriptionParser Error: <name> can only be inside <loadable> but was found inside <" + ps->OpenTags.top() + ">");
      if (ps->ErrorDescription.size() == 0)
        {
        ps->ErrorDescription = error;
        ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
        ps->Error = true;
        }
      ps->OpenTags.push(name);
      return;
      }
    }
  else if (name == "message")
    {
    if (ps->OpenTags.top() != "loadable")
      {
        std::string error("LoadableModuleDescriptionParser Error: <message> can only be used inside <loadable> but was found inside <" + ps->OpenTags.top() + ">");
        if (ps->ErrorDescription.size() == 0)
          {
            ps->ErrorDescription = error;
            ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
            ps->Error = true;
          }
        ps->OpenTags.push(name);
        return;
      }

    }

  ps->OpenTags.push(name);

}

void
lmEndElement(void *userData, const char *element)
{
  LMParserState *ps = reinterpret_cast<LMParserState *>(userData);
  std::string name(element);

  if (name == "name")
    {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetName(temp);
    }
  else if (name ==  "shortname")
    {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetShortName(temp);
    }
  else if (name ==  "guiname")
    {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetGUIName(temp);
    }
  else if (name ==  "tclinitname")
    {
    std::string temp = ps->LastData[ps->Depth];
    trimLeadingAndTrailing(temp);
    ps->CurrentDescription.SetTclInitName(temp);
    }
  else if (name ==  "message")
    {
    std::string temp = ps->LastData[ps->Depth];
    replaceSubWithSub(temp, "\"", "'");
    replaceSubWithSub(temp, "\n", " ");
    trimLeadingAndTrailing(temp);

    ps->CurrentDescription.SetMessage(temp);
    } 
  else if (name ==  "dependency")
    {
    std::string temp = ps->LastData[ps->Depth];
    replaceSubWithSub(temp, "\"", "'");
    replaceSubWithSub(temp, "\n", " ");
    trimLeadingAndTrailing(temp);

    ps->CurrentDescription.AddDependency(temp);
    }
 else if(name != "loadable")
    {
    std::string error("LoadableModuleDescriptionParser Error: Unrecognized element <" + name + std::string("> was found."));
    if (ps->ErrorDescription.size() == 0)
      {
      ps->ErrorDescription = error;
      ps->ErrorLine = XML_GetCurrentLineNumber(ps->Parser);
      ps->Error = true;
      }
    } 

  if (!ps->OpenTags.empty())
    {
    ps->OpenTags.pop();
    ps->Depth--;
    }
}

void
lmCharData(void *userData, const char *s, int len)
{
  LMParserState *ps = reinterpret_cast<LMParserState *>(userData);
  if (len)
    {
    std::string str(s,len);
    ps->LastData[ps->Depth] += str;
    }
}

int
LoadableModuleDescriptionParser::Parse( const std::string& xml, LoadableModuleDescription& description)
{
// Check the first line if the XML
  if (strncmp(xml.c_str(),"<?xml ", 6) != 0)
    {
    std::string required("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    std::cerr << "LoadableModuleDesriptionParser: first line must be " << std::endl;
    std::cerr << required << std::endl;
    return 1;
    }

  LMParserState parserState;
  parserState.CurrentDescription = description;
  
  XML_Parser parser = XML_ParserCreate(NULL);
  int done;

  parserState.Parser = parser;

  XML_SetUserData(parser, static_cast<void *>(&parserState));
  XML_SetElementHandler(parser, lmStartElement, lmEndElement);
  XML_SetCharacterDataHandler(parser, lmCharData);

  // Parse the XML
  done = true;
  int status = 0;
  if (XML_Parse(parser, xml.c_str(), xml.size(), done) == 0)
    {
    std::cerr << XML_ErrorString(XML_GetErrorCode(parser))
              << " at line "
              << XML_GetCurrentLineNumber(parser)
              << std::endl;
    status = 1;
    }
  else
    {
    if (parserState.Error)
      {
      std::cerr << parserState.ErrorDescription
                << " at line "
                <<  parserState.ErrorLine
                << " while parsing "
                << xml
                << std::endl;
      status = 1;
      }
    }

  XML_ParserFree(parser);

  description = parserState.CurrentDescription;
  return status;

}

int
LoadableModuleDescriptionParser::ParseXmlDescription( const std::string& xmlInput, LoadableModuleDescription& description)
{
  // The parsing algorithm assumes that the string has a trailing newline character
  std::string xml=xmlInput;
  xml.append("\n");

  std::string::size_type pos = 0;
  std::string::size_type lt = xml.find("<", pos);
  std::string::size_type gt = xml.find(">", pos);
  std::string::size_type newline = xml.find("\n", pos);
  
  while (gt != std::string::npos) {

    std::string key(xml.substr(lt + 1, gt - (lt + 1)));

    lt = xml.find("<", gt);

    std::string value(xml.substr(gt + 1, lt - (gt + 1)));

    std::transform(key.begin(), key.end(), key.begin(), (int(*)(int)) std::toupper);

    trimLeadingAndTrailing(value);

    if (key.compare("NAME") == 0) {
      description.SetName(value);
    } else if (key.compare("GUINAME") == 0) {
      description.SetGUIName(value);
    } else if (key.compare("DEPENDENCY") == 0) {
      description.AddDependency(value);
    }

    pos = newline + 1;

    lt = xml.find("<", pos);
    gt = xml.find(">", pos);
    newline = xml.find("\n", pos);

  }

  return 0;
}

int
LoadableModuleDescriptionParser::ParseText( const std::string& txtInput, LoadableModuleDescription& description)
{
  // The parsing algorithm assumes that the string has a trailing newline character
  std::string txt=txtInput;
  txt.append("\n");

  std::string::size_type pos = 0;
  std::string::size_type colon = txt.find(":", pos);
  std::string::size_type newline = txt.find("\n", pos);
  
  while (colon != std::string::npos) {
    std::string key(txt.substr(pos, colon - pos));
    std::string value(txt.substr(colon + 1, newline - (colon + 1)));
    
    std::transform(key.begin(), key.end(), key.begin(), (int(*)(int)) std::toupper);

    trimLeadingAndTrailing(value);

    if (key.compare("NAME") == 0) {
      description.SetName(value);
    } else if (key.compare("GUINAME") == 0) {
      description.SetGUIName(value);
    } else if (key.compare("DEPENDENCY") == 0) {
      description.AddDependency(value);
    }

    pos = newline + 1;

    colon = txt.find(":", pos);
    newline = txt.find("\n", pos);

  }

  if (description.GetGUIName().empty()) {
    description.SetGUIName(description.GetName());
  }

  return 0;
}
