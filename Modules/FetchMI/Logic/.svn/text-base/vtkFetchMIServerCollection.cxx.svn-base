#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkFetchMIServerCollection.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro ( vtkFetchMIServerCollection );
vtkCxxRevisionMacro(vtkFetchMIServerCollection, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
vtkFetchMIServerCollection::vtkFetchMIServerCollection ( )
{
  std::vector <int> parameters;
  std::string webservicesType;
 

  // Developers, extend here as new web service types are added.
  webservicesType.clear();
  parameters.clear();
  // web services type char string
  webservicesType = "XND";
  // whether URI is known or inferrable prior to post (1)
  // or URI is returned by webservice upon request (0)
  parameters.push_back ( 0 );
  this->WebServiceTypesAndParameters.insert ( std::make_pair (webservicesType, parameters));
                                              
  webservicesType.clear();
  parameters.clear();
  webservicesType = "HID";
  parameters.push_back ( 0 );
  this->WebServiceTypesAndParameters.insert ( std::make_pair (webservicesType, parameters));  

  webservicesType.clear();
  parameters.clear();
  parameters.push_back ( 1 );
  webservicesType = "XNE";
  this->WebServiceTypesAndParameters.insert ( std::make_pair (webservicesType, parameters));  
}


// Description:
// Method tells logic whether, based on web services type, the
// uri is inferrable or it must be retrieved via posting metadata and parsing response.
// Method looks at map set up for all web service Types to pull out that info.
// Returns -1 if web service type is not found.
//----------------------------------------------------------------------------
int vtkFetchMIServerCollection::InferrableURI ( const char *webserviceType )
{
  std::map<std::string, std::vector<int> >::iterator iter;
  
  for ( iter = this->WebServiceTypesAndParameters.begin();
        iter != this->WebServiceTypesAndParameters.end();
        iter++ )
    {
    if ( ! iter->first.compare (webserviceType ) )
      {
      return iter->second[0];
      }
    }
  return (-1);
}




//----------------------------------------------------------------------------
void vtkFetchMIServerCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf ( os, indent );
}




//---------------------------------------------------------------------------
vtkFetchMIServerCollection::~vtkFetchMIServerCollection ( )
{

  this->WebServiceTypesAndParameters.clear();
  this->RemoveAllItems();
}




//---------------------------------------------------------------------------
int vtkFetchMIServerCollection::IsKnownServiceType ( const char *webserviceType )
{
  // NOTE TO DEVELOPERS: build this out as new web service types are added.
  // add your new service types here.
  int retval = 0;
  std::map<std::string, std::vector<int> >::iterator iter;
  for ( iter = this->WebServiceTypesAndParameters.begin();
        iter != this->WebServiceTypesAndParameters.end();
        iter++ )
    {
    if ( ! iter->first.compare (webserviceType ) )
      {
      retval = 1;
      break;
      }
    }
  return (retval);
}



//---------------------------------------------------------------------------
void vtkFetchMIServerCollection::AddServerByName ( vtkFetchMIServer *w, const char *name )
{
  if ( name == NULL )
    {
    vtkErrorMacro ( "AddServerByName got null name.");
    return;
    }
  w->SetName ( name );
  this->vtkCollection::AddItem (w);
}





//---------------------------------------------------------------------------
void vtkFetchMIServerCollection::DeleteServerByName ( const char *name )
{
  if ( name == NULL )
    {
    vtkErrorMacro ( "DeleteServerByName got null name.");
    return;
    }

  vtkFetchMIServer *w;
  for ( int i=0; i < this->GetNumberOfItems(); i++ )
    {
    w = vtkFetchMIServer::SafeDownCast (this->GetItemAsObject(i));
    if ( w != NULL )
      {
      if ( !strcmp ( w->GetName(), name ) )
        {
        this->RemoveItem ( i );
        w->Delete();
        break;
        }
      }
    }
}



 
//---------------------------------------------------------------------------
vtkFetchMIServer *vtkFetchMIServerCollection::FindServerByName (const char *name )
{
  if ( name == NULL )
    {
    vtkErrorMacro ( "FindServerByName got null name.");
    return NULL;
    }

  vtkFetchMIServer *w;
  for ( int i=0; i < this->GetNumberOfItems(); i++ )
    {
    w = vtkFetchMIServer::SafeDownCast (this->GetItemAsObject(i));
    if ( w != NULL )
      {
      if ( !strcmp ( w->GetName(), name ) )
        {
        return ( w );
        }
      }
    }
  return ( NULL );
}
