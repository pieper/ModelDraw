/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Loadable Module
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/LoadableModule/LoadableModuleDescriptionParser.h $
  Date:      $Date: 2010-01-29 12:01:42 -0500 (Fri, 29 Jan 2010) $
  Version:   $Revision: 11875 $

==========================================================================*/
#ifndef __LoadableModuleDescriptionParser_h
#define __LoadableModuleDescriptionParser_h

#include <string>

#include "LoadableModuleWin32Header.h"

class LoadableModuleDescription;

class LoadableModule_EXPORT LoadableModuleDescriptionParser
{
public:
  LoadableModuleDescriptionParser() {};
  ~LoadableModuleDescriptionParser() {};

  int Parse( const std::string& xml, LoadableModuleDescription& description);

  /// Use this to parse the module's XML-like description file
  int ParseXmlDescription(const std::string& xmlInput, LoadableModuleDescription& description);

  int ParseText( const std::string& txtInput, LoadableModuleDescription& description);
};

#endif
