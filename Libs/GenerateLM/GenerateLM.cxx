/*=========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   GenerateLM

========================================================================*/

/* Generate loadable mode support from an key:value description
   Usage: GenerateLM input_txt_file output_include_file ouput_source_file

   This program generates source code that allows module detection at
   run-time.  The module description is in an xml file.  The output
   files defines the required entry points that will be called during
   startup to initialize the module.

   Typical usage is:
   GenerateLM foo.txt fooLib.h fooLib.cxx

   fooLib.cxx contains:
   #include fooLib.h
*/


#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "expat.h"
#include <string>
#include <vector>
#include <cctype>

#include <itksys/SystemTools.hxx>

// use GenerateCLP to make the header file that implements PARSE_ARGS
#include "GenerateLMCLP.h"

#include "LoadableModuleDescriptionParser.h"
#include "LoadableModuleDescription.h"

/* Generate the preamble to the code. This includes the required
 * include files and code to process comma separated arguments.
 */
void GeneratePre(std::ofstream &, LoadableModuleDescription &, int, char *[]);

/* Generate the code that echos the XML file that describes the
 * command line arguments.
 */
void GenerateSourceIncludes(std::ofstream &, const std::string&, bool, int, char *[]);
void GenerateExports(std::ofstream &);
void GenerateModuleDataSymbols(std::ofstream &, const LoadableModuleDescription&, const std::string&);
void GenerateModuleEntryPoints(std::ofstream &);

int
main(int argc, char *argv[])
{
  PARSE_ARGS;
  LoadableModuleDescription module;
  LoadableModuleDescriptionParser parser;

  std::string input;
  bool xml = false;

  if (!InputTxt.empty())
    {
    input = InputTxt;
    }
  else if (!InputXml.empty())
    {
    xml = true;
    input = InputXml;
    }
  else
    {
      std::cout << argv[0] << ": No input file specified, please use --InputTxt or --InputXml" << std::endl;
      perror(argv[0]);
      return EXIT_FAILURE;
    }

  // Read the input file
  std::ifstream fin(input.c_str(),std::ios::in|std::ios::binary);
  if (fin.fail())
    {
    std::cerr << argv[0] << ": Cannot open " << InputTxt << " for input" << std::endl;
    perror(argv[0]);
    return EXIT_FAILURE;
    }

  // Get the length of the file
  fin.seekg (0, std::ios::end);
  size_t len = fin.tellg();
  fin.seekg (0, std::ios::beg);
  char * TXT = new char[len+1];
  fin.read (TXT, len);
  TXT[len] = '\0';

  // Parse the module description
  std::cerr << "GenerateLM";
  for (int i = 1; i < argc; i++)
    {
    std::cerr << " " << argv[i];
    }
  std::cerr << std::endl;

  if (xml)
    {
    if (parser.ParseXmlDescription(TXT, module))
      {
      std::cerr << "GenerateLM: One or more errors detected. Code generation aborted." << std::endl;
      return EXIT_FAILURE;
      }
    }
  else
    {
    if (parser.ParseText(TXT, module))
      {
      std::cerr << "GenerateLM: One or more errors detected. Code generation aborted." << std::endl;
      return EXIT_FAILURE;
      }
    }

  // fixup information

  std::string nospaces = module.GetName();
  std::string::iterator iter = nospaces.begin();
  while (iter != nospaces.end()) {
    if ((*iter) == ' ') {
      iter = nospaces.erase(iter);
    } else {
      iter++;
    }
  }

  module.SetShortName(nospaces);
  
  std::transform(nospaces.begin(), nospaces.end(), nospaces.begin(), (int(*)(int)) std::tolower);

  if (nospaces.empty()) {
    // do nothing, it's empty
  } else if (nospaces[0] > 96 && nospaces[0] < 122) {
    // it's alphetic
    nospaces[0] = nospaces[0] - 32;
  }

  module.SetTclInitName(nospaces + std::string("_Init"));
  // make outputs

  {
    std::ofstream sout(OutputCxx.c_str(),std::ios::out);
    if (sout.fail())
      {
        std::cerr << argv[0] << ": Cannot open " << OutputCxx << " for output" << std::endl;
        perror(argv[0]);
        return EXIT_FAILURE;
      }
    
    // make source cxx
    std::string classname = itksys::SystemTools::GetFilenameWithoutExtension(input);

    if (module.GetGUIName().empty()) {
      GenerateSourceIncludes(sout, classname, false, argc, argv);
    } else {
      GenerateSourceIncludes(sout, classname, true, argc, argv);
    }
    GenerateModuleDataSymbols(sout, module, classname);
    sout.close();
  }

  {
    std::ofstream sout(OutputHeader.c_str(),std::ios::out);
    if (sout.fail())
      {
        std::cerr << argv[0] << ": Cannot open " << OutputHeader << " for output" << std::endl;
        perror(argv[0]);
        return EXIT_FAILURE;
      }


    // make header


    GeneratePre(sout, module, argc, argv);
    GenerateExports(sout);
    GenerateModuleEntryPoints(sout);
    sout.close();
  }

  return (EXIT_SUCCESS);
}

void GeneratePre(std::ofstream &sout, LoadableModuleDescription & /*module*/,
                 int argc, char *argv[])
{
  sout << "// This file was automatically generated by:" << std::endl;
  sout << "// ";
  for (int i = 0; i < argc; i++)
    {
    sout << " " << argv[i];
    }
  sout << std::endl;
  sout << "//" << std::endl;
  sout << "#include <stdio.h>" << std::endl;
  sout << "#include <stdlib.h>" << std::endl;
  sout << "#include <iostream>" << std::endl;
  sout << "#include <string.h>" << std::endl;
  sout << std::endl;
}

void GenerateSourceIncludes(std::ofstream &sout,
                            const std::string &classname,
                            bool guiname,
                            int argc, 
                            char *argv[])
{
  sout << "// This file was automatically generated by:" << std::endl;
  sout << "// ";
  for (int i = 0; i < argc; i++)
    {
    sout << " " << argv[i];
    }
  sout << std::endl;
  sout << "#include \"" << classname << "Lib.h\"" << std::endl;
  sout << std::endl;
  sout << "#include \"vtk" << classname << "Logic.h\"" << std::endl;
  if (guiname) {
    sout << "#include \"vtk" << classname << "GUI.h\"" << std::endl;
  }
  sout << std::endl;
}

void GenerateExports(std::ofstream &sout)
{
  sout << "#ifdef WIN32" << std::endl;
  sout << "#define Module_EXPORT __declspec(dllexport)" << std::endl;
  sout << "#else" << std::endl;
  sout << "#define Module_EXPORT " << std::endl;
  sout << "#endif" << std::endl;
  sout << std::endl;
}

void GenerateModuleDataSymbols(std::ofstream &sout, const LoadableModuleDescription &module, const std::string &classname)
{
  sout << "extern \"C\" {" << std::endl;
  sout << "Module_EXPORT char " << classname << "ModuleDescription[] = " << std::endl;
  sout << "\"<?xml version=\\\"1.0\\\" encoding=\\\"utf-8\\\"?>\\n\"" << std::endl;
  sout << "\"<loadable>\\n\"" << std::endl;
  sout << "\"  <name>" << module.GetName() << "</name>\\n\"" << std::endl;
  sout << "\"  <shortname>" << module.GetShortName() << "</shortname>\\n\"" << std::endl;
  if (!module.GetGUIName().empty()) {
    sout << "\"  <guiname>" << module.GetGUIName() << "</guiname>\\n\"" << std::endl;
  }
  sout << "\"  <tclinitname>" << module.GetTclInitName() << "</tclinitname>\\n\"" << std::endl;
  sout << "\"  <message>Initializing " << module.GetName() << " Module...</message>\\n\"" << std::endl;

  std::vector<std::string>::const_iterator iter = module.GetDependencies().begin();
  while (iter != module.GetDependencies().end()) {
    sout << "\"  <dependency>" << (*iter) << "</dependency>\\n\"" << std::endl;
    iter++;
  }

  sout << "\"</loadable>\\n\"" << std::endl;
  sout << "\"\\n\"" << std::endl;
  sout << ";" << std::endl;
  sout << "}" << std::endl;
  sout << std::endl;

  sout << "char*" << std::endl;
  sout << "GetLoadableModuleDescription()" << std::endl;
  sout << "{" << std::endl;
  sout << "  return " << classname << "ModuleDescription;" << std::endl;
  sout << "}" << std::endl;
  sout << std::endl;
  sout << "void*" << std::endl;
  sout << "GetLoadableModuleGUI()" << std::endl;
  sout << "{" << std::endl;
  if (module.GetGUIName().empty()) {
    sout << "  return 0;" << std::endl;
  } else {
    sout << "  return vtk" << classname << "GUI::New ( );" << std::endl;
  }
  sout << "}" << std::endl;
  sout << std::endl;
  sout << std::endl;
  sout << "void*" << std::endl;
  sout << "GetLoadableModuleLogic()" << std::endl;
  sout << "{" << std::endl;
  sout << "  return vtk" << classname << "Logic::New ( );" << std::endl;
  sout << "}" << std::endl;

}

void GenerateModuleEntryPoints(std::ofstream &sout)
{
  sout << "extern \"C\" {" << std::endl;
  sout << "  Module_EXPORT char* GetLoadableModuleDescription();" << std::endl;
  sout << "  Module_EXPORT void* GetLoadableModuleGUI();" << std::endl;
  sout << "  Module_EXPORT void* GetLoadableModuleLogic();" << std::endl;
  sout << "}" << std::endl;
  sout << std::endl;
}

