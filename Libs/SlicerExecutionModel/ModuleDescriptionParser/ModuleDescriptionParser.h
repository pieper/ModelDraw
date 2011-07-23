/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/SlicerExecutionModel/ModuleDescriptionParser/ModuleDescriptionParser.h $
  Date:      $Date: 2008-06-25 11:58:36 -0400 (Wed, 25 Jun 2008) $
  Version:   $Revision: 7176 $

==========================================================================*/
#ifndef __ModuleDescriptionParser_h
#define __ModuleDescriptionParser_h

#include <string>

#include "ModuleDescriptionParserWin32Header.h"

class ModuleDescription;
class ModuleParameterGroup;


class ModuleDescriptionParser_EXPORT ModuleDescriptionParser
{
public:
  ModuleDescriptionParser() {};
  ~ModuleDescriptionParser() {};

  int Parse( const std::string& xml, ModuleDescription& description);
};

#endif
