#ifndef __qMRMLVolumeThresholdWidget_h
#define __qMRMLVolumeThresholdWidget_h


/// qVTK includes
#include <qVTKObject.h>

/// qCTK includes
#include <qCTKPimpl.h>

/// QT includes
#include <QWidget>

#include "qMRMLWidgetsExport.h"

class vtkMRMLNode;
class vtkMRMLScalarVolumeDisplayNode;
class vtkMRMLScalarVolumeNode;
class qMRMLVolumeThresholdWidgetPrivate;

class QMRML_WIDGETS_EXPORT qMRMLVolumeThresholdWidget : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT

  Q_PROPERTY(int  autoThreshold READ autoThreshold WRITE setAutoThreshold)
  Q_PROPERTY(double lowerThreshold READ lowerThreshold WRITE setLowerThreshold)
  Q_PROPERTY(double upperThreshold READ upperThreshold WRITE setUpperThreshold)

public:
  /// Constructors
  typedef QWidget Superclass;
  explicit qMRMLVolumeThresholdWidget(QWidget* parent);
  virtual ~qMRMLVolumeThresholdWidget(){}

  int autoThreshold() const;

  /// 
  /// Get lowerThreshold
  double lowerThreshold()const;

  /// 
  /// Get upperThreshold
  double upperThreshold()const;

  /// 
  /// Return the current MRML node of interest
  vtkMRMLScalarVolumeNode* mrmlVolumeNode()const
    { return this->VolumeNode; };

signals:
  /// 
  /// Signal sent if the lowerThreshold/upperThreshold value is updated
  void thresholdValuesChanged(double lowerThreshold, double upperThreshold);
  /// 
  /// Signal sent if the auto/manual value is updated
  void autoThresholdValueChanged(int value);

public slots:
  ///
  /// Set Auto/Manual mode
  void setAutoThreshold(int autoThreshold);

  /// 
  /// Set lowerThreshold
  void setLowerThreshold(double lowerThreshold);

  /// 
  /// Set upperThreshold
  void setUpperThreshold(double upperThreshold);

  /// 
  /// Set lowerThreshold/upperThreshold in once
  void setThreshold(double lowerThreshold, double upperThreshold);

  // TODO remove when range becomes double
  void setMinMaxRange(int min, int max);

  /// 
  /// Set the MRML node of interest
  void setMRMLVolumeNode(vtkMRMLScalarVolumeNode* displayNode);
  void setMRMLVolumeNode(vtkMRMLNode* node);


protected slots:

  /// update widget GUI from MRML node
  void updateWidgetFromMRML();

protected:
  /// 
  /// Return the current MRML display node
  vtkMRMLScalarVolumeDisplayNode* mrmlDisplayNode()const 
    { return this->VolumeDisplayNode;};

  /// 
  /// Set current MRML display node
  void setMRMLVolumeDisplayNode(vtkMRMLScalarVolumeDisplayNode* displayNode);

  /// 
  /// Set sliders range
  void setMinimum(double min);
  void setMaximum(double max);


private:
  QCTK_DECLARE_PRIVATE(qMRMLVolumeThresholdWidget);

  vtkMRMLScalarVolumeNode* VolumeNode;
  vtkMRMLScalarVolumeDisplayNode* VolumeDisplayNode;
};

#endif
