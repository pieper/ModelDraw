
#include "vtkObjectFactory.h"
#include "vtkURIHandler.h"

vtkStandardNewMacro ( vtkURIHandler );
vtkCxxRevisionMacro ( vtkURIHandler, "$Revision: 1.0 $" );
//----------------------------------------------------------------------------
vtkURIHandler::vtkURIHandler()
{
  this->LocalFile = NULL;
  this->RequiresPermission = 0;
  this->PermissionPrompter = NULL;
  this->FileBucket = NULL;
  this->Prefix = NULL;
  this->Name = NULL;
  this->HostName = NULL;
  this->RemoteCacheDirectory = NULL;
}


//----------------------------------------------------------------------------
vtkURIHandler::~vtkURIHandler()
{
  this->LocalFile = NULL;
  if ( this->PermissionPrompter != NULL )
    {
    this->PermissionPrompter->Delete();
    this->PermissionPrompter = NULL;
    }
  if ( this->FileBucket != NULL)
    {
    this->SetFileBucket ( NULL );
    }
  if ( this->Prefix != NULL )
    {
    this->SetPrefix ( NULL );
    }
  if ( this->Name != NULL )
    {
    this->SetName ( NULL );
    }
  if ( this->HostName != NULL )
    {
    this->SetHostName (NULL );
    }
  if ( this->RemoteCacheDirectory != NULL )
    {
    this->SetRemoteCacheDirectory ( NULL );
    }
}


//----------------------------------------------------------------------------
void vtkURIHandler::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf ( os, indent );
}


//----------------------------------------------------------------------------
void vtkURIHandler::SetLocalFile (FILE *localFile )
{
  this->LocalFile = localFile;
}


//----------------------------------------------------------------------------
void vtkURIHandler::CreateFileBucket ()
{

  //--- do some checking to see if directory is set and valid.
  if ( this->GetRemoteCacheDirectory() == NULL )
    {
    vtkWarningMacro ("No path to cache found! Creating files in current dir");
    this->SetFileBucket("SlicerTemporaryDownloadBuffer");
    return;
    }
  if ( !vtksys::SystemTools::FileIsDirectory ( this->GetRemoteCacheDirectory() ))
    {
    vtkWarningMacro ("No valid path to cache found! Creating files in current dir");
    this->SetFileBucket("SlicerTemporaryDownloadBuffer");
    return;
    }
    
  //--- for now, create temporary query response file in cache dir.
  std::vector<std::string> pathComponents;
  vtksys::SystemTools::SplitPath( this->GetRemoteCacheDirectory(), pathComponents);
  // now add the new file name to the end of the path
  pathComponents.push_back("SlicerTemporaryDownloadBuffer");

  //-- create or update temporary staging area for downloads.
  std::string bucket = vtksys::SystemTools::JoinPath(pathComponents);
  this->SetFileBucket (bucket.c_str() );

  vtkDebugMacro ( "FileBucket = " << this->GetFileBucket() );
}



//----------------------------------------------------------------------------
void vtkURIHandler::CreateFileBucket (const char *fileName)
{

  std::string useName;
  if ( !fileName || !(*fileName) )
    {
    useName = "SlicerTemporaryDownloadBuffer";
    }
  else
    {
    useName = fileName;
    useName.append(".part");
    }

  //--- do some checking to see if directory is set and valid.
  if ( this->GetRemoteCacheDirectory() == NULL )
    {
    vtkWarningMacro ("No path to cache found! Creating files in current dir");
    this->SetFileBucket(useName.c_str());
    return;
    }
  if ( !vtksys::SystemTools::FileIsDirectory ( this->GetRemoteCacheDirectory() ))
    {
    vtkWarningMacro ("No valid path to cache found! Creating files in current dir");
    this->SetFileBucket(useName.c_str());
    return;
    }
    
  //--- for now, create temporary query response file in cache dir.
  std::vector<std::string> pathComponents;
  vtksys::SystemTools::SplitPath( this->GetRemoteCacheDirectory(), pathComponents);
  // now add the new file name to the end of the path
  pathComponents.push_back(useName.c_str());

  //-- create or update temporary staging area for downloads.
  std::string bucket = vtksys::SystemTools::JoinPath(pathComponents);
  this->SetFileBucket (bucket.c_str() );

  vtkDebugMacro ( "FileBucket = " << this->GetFileBucket() );
}



//----------------------------------------------------------------------------
void vtkURIHandler::DeleteFileBucket()
{
  if ( this->FileBucket == NULL )
    {
    return;
    }

  // Be tidy. delete the file.
  if ( vtksys::SystemTools::FileExists ( this->GetFileBucket() ) )
    {
    bool clean = vtksys::SystemTools::RemoveFile ( this->GetFileBucket() );
    if (!clean )
      {
      vtkWarningMacro ( "Unable to clean up temporary download file " << this->GetFileBucket() );
      }
    }

  // set the temporary staging area to null
  this->SetFileBucket ( NULL );
}



//----------------------------------------------------------------------------
int vtkURIHandler::ProgressCallback ( FILE * vtkNotUsed(outputFile), double dltotal,
                              double dlnow, double ultotal, double ulnow )
{
    if(ultotal == 0)
    {
    if(dltotal > 0) 
      {
      std::cout << "<filter-progress>" << dlnow/dltotal 
                << "</filter-progress>" << std::endl;
      }
    }
  else
    {
    std::cout << ulnow*100/ultotal << "%" << std::endl;
    }
  return 0;
}


//----------------------------------------------------------------------------
size_t vtkURIHandler::BufferedWrite ( char *buffer, size_t size, size_t nitems )
{
  if ( this->LocalFile != NULL )
    {
    fwrite(buffer, sizeof(char), size*nitems, this->LocalFile);
    size *= nitems;
    return size;
    }
  else
    {
    return ( 0 );
    }
}

//----------------------------------------------------------------------------
void vtkURIHandler::StageFileRead ( const char * vtkNotUsed( source ), const char *  vtkNotUsed( destination ))
{
}

//----------------------------------------------------------------------------
void vtkURIHandler::StageFileWrite ( const char * vtkNotUsed( source ), const char *  vtkNotUsed( destination ))
{
}

//----------------------------------------------------------------------------
void vtkURIHandler::StageFileRead(const char * vtkNotUsed( source ),
                             const char * vtkNotUsed( destination ),
                             const char * vtkNotUsed( username ),
                             const char * vtkNotUsed( password ),
                             const char * vtkNotUsed( hostname ) )
{ 
}

//----------------------------------------------------------------------------
void vtkURIHandler::StageFileWrite(const char * vtkNotUsed( source ),
                              const char * vtkNotUsed( username ),
                              const char * vtkNotUsed( password ),
                              const char * vtkNotUsed( hostname ),
                              const char * vtkNotUsed( sessionID ) )
{ 
}

//----------------------------------------------------------------------------
void vtkURIHandler::InitTransfer ( )
{
}
