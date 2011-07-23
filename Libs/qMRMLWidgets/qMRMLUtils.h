#ifndef __qMRMLUtils_h
#define __qMRMLUtils_h

/// QT includes
#include <QString>
#include <QVector>

#include "qMRMLWidgetsExport.h"

class vtkMRMLNode;
class vtkMRMLLinearTransformNode;
class vtkTransform;
class vtkMatrix4x4;
class vtkMRMLScene;

#define compare_double(x, y) (((x-y)<0.000001) && ((x-y)>-0.000001))

class QMRML_WIDGETS_EXPORT qMRMLUtils
{
  
public:
  typedef qMRMLUtils Self; 
  
  /// 
  /// Convert a vtkMatrix to a QVector
  static void vtkMatrixToQVector(vtkMatrix4x4* matrix, QVector<double> & vector); 
  
  /// 
  static void getTransformInCoordinateSystem(vtkMRMLNode* transformNode, bool global, 
    vtkTransform* transform); 
  static void getTransformInCoordinateSystem(vtkMRMLLinearTransformNode* transformNode, 
    bool global, vtkTransform* transform); 
  
  /// 
  /// Used by the qMRMLItemModel;
  static vtkMRMLNode* topLevelNthNode(vtkMRMLScene* scene, int index);
  static vtkMRMLNode* childNode(vtkMRMLNode* node, int childIndex = 0);
  static vtkMRMLNode* parentNode(vtkMRMLNode* node);
  static int          nodeIndex(vtkMRMLNode* node);
  /// count 1 level of children
  static int          childCount(vtkMRMLNode* node);
  /// count 1 level of children
  static int          childCount(vtkMRMLScene* node);
  /// fast function that only check the type of the node to know if it can be a child.
  static bool         canBeAChild(vtkMRMLNode* node);
  /// fast function that only check the type of the node to know if it can be a parent.
  static bool         canBeAParent(vtkMRMLNode* node);
  /// if newParent == 0, set the node into the vtkMRMLScene
  static bool         reparent(vtkMRMLNode* node, vtkMRMLNode* newParent);
  /// fast function that check if the node can be a child of newParent
  static bool         canReparent(vtkMRMLNode* node, vtkMRMLNode* newParent);
  
private:
  /// Not implemented
  qMRMLUtils(){}
  virtual ~qMRMLUtils(){}

};

#endif
