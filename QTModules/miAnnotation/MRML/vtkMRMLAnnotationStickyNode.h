// .NAME vtkMRMLAnnotationStickyNode - MRML node to represent a fiber bundle from tractography in DTI data.
// .SECTION Description
// Annotation nodes contains control points, internally represented as vtkPolyData.
// A Annotation node contains many control points  and forms the smallest logical unit of tractography 
// that MRML will manage/read/write. Each control point has accompanying data.  
// Visualization parameters for these nodes are controlled by the vtkMRMLAnnotationTextDisplayNode class.
//

#ifndef __vtkMRMLAnnotationStickyNode_h
#define __vtkMRMLAnnotationStickyNode_h

#include "qSlicermiAnnotationModuleExport.h"
#include "vtkMRMLAnnotationNode.h" 

class vtkStringArray;
class vtkMRMLStorageNode;

class  Q_SLICER_QTMODULES_ANNOTATIONS_EXPORT vtkMRMLAnnotationStickyNode : public vtkMRMLAnnotationNode
{
public:
  static vtkMRMLAnnotationStickyNode *New();
  vtkTypeMacro(vtkMRMLAnnotationStickyNode, vtkMRMLAnnotationNode);

  //--------------------------------------------------------------------------
  // MRMLNode methods
  //--------------------------------------------------------------------------

  virtual vtkMRMLNode* CreateNodeInstance();
  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "AnnotationSticky";};


  int  SetSticky(const char* text,int selectedFlag);

  // Selected and visible are currently always set to 1 and are controlled by selected and visible flag - we can change this later
  void SetStickyText(const char* newLabel) {this->SetText(0,newLabel,1,1);}
  vtkStdString GetStickyText() {return this->GetText(0);}

  void Initialize(vtkMRMLScene* mrmlScene);


protected:
  vtkMRMLAnnotationStickyNode() { }; 
  ~vtkMRMLAnnotationStickyNode() { };
  vtkMRMLAnnotationStickyNode(const vtkMRMLAnnotationStickyNode&);
  void operator=(const vtkMRMLAnnotationStickyNode&);

};

#endif
