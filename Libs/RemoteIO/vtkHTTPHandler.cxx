#include "vtkHTTPHandler.h"
#include "vtkDirectory.h"

vtkStandardNewMacro ( vtkHTTPHandler );
vtkCxxRevisionMacro ( vtkHTTPHandler, "$Revision: 1.0 $" );

/*------------------------------------------------------------------------------
vtkHTTPHandler* vtkHTTPHandler::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkHTTPHandler");
  if(ret)
    {
    return static_cast<vtkHTTPHandler*>(ret);
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkHTTPHandler;
}
*/

size_t read_callback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t retcode;

  /* in real-world cases, this would probably get this data differently
     as this fread() stuff is exactly what the library already would do
     by default internally */
  retcode = fread(ptr, size, nmemb, stream);

  std::cout << "*** We read " << retcode << " bytes from file\n";

  return retcode;
}

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
  if (stream == NULL)
    {
    std::cerr << "write_callback: can't write, stream is null. size = " << size << std::endl;
    return 0;
    }
  int written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

size_t ProgressCallback(FILE* vtkNotUsed( outputFile ), double dltotal, double dlnow, double ultotal, double ulnow)
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
vtkHTTPHandler::vtkHTTPHandler()
{
  this->CurlHandle = NULL;
  this->ForbidReuse = 0;
}


//----------------------------------------------------------------------------
vtkHTTPHandler::~vtkHTTPHandler()
{
  this->CurlHandle = NULL;
}


//----------------------------------------------------------------------------
void vtkHTTPHandler::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf ( os, indent );
}



//----------------------------------------------------------------------------
int vtkHTTPHandler::CanHandleURI ( const char *uri )
{
  //--- What's the best way to determine whether this handler
  //--- speaks the correct protocol?
  //--- first guess is to look at the prefix up till the colon.

  std::string::size_type index;
  std::string uriString (uri);
  std::string prefix;

  //--- get all characters up to (and not including) the '://'
  if ( ( index = uriString.find ( "://", 0 ) ) != std::string::npos )
    {
    prefix = uriString.substr ( 0, index );
    //--- check to see if any bracketed characters are in
    //--- this part of the string.
    if ( (index = prefix.find ( "]:", 0 ) ) != std::string::npos )
      {
      //--- if so, strip off the leading bracketed characters in case
      //--- we adopt the gwe "[filename.ext]:" prefix.
      prefix = prefix.substr ( index+2 );
      }
    if ( !strcmp(prefix.c_str(), "http"))
      {
      vtkDebugMacro("vtkHTTPHandler: CanHandleURI: can handle this file: " << uriString.c_str());
      return (1);
      }
    }
  else
    {
    vtkDebugMacro ( "vtkHTTPHandler::CanHandleURI: unrecognized uri format: " << uriString.c_str() );
    }
  return ( 0 );
}



//----------------------------------------------------------------------------
void vtkHTTPHandler::InitTransfer( )
{
  curl_global_init(CURL_GLOBAL_ALL);
  vtkDebugMacro("vtkHTTPHandler: InitTransfer: initialising CurlHandle");
  this->CurlHandle = curl_easy_init();
  if (this->CurlHandle == NULL)
    {
    vtkErrorMacro("InitTransfer: unable to initialise");
    }
}

//----------------------------------------------------------------------------
int vtkHTTPHandler::CloseTransfer( )
{
  curl_easy_cleanup(this->CurlHandle);
  curl_global_cleanup();
  this->CurlHandle = NULL;
  return EXIT_SUCCESS;
}


//----------------------------------------------------------------------------
void vtkHTTPHandler::StageFileRead(const char * source, const char * destination)
{
  if (source == NULL || destination == NULL)
    {
    vtkErrorMacro("StageFileRead: source or dest is null!");
    return;
    }

  //---
  //--- make sure destination directory is present
  //---
  std::string destination_dir = vtksys::SystemTools::GetFilenamePath ( destination );
  if (!( vtksys::SystemTools::FileExists (destination_dir.c_str() ) &&
         vtksys::SystemTools::FileIsDirectory (destination_dir.c_str() )))
    {
    //--- destination should be cache dir -- looks like it doesn't exist.
    vtkDebugMacro ( "Creating destination directory for download." );
    vtksys::SystemTools::MakeDirectory (destination_dir.c_str() );
    }

  //---
  //--- try to open temporary bucket for download.
  //--- once download is successful, move it to destination.
  //--- otherwise return error,
  //--- and make sure destination file is zero length.
  //---
  //--- if it's open already, close it up.
  //---
  if ( this->LocalFile )
    {
    if ( fclose(this->LocalFile) != 0 )
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
    this->LocalFile = NULL;
    }

  int useBucket = 1;
  if ( useBucket )
    {
    std::string bucketName = vtksys::SystemTools::GetFilenameName (destination);
    this->CreateFileBucket(bucketName.c_str() );
    this->LocalFile = fopen (this->FileBucket, "wb");
    }
  if ( this->LocalFile == NULL )
    {
    vtkWarningMacro ( "Unable to open temporary download buffer. Writing directly to destination file.");
    useBucket = 0;
    this->LocalFile = fopen(destination, "wb");
    }
  if ( this->LocalFile == NULL )
    {
    vtkErrorMacro ( "Unable to open destination file. No file is downloaded.");
    return;
    }
  
  //---
  //--- init and configure transfer
  //---
  this->InitTransfer( );
  if ( this->CurlHandle == NULL )
    {
    vtkErrorMacro ( "Got NULL curl handle." );
    return;
    }

  if ( this->ForbidReuse )
    {
    curl_easy_setopt(this->CurlHandle, CURLOPT_FORBID_REUSE, 1);
    }
  curl_easy_setopt(this->CurlHandle, CURLOPT_HTTPGET, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_URL, source);
  curl_easy_setopt(this->CurlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEFUNCTION, NULL); // write_callback);
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEDATA, this->LocalFile);
  // quick timeout during connection phase if URL is not accessible (e.g. blocked by a firewall)
  curl_easy_setopt(this->CurlHandle, CURLOPT_CONNECTTIMEOUT, 5); // in seconds (type long)

  vtkDebugMacro("StageFileRead: about to do the curl download... source = " << source << ", dest = " << destination);
  CURLcode retval = curl_easy_perform(this->CurlHandle);
  if (retval == CURLE_BAD_FUNCTION_ARGUMENT)
    {
    vtkErrorMacro("StageFileRead: bad function argument to curl, did you init CurlHandle?");
    }
  else if (retval == CURLE_OUT_OF_MEMORY)
    {
    vtkErrorMacro("StageFileRead: curl ran out of memory!");
    }
  else if ( retval != CURLE_OK )
    {
    const char *stringError = curl_easy_strerror(retval);
    vtkErrorMacro("StageFileRead: error running curl: " << stringError);
    //--- in case the permissions were not correct and that's
    //--- the reason the read command failed,
    //--- reset the 'remember check' in the permissions
    //--- prompter so that new login info  will be prompted.
    if ( this->GetPermissionPrompter() != NULL )
      {
      this->GetPermissionPrompter()->SetRemember ( 0 );
      }
    }

  // close transfer and clean up.
  this->CloseTransfer();

  if ( this->LocalFile )
    {
    if ( fclose(this->LocalFile) != 0 )
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
    this->LocalFile = NULL;
    }

  if (retval == CURLE_OK)
    {
    vtkDebugMacro("StageFileRead: successful return from curl");
    if ( useBucket )
      {
      //--- if the bucket exists and is not zero-length,
      //--- move downloaded data from bucket to actual
      //--- destination and clean up.
      if ( (vtksys::SystemTools::FileExists ( this->GetFileBucket()) ) &&
           (vtksys::SystemTools::FileLength ( this->GetFileBucket()) != 0 ) )
        {
        vtkDirectory::Rename (this->GetFileBucket(), destination );
        }
      }
    }

  if ( useBucket )
    {
    this->DeleteFileBucket();
    }

}


//----------------------------------------------------------------------------
void vtkHTTPHandler::StageFileWrite(const char * source, const char * destination)
{

  // open file.
  if ( this->LocalFile != NULL )
    {
    if ( fclose ( this->LocalFile ) != 0)
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
    this->LocalFile = NULL;
    }
  this->LocalFile = fopen(source, "r");
  if ( this->LocalFile == NULL )
    {
    vtkErrorMacro("StageFileWrite: unable to open file " << source );
    return;
    }

  this->InitTransfer( );
  if ( this->CurlHandle == NULL )
    {
    vtkErrorMacro ( "Got NULL curl handle." );
    return;
    }
  
  curl_easy_setopt(this->CurlHandle, CURLOPT_PUT, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_URL, destination);
  curl_easy_setopt(this->CurlHandle, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_READFUNCTION, read_callback);
  curl_easy_setopt(this->CurlHandle, CURLOPT_READDATA, this->LocalFile);
  curl_easy_setopt(this->CurlHandle, CURLOPT_CONNECTTIMEOUT, 10);  
  CURLcode retval = curl_easy_perform(this->CurlHandle);

   if (retval == CURLE_OK)
    {
    vtkDebugMacro("StageFileWrite: successful return from curl");
    }
   else
    {
    const char *stringError = curl_easy_strerror(retval);
    vtkErrorMacro("StageFileWrite: error running curl: " << stringError);
    //--- in case the permissions were not correct and that's
    //--- the reason the read command failed,
    //--- reset the 'remember check' in the permissions
    //--- prompter so that new login info  will be prompted.
    if ( this->GetPermissionPrompter() != NULL )
      {
      this->GetPermissionPrompter()->SetRemember ( 0 );
      }
    }

  this->CloseTransfer();

  if ( this->LocalFile )
    {
    if ( fclose(this->LocalFile) != 0 )
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
    this->LocalFile = NULL;
    }
}



//--- for calling by the application. Return 1 for connected, 0 for not.
//----------------------------------------------------------------------------
bool vtkHTTPHandler::CheckConnectionAndServer ( const char *uri )
{
  if ( uri == NULL )
    {
    vtkErrorMacro ( "CheckConnectionAndServer: got NULL uri." );
    return 0;
    }
  
  const char *retval = this->CheckServerStatus ( uri );
  if ( ( retval != NULL ) && ( !(strcmp (retval, "OK")) ))
    {
    return 1;
    }
  else
    {
    return 0;
    }
}


//----------------------------------------------------------------------------
const char *vtkHTTPHandler::CheckServerStatus ( const char *uri )
{

  if ( uri == NULL )
    {
    vtkErrorMacro ( "CheckServerStatus: got NULL uri." );
    return ( "A NULL uri was specified." );
    }

  CURLcode retval = CURLE_OK;
  const char *returnString;

  // configure a handle to send a generic ping.
  this->InitTransfer( );
  if ( this->CurlHandle == NULL )
    {
    vtkErrorMacro ( "Got NULL curl handle." );
    return ("Could not initialize check on server status.");
    }

  curl_easy_setopt(this->CurlHandle, CURLOPT_URL, uri);
  curl_easy_setopt(this->CurlHandle, CURLOPT_NOPROGRESS, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_HEADER, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_NOBODY, true);  
  curl_easy_setopt(this->CurlHandle, CURLOPT_CONNECTTIMEOUT, 10 );
  curl_easy_setopt(this->CurlHandle, CURLOPT_TIMEOUT, 10 );
  retval = curl_easy_perform(this->CurlHandle);

/*
  long httpCode;
  long responseCode;
  long connectCode;
  curl_easy_getinfo ( this->CUrlHandle, CURLINFO_HTTP_CONNECTCODE, &connectCode );
  curl_easy_getinfo ( this->CUrlHandle, CURLINFO_HTTP_CODE, &httpCode );
  curl_easy_getinfo ( this->CUrlHandle, CURLINFO_RESPONSE_CODE, &responseCode );
*/
  
  this->CloseTransfer();
  
  if (retval == CURLE_OK)
    {
    returnString = "OK";
    vtkDebugMacro("CheckServerStatus: successful return from curl");
    }
  else
    {
    if (retval == CURLE_COULDNT_CONNECT)
      {
      returnString = "Could not establish connection to server. Please check server status or your network connection.";
      vtkErrorMacro ( "CheckServerStatus: could not connect." );
      }
    else if (retval == CURLE_COULDNT_RESOLVE_HOST)
      {
      returnString = "Could not reach server. Please check to see if webservices are active.";
      vtkErrorMacro ( "CheckServerStatus: could not reach server." );
      }
    else if (retval == CURLE_OUT_OF_MEMORY)
      {
      returnString = "Transfer library (cURL) ran out of memory.";
      vtkErrorMacro ( "CheckServerStatus: cURL ran out of memory." );
      }
    else
      {
      returnString = curl_easy_strerror(retval);
      vtkErrorMacro ( "CheckServerStatus: trouble connecting to web services." );
      }
    }

  return (returnString);

}
