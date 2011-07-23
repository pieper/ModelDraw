/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkOpenIGTLinkIFGUI.h $
  Date:      $Date: 2010-04-01 11:42:15 -0400 (Thu, 01 Apr 2010) $
  Version:   $Revision: 12582 $

==========================================================================*/

#ifndef __vtkIGTLTestWindow_h
#define __vtkIGTLTestWindow_h

// for getting display device information
#ifdef WIN32
#include "Windows.h"
#endif

#include <queue>
#include <vector>

#include "vtkOpenIGTLinkIFWin32Header.h"

#include "vtkSmartPointer.h"
#include "vtkKWTopLevel.h"
#include "vtkSmartPointer.h"

#include "igtlMath.h"

class vtkKWRadioButtonSet;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWPushButton;
class vtkKWEntry;
class vtkKWFrame;

class vtkMultiThreader;
class vtkMutexLock;
class vtkKWText;
class vtkKWTextWithScrollbars;

class VTK_OPENIGTLINKIF_EXPORT vtkIGTLTestWindow : public vtkKWTopLevel
{
public:

  //BTX
  enum {
    MODE_RANDOM = 0,
    MODE_FILE = 1,
  };
  //ETX

  static vtkIGTLTestWindow *New();  
  vtkTypeRevisionMacro(vtkIGTLTestWindow,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetInGUICallbackFlag (int flag) {
    this->InGUICallbackFlag = flag;
    }
  vtkGetMacro(InGUICallbackFlag, int);

  void ProcessTimerEvents();
  void DisplayOnWindow();

protected:

  vtkIGTLTestWindow();
  ~vtkIGTLTestWindow();  

  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );
  
  
  virtual void CreateWidget();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);

  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();
  
  void SwitchMode(int mode);
  void StartServer(int port, float rate);
  void StopServer();

  static void* ServerThreadFunction(void * ptr);
  //BTX
  void GetRandomTestMatrix(igtl::Matrix4x4& matrix);
  //ETX
 
  int LoadCSVTrackingFile(const char * path);
  
 protected:
  
  //----------------------------------------------------------------
  // GUI widgets and Callbacks
  //----------------------------------------------------------------
  vtkKWFrame* MainFrame;
  
  vtkKWRadioButtonSet* ModeButtonSet;
  vtkKWLoadSaveButtonWithLabel* SelectTrackingFileButton;

  vtkKWEntry*          PortEntry;
  vtkKWEntry*          FrameRateEntry;

  vtkKWPushButton* StartTrackingButton;
  vtkKWPushButton* StopTrackingButton;
  vtkKWPushButton* CloseButton;

  vtkKWTextWithScrollbars* StatusText;

  vtkCallbackCommand *GUICallbackCommand;

  int InGUICallbackFlag;
  int IsObserverAddedFlag;
  
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkMultiThreader* Thread;
  vtkMutexLock*     Mutex;
  int               ThreadID;
  int               ServerStopFlag;


  bool   MultipleMonitorsAvailable; 
  int    WindowPosition[2]; // virtual screen position in pixels
  int    WindowSize[2]; // virtual screen size in pixels
  
  int    Mode;
  int    Port;
  double FrameRate;

  //BTX
  std::queue< std::string > StatusMessageBuffer;
  std::vector< std::vector<double> > TrackingData;
  //ETX
  int    TrackingDataIndex;

 private:
  vtkIGTLTestWindow(const vtkIGTLTestWindow&);
  void operator=(const vtkIGTLTestWindow&);


};

#endif
