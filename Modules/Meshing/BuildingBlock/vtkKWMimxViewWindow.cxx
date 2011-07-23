/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxViewWindow.cxx,v $
Language:  C++
Date:      $Date: 2008/02/17 04:46:17 $
Version:   $Revision: 1.18 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) The University of Iowa. All rights reserved.
See MIMXCopyright.txt or http://www.ccad.uiowa.edu/mimx/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkKWMimxViewWindow.h"
#include "vtkKWChangeColorButton.h"

#include "vtkMimxUnstructuredGridWidget.h"
#include "vtkMimxErrorCallback.h"

#include "vtkKWApplication.h"
#include "vtkKWMenu.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWWindowBase.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkObjectFactory.h"
#include "vtkPVAxesActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include <vtksys/SystemTools.hxx>

#include "vtkToolkits.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWMimxViewWindow );

vtkCxxRevisionMacro(vtkKWMimxViewWindow, "$Revision: 1.18 $");
//----------------------------------------------------------------------------
class vtkKeypress : public vtkCommand
{
public:
  static vtkKeypress *New() 
  { return new vtkKeypress; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
 /*   vtkRenderer *renderer = reinterpret_cast<vtkRenderer*>(caller);
    cout << renderer->GetActiveCamera()->GetPosition()[0] << " "
      << renderer->GetActiveCamera()->GetPosition()[1] << " "
      << renderer->GetActiveCamera()->GetPosition()[2] << "\n";*/
  }
};

//----------------------------------------------------------------------------
vtkKWMimxViewWindow::vtkKWMimxViewWindow()
{
        this->RenderWidget = NULL;
        this->PVAxesActor = NULL;
        this->AxesRenderer = NULL;
        this->CallbackCommand = NULL;
        this->ChangeColorButton = NULL;
        this->ErrorCallback = vtkMimxErrorCallback::New();
 // this->UnstructuredGridWidget = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxViewWindow::~vtkKWMimxViewWindow()
{
        if (this->AxesRenderer) {
                this->AxesRenderer->Delete();
                this->PVAxesActor->Delete();
                this->CallbackCommand->Delete();
        }
        if(this->RenderWidget)
        {
                this->RenderWidget->Delete();
        }
        if(this->ChangeColorButton)
        {
                this->ChangeColorButton->Delete();
                this->ChangeColorButton = NULL;
        }
        this->ErrorCallback->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewWindow::CreateWidget()
{
        // setting the position and size of the window
        if(this->IsCreated())
        {
                vtkErrorMacro("Class already created");
                return;
        }
        this->Superclass::CreateWidget();
        this->SetSize(950, 700);
        this->SetPosition(310,0);

        // setting the render window
        if(!this->RenderWidget)
        {
                this->RenderWidget = vtkKWRenderWidget::New();
        }
        this->RenderWidget->SetParent(this->GetViewFrame());
        this->RenderWidget->Create();

        this->GetApplication()->Script("pack %s -side top -fill both -expand y -padx 0 -pady 0", 
                this->RenderWidget->GetWidgetName());
        this->GetMenu()->Unpack();

        // creation of axes representation
        // we need two renderers
        if(!this->AxesRenderer)
        {
                this->AxesRenderer = vtkRenderer::New();
                this->PVAxesActor = vtkPVAxesActor::New();
                this->CallbackCommand = vtkCallbackCommand::New();
        }
        this->AxesRenderer->InteractiveOff();
        this->RenderWidget->GetRenderWindow()->SetNumberOfLayers(2);
        this->RenderWidget->GetRenderer()->SetLayer(0);
        this->AxesRenderer->SetLayer(1);
        this->AxesRenderer->SetViewport(0.0,0.0,0.25,0.25);
        this->AxesRenderer->AddActor(this->PVAxesActor);
        this->CallbackCommand->SetCallback(updateAxis);
        this->CallbackCommand->SetClientData(this);
        this->RenderWidget->GetRenderer()->AddObserver(vtkCommand::AnyEvent,this->CallbackCommand);
        this->AxesRenderer->SetBackground(0.0,0.0,0.0);
        this->RenderWidget->GetRenderer()->SetBackground(0.0,0.0,0.0);
        this->RenderWidget->GetRenderWindow()->AddRenderer(this->AxesRenderer);
        
  //vtkKeypress *Keypress = vtkKeypress::New();
  this->RenderWidget->GetRenderWindowInteractor()->RemoveObserver(vtkCommand::KeyPressEvent);
  this->RenderWidget->GetRenderWindowInteractor()->RemoveObserver(vtkCommand::KeyReleaseEvent);
  //this->RenderWidget->GetRenderWindowInteractor()->AddObserver(vtkCommand::KeyPressEvent, Keypress);
  //Keypress->Delete();
  this->ErrorCallback->SetKWApplication(this->GetApplication());
}
//----------------------------------------------------------------------------------------------------
void vtkKWMimxViewWindow::ViewWindowProperties()
{
        if(!this->ChangeColorButton)
        {
                this->ChangeColorButton = vtkKWChangeColorButton::New();
//              this->ChangeColorButton->SetCommand(this, "ChangeBackgroundColor");
                this->ChangeColorButton->SetApplication(this->GetApplication());
                this->ChangeColorButton->Create();
        }
        this->ChangeColorButton->SetColor(this->RenderWidget->GetRenderer()->GetBackground());
        this->ChangeColorButton->ButtonPressCallback();
        this->ChangeColorButton->ButtonReleaseCallback();
        if(this->ChangeColorButton)
        {
                this->RenderWidget->GetRenderer()->SetBackground(this->ChangeColorButton->GetColor());
                this->RenderWidget->Render();
        }

}
//----------------------------------------------------------------------------------------------
//void vtkKWMimxViewWindow::Keypress()
//{
//  return;
//}
void updateAxis(vtkObject* caller, unsigned long , void* arg, void* ) {
        double    cPos[3], cFoc[3], aFoc[3];

        // set the axis camera according to the main renderer.
        vtkKWMimxViewWindow *mimxviewwindow = (vtkKWMimxViewWindow *)arg;
        vtkCamera *axes_cam = mimxviewwindow->RenderWidget->GetRenderer()->IsActiveCameraCreated() ? mimxviewwindow->RenderWidget->GetRenderer()->GetActiveCamera() : NULL;
        cam->GetPosition(cPos);
        cam->GetActiveCamera()->GetFocalPoint(cFoc);
        vtkCamera *axes_cam = mimxviewwindow->AxesRenderer->IsActiveCameraCreated() ? mimxviewwindow->AxesRenderer->GetActiveCamera() : NULL;
        axes_cam->GetFocalPoint(aFoc);
        axes_cam->SetViewUp(cam->GetViewUp());
        axes_cam->SetPosition(cPos[0] - cFoc[0] +
                aFoc[0],\
                cPos[1] - cFoc[1] + aFoc[1],\
                cPos[2] - cFoc[2] + aFoc[2]);
        mimxviewwindow->AxesRenderer->ResetCamera();
}
