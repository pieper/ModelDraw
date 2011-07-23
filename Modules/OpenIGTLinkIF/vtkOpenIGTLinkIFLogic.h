/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkOpenIGTLinkIFLogic.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

// .NAME vtkOpenIGTLinkIFLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkOpenIGTLinkIFLogic_h
#define __vtkOpenIGTLinkIFLogic_h

#include <vector>

#include "vtkOpenIGTLinkIFWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLTransformNode.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMultiThreader.h"

#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"

#include "vtkIGTLToMRMLBase.h"
#include "vtkIGTLToMRMLLinearTransform.h"
#include "vtkIGTLToMRMLImage.h"
#include "vtkIGTLToMRMLPosition.h"

#ifdef OpenIGTLinkIF_USE_VERSION_2
  #include "vtkIGTLToMRMLImageMetaList.h"
  #include "vtkIGTLToMRMLTrackingData.h"
#endif //OpenIGTLinkIF_USE_VERSION_2

class vtkMRMLIGTLConnectorNode;


class VTK_OPENIGTLINKIF_EXPORT vtkOpenIGTLinkIFLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {
    SLICE_DRIVER_USER    = 0,
    SLICE_DRIVER_LOCATOR = 1,
    SLICE_DRIVER_RTIMAGE = 2
  };

  enum ImageOrient{
    SLICE_RTIMAGE_NONE      = 0,
    SLICE_RTIMAGE_PERP      = 1,
    SLICE_RTIMAGE_INPLANE90 = 2,
    SLICE_RTIMAGE_INPLANE   = 3
  };

  enum {  // Events
    StatusUpdateEvent       = 50001,
    //SliceUpdateEvent        = 50002,
  };

  typedef struct {
    std::string name;
    std::string type;
    int io;
    std::string nodeID;
  } IGTLMrmlNodeInfoType;

  typedef std::vector<IGTLMrmlNodeInfoType>         IGTLMrmlNodeListType;
  typedef std::vector<vtkIGTLToMRMLBase*>           MessageConverterListType;
  //ETX
  
  // Work phase keywords used in NaviTrack (defined in BRPTPRInterface.h)

 public:
  
  static vtkOpenIGTLinkIFLogic *New();
  
  vtkTypeRevisionMacro(vtkOpenIGTLinkIFLogic,vtkObject);

  vtkSetMacro ( EnableOblique,           bool );
  vtkGetMacro ( EnableOblique,           bool );
  vtkSetMacro ( FreezePlane,             bool );
  vtkGetMacro ( FreezePlane,              bool );

  vtkGetObjectMacro ( LocatorTransform, vtkTransform );
  vtkGetObjectMacro ( LocatorMatrix,    vtkMatrix4x4 );

  /// The selected transform node is observed for TransformModified events and the transform 
  /// data is copied to the slice nodes depending on the current mode
  vtkGetObjectMacro ( LocatorTransformNode,    vtkMRMLTransformNode );

  void PrintSelf(ostream&, vtkIndent);

  //----------------------------------------------------------------
  // Start up the class
  //----------------------------------------------------------------

  int Initialize();

  //----------------------------------------------------------------
  // Connector and converter Management
  //----------------------------------------------------------------

  // Access connectors
  vtkMRMLIGTLConnectorNode* GetConnector(const char* conID);
  void                      ImportFromCircularBuffers();
  void                      ImportEvents(); // check if there are any events in the connectors that should be invoked in the main thread (such as connected, disconnected)
  
  // Device Name management
  int  SetRestrictDeviceName(int f);

  int  RegisterMessageConverter(vtkIGTLToMRMLBase* converter);
  int  UnregisterMessageConverter(vtkIGTLToMRMLBase* converter);

  unsigned int       GetNumberOfConverters();
  vtkIGTLToMRMLBase* GetConverter(unsigned int i);
  vtkIGTLToMRMLBase* GetConverterByDeviceType(const char* deviceType);

  //----------------------------------------------------------------
  // MRML Management
  //----------------------------------------------------------------

  void ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData);

  int  SetLocatorDriver(const char* nodeID);
  int  EnableLocatorDriver(int i);
  int  SetRealTimeImageSource(const char* nodeID);
  int  SetSliceDriver(int index, int v);
  int  GetSliceDriver(int index);
  void UpdateSliceNode(int sliceNodeNumber, vtkMatrix4x4* transform);
  void UpdateSliceNodeByImage(int sliceNodeNuber);
  void CheckSliceNode();

  vtkMRMLModelNode* SetVisibilityOfLocatorModel(const char* nodeName, int v);
  vtkMRMLModelNode* AddLocatorModel(const char* nodeName, double r, double g, double b);

  void ProcCommand(const char* nodeName, int size, unsigned char* data);

  //BTX
  void GetDeviceNamesFromMrml(IGTLMrmlNodeListType &list);
  void GetDeviceNamesFromMrml(IGTLMrmlNodeListType &list, const char* mrmlTagName);
  //void GetDeviceTypes(std::vector<char*> &list);
  //ETX

 protected:
  
  //----------------------------------------------------------------
  // Constructor, destructor etc.
  //----------------------------------------------------------------

  vtkOpenIGTLinkIFLogic();
  ~vtkOpenIGTLinkIFLogic();
  vtkOpenIGTLinkIFLogic(const vtkOpenIGTLinkIFLogic&);
  void operator=(const vtkOpenIGTLinkIFLogic&);

  
  static void DataCallback(vtkObject*, unsigned long, void *, void *);

  void UpdateAll();
  void UpdateSliceDisplay();
  void UpdateLocator();

  vtkCallbackCommand *DataCallbackCommand;

 private:

  int Initialized;

  //----------------------------------------------------------------
  // Connector Management
  //----------------------------------------------------------------

  //BTX
  //ConnectorMapType              ConnectorMap;
  MessageConverterListType      MessageConverterList;
  //ETX

  //int LastConnectorID;
  int RestrictDeviceName;

  //----------------------------------------------------------------
  // IGTL-MRML converters
  //----------------------------------------------------------------
  vtkIGTLToMRMLLinearTransform* LinearTransformConverter;
  vtkIGTLToMRMLImage*           ImageConverter;
  vtkIGTLToMRMLPosition*        PositionConverter;
#ifdef OpenIGTLinkIF_USE_VERSION_2
  vtkIGTLToMRMLImageMetaList*   ImageMetaListConverter;
  vtkIGTLToMRMLTrackingData*    TrackingDataConverter;
#endif //OpenIGTLinkIF_USE_VERSION_2

  //----------------------------------------------------------------
  // Real-time image
  //----------------------------------------------------------------
  
  vtkMRMLSliceNode *SliceNode[3];

  int   SliceDriver[3];
  int   SliceDriverConnectorID[3]; // will be obsolete
  int   SliceDriverDeviceID[3];    // will be obsolete

  //BTX
  std::string   LocatorDriverNodeID;
  std::string   RealTimeImageSourceNodeID;
  //ETX

  int   LocatorDriverFlag;
  bool  EnableOblique;
  bool  FreezePlane;
  int   SliceOrientation[3];
  
  //----------------------------------------------------------------
  // Locator
  //----------------------------------------------------------------

  // What's a difference between LocatorMatrix and Locator Transform???
  vtkMatrix4x4*         LocatorMatrix;
  vtkTransform*         LocatorTransform;
  vtkMRMLTransformNode* LocatorTransformNode;

};

#endif


  
