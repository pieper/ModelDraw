/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkUDPServerLogic_h
#define __vtkUDPServerLogic_h

#include "vtkUDPServerWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <fcntl.h>

class vtkMultiThreader;

class VTK_UDPServer_EXPORT vtkUDPServerLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkUDPServerLogic *New();
  
  vtkTypeRevisionMacro(vtkUDPServerLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);
  
  //----------------------------------
  //Data Handlers
  int StartServerConnection();
  void ImportData();
  int Start(int p);
  int Stop();
  bool GetServerStopFlag() {return this->ServerStopFlag;};
  char* GetImportedData(){return this->ImportedData;};
  int GetBytesReceived(){return this->received;};
  void SetPort(int p) {this->port = p;};
  
  int ProbeType; //Probe Type 0 for rejection probe and 1 for single probe

  //-----------------------------------
  //Thread Control
  
  static void* ThreadFunction(void* ptr);

 protected:
  
  vtkUDPServerLogic();
  ~vtkUDPServerLogic();

  void operator=(const vtkUDPServerLogic&);
  vtkUDPServerLogic(const vtkUDPServerLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

  //-----------------------------------------------------------------
  // Server Connection
  //-----------------------------------------------------------------
  
  //BTX
  int sock;
  int port;
  int received;
  static const int BUFFSIZE = 255;
  char buffer[BUFFSIZE];
  struct sockaddr_in echoserver;
  struct sockaddr_in echoclient;
  unsigned int echolen, clientlen, serverlen;
  int ThreadID;
  bool ServerStopFlag;
  char* ImportedData;
  //ETX

 private:

 vtkMultiThreader* Thread;

};

#endif
