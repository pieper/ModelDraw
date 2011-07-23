/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVolumeNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.13 $

=========================================================================auto=*/
///  vtkMRMLVolumeNode - MRML node for representing a volume (image stack).
/// 
/// Volume nodes describe data sets that can be thought of as stacks of 2D 
/// images that form a 3D volume.  Volume nodes describe where the images 
/// are stored on disk, how to render the data (window and level), and how 
/// to read the files.  This information is extracted from the image 
/// headers (if they exist) at the time the MRML file is generated.  
/// Consequently, MRML files isolate MRML browsers from understanding how 
/// to read the myriad of file formats for medical data. 

#ifndef __vtkMRMLVolumeNode_h
#define __vtkMRMLVolumeNode_h

#include "vtkMatrix4x4.h"
#include "vtkImageData.h"

#include "vtkMRML.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLVolumeDisplayNode.h"
#include "vtkMRMLDisplayableNode.h"

#include "itkMetaDataDictionary.h"

class vtkImageData;
class vtkMRMLStorageNode;

class VTK_MRML_EXPORT vtkMRMLVolumeNode : public vtkMRMLDisplayableNode
{
  public:
  static vtkMRMLVolumeNode *New(){return NULL;};
  vtkTypeMacro(vtkMRMLVolumeNode,vtkMRMLDisplayableNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance() = 0;

  /// 
  /// Read node attributes from XML file
  virtual void ReadXMLAttributes( const char** atts);

  /// 
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// 
  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  /// 
  /// Copy the node's attributes to this object
  void CopyOrientation(vtkMRMLVolumeNode *node);


  /// 
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() = 0;

  /// 
  /// Finds the storage node and read the data
  virtual void UpdateScene(vtkMRMLScene *scene);

  //--------------------------------------------------------------------------
  /// RAS->IJK Matrix Calculation
  //--------------------------------------------------------------------------

  /// 
  /// The order of slices in the volume. One of: LR (left-to-right), 
  /// RL, AP, PA, IS, SI. This information is encoded in the rasToIJKMatrix.
  /// This matrix can be computed either from corner points, or just he
  /// scanOrder.
  static void ComputeIJKToRASFromScanOrder(char *order, 
                                           double* spacing, int *dims,
                                           bool centerImage,
                                           vtkMatrix4x4 *IJKToRAS);

  // So it is tcl wrapped
  void ComputeIJKToRASFromScanOrder(char *order, 
                                    double spacing1, double spacing2, double spacing3, 
                                    int dims1, int dims2, int dims3,
                                    bool centerImage,
                                    vtkMatrix4x4 *IJKToRAS);

  static const char* ComputeScanOrderFromIJKToRAS(vtkMatrix4x4 *IJKToRAS);


  void SetIJKToRASDirections(double dirs[3][3]);
  void SetIJKToRASDirections(double ir, double ia, double is,
                             double jr, double ja, double js,
                             double kr, double ka, double ks);
  void SetIToRASDirection(double ir, double ia, double is);
  void SetJToRASDirection(double jr, double ja, double js);
  void SetKToRASDirection(double kr, double ka, double ks);

  void GetIJKToRASDirections(double dirs[3][3]);
  void GetIToRASDirection(double dirs[3]);
  void GetJToRASDirection(double dirs[3]);
  void GetKToRASDirection(double dirs[3]);

  /// 
  /// Spacing and Origin, with the Directions, are the independent
  /// parameters that go to make up the IJKToRAS matrix
  vtkGetVector3Macro (Spacing, double);
  vtkSetVector3Macro (Spacing, double);
  vtkGetVector3Macro (Origin, double);
  vtkSetVector3Macro (Origin, double);

  /// 
  /// Get the IJKToRAS Matrix that includes the spacing and origin
  /// information (assumes the image data is Origin 0 0 0 and Spacing 1 1 1)
  /// RASToIJK is the inverse of this
  void GetIJKToRASMatrix(vtkMatrix4x4* mat);
  void GetRASToIJKMatrix(vtkMatrix4x4* mat);

  void GetIJKToRASDirectionMatrix(vtkMatrix4x4* mat);
  void SetIJKToRASDirectionMatrix(vtkMatrix4x4* mat);

  /// 
  /// Convenience methods to set the directions, spacing, and origin 
  /// from a matrix
  void SetIJKToRASMatrix(vtkMatrix4x4* mat);
  void SetRASToIJKMatrix(vtkMatrix4x4* mat);

  /// 
  /// Associated display MRML node
  virtual vtkMRMLVolumeDisplayNode* GetVolumeDisplayNode()
  {
    return vtkMRMLVolumeDisplayNode::SafeDownCast(this->GetDisplayNode());
  }

  /// 
  /// Associated ImageData
  vtkGetObjectMacro(ImageData, vtkImageData);
  void SetAndObserveImageData(vtkImageData *ImageData);

  /// 
  /// alternative method to propagate events generated in Display nodes
  virtual void ProcessMRMLEvents ( vtkObject * /*caller*/, 
                                   unsigned long /*event*/, 
                                   void * /*callData*/ );

//BTX
  /// 
  /// DisplayModifiedEvent is generated when display node parameters is changed
  /// PolyDataModifiedEvent is generated when PloyData is changed
  enum
    {
      ImageDataModifiedEvent = 18001
    };
//ETX

//BTX
  /// 
  /// Set/Get the ITK MetaDataDictionary
  void SetMetaDataDictionary( const itk::MetaDataDictionary& );
  const itk::MetaDataDictionary& GetMetaDataDictionary() const;
//ETX

  virtual bool CanApplyNonLinearTransforms() { return false; }
  virtual void ApplyTransform(vtkMatrix4x4* transformMatrix);
  virtual void ApplyTransform(vtkAbstractTransform* transform);

  /// 
  /// Subclasses can define actions to take when the mrml scene passes an event
  /// that the image data has changed, usually call CalculateAutoLevels
  virtual void UpdateFromMRML();
  
  /// 
  /// Create default storage node or NULL if does not have one
  virtual vtkMRMLStorageNode* CreateDefaultStorageNode()
    {
    return Superclass::CreateDefaultStorageNode();
    };


protected:
  vtkMRMLVolumeNode();
  ~vtkMRMLVolumeNode();
  vtkMRMLVolumeNode(const vtkMRMLVolumeNode&);
  void operator=(const vtkMRMLVolumeNode&);

  vtkSetObjectMacro(ImageData, vtkImageData);

  /// these are unit length direction cosines
  double IJKToRASDirections[3][3];

  /// these are mappings to mm space
  double Spacing[3];
  double Origin[3];

  vtkImageData               *ImageData;

//BTX
  itk::MetaDataDictionary Dictionary;
//ETX  
};

#endif


 

