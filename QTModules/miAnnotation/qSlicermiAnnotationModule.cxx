#include "qSlicermiAnnotationModule.h"

// SlicerQT includes
#include "qSlicermiAnnotationModuleWidget.h"
#include "vtkSlicermiAnnotationModuleLogic.h"

// QT includes
#include <QtPlugin>

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicermiAnnotationModule, qSlicermiAnnotationModule);

//-----------------------------------------------------------------------------
class qSlicermiAnnotationModulePrivate: public qCTKPrivate<qSlicermiAnnotationModule>
{
  public:
};

//-----------------------------------------------------------------------------
QCTK_CONSTRUCTOR_1_ARG_CXX(qSlicermiAnnotationModule, QObject*);

//-----------------------------------------------------------------------------
qSlicerAbstractModuleWidget * qSlicermiAnnotationModule::createWidgetRepresentation()
{
  return new qSlicermiAnnotationModuleWidget;
}

//-----------------------------------------------------------------------------
vtkSlicerLogic* qSlicermiAnnotationModule::createLogic()
{
  return vtkSlicermiAnnotationModuleLogic::New();
}
