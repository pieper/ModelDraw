
#include "vtkCallbackCommand.h"

#include "vtkKWProgressDialog.h"

#include "vtkKWLabel.h"
#include "vtkKWProgressGauge.h"
#include "vtkKWApplication.h"

#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWProgressDialog );

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkKWProgressDialog , "$Revision: 12141 $");

//----------------------------------------------------------------------------
// Description:
// the Callback is a static function to reflect progress to the widget
//
void 
vtkKWProgressDialog::Callback(vtkObject *vtkNotUsed(caller), 
                              unsigned long vtkNotUsed(eid),
                              void *clientData, void *callData)
{
  vtkKWProgressDialog *self = reinterpret_cast<vtkKWProgressDialog *>(clientData);

  double progress = * (static_cast<double*> (callData));
  self->UpdateProgress(progress);
}

//----------------------------------------------------------------------------
vtkKWProgressDialog::vtkKWProgressDialog(void)
{
    this->Message=NULL;
    this->Progress=NULL;
    this->MessageText="";
    this->ObservedObject=NULL;
    this->CallbackCommand=vtkCallbackCommand::New();
    this->CallbackCommand->SetCallback( &vtkKWProgressDialog::Callback );
    this->CallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
}

//----------------------------------------------------------------------------
vtkKWProgressDialog::~vtkKWProgressDialog(void)
{
    if(this->Message!=NULL)
    {
        this->Message->SetParent(NULL);
        this->Message->Delete();
        this->Message=NULL;
    }

    if(this->Progress!=NULL)
    {
        this->Progress->SetParent(NULL);
        this->Progress->Delete();
        this->Progress=NULL;
    }

    this->SetObservedObject(NULL);

    if ( this->CallbackCommand != NULL )
      {
      this->CallbackCommand->Delete();
      this->CallbackCommand= NULL;
      }

}

//----------------------------------------------------------------------------
void vtkKWProgressDialog::CreateWidget(void)
{
    // Check if already created

    if (this->IsCreated())
    {
        vtkErrorMacro(<< this->GetClassName() << " already created");
        return;
    }

    // Call the superclass to create the whole widget

    this->Superclass::CreateWidget();

    this->Message=vtkKWLabel::New();
    this->Message->SetParent(this);
    this->Message->Create();
    this->Message->SetText(this->MessageText.c_str());
    this->Script("pack %s",this->Message->GetWidgetName());

    this->Progress=vtkKWProgressGauge::New();
    this->Progress->SetParent(this);
    this->Progress->Create();
    this->Script("pack %s",this->Progress->GetWidgetName());

}

//----------------------------------------------------------------------------
void vtkKWProgressDialog::UpdateProgress(double progress)
{
    this->Progress->SetValue(100*progress);
}

//----------------------------------------------------------------------------
void vtkKWProgressDialog::SetObservedObject(vtkObject *observedObject)
{
  if ( this->ObservedObject == observedObject ) 
    {
    return;
    }

  if ( this->ObservedObject != NULL ) 
    {
    this->ObservedObject->RemoveObservers (vtkCommand::ProgressEvent, this->CallbackCommand);
    this->ObservedObject->Delete();
    }

  this->ObservedObject = observedObject;

  if ( this->ObservedObject != NULL ) 
    {
    this->ObservedObject->Register(this);
    this->ObservedObject->AddObserver (vtkCommand::ProgressEvent, this->CallbackCommand);
    }

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkKWProgressDialog::SetMessageText(const char* messageText)
{
    if(messageText==NULL)
    {
        vtkErrorMacro("messageText is not valid");
        return;
    }
    this->MessageText=messageText;
    if(this->Message!=NULL)
    {
        this->Message->SetText(this->MessageText.c_str());
    }
}

//----------------------------------------------------------------------------
void vtkKWProgressDialog::PrintSelf(std::ostream &os, vtkIndent indent)
{
    Superclass::PrintSelf(os,indent);
    os<<indent<<"vtkKWProgressDialog";
    os<<indent<<"Text: "<<this->MessageText;
    os<<indent<<"Progress: "<<this->Progress->GetValue();

}
void vtkKWProgressDialog::Display(void)
{
    Superclass::Display();
    this->GetApplication()->ProcessPendingEvents();
}
