#include "itksys/Process.h"
#include "vtkDirectory.h"
#include "vtkXNDHandler.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>
#include <vector>

vtkStandardNewMacro ( vtkXNDHandler );
vtkCxxRevisionMacro ( vtkXNDHandler, "$Revision: 1.0 $" );


size_t xnd_read_callback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t retcode;

  /* in real-world cases, this would probably get this data differently
     as this fread() stuff is exactly what the library already would do
     by default internally */
  retcode = fread(ptr, size, nmemb, stream);

  std::cout << "*** vtkXNDHandler: We read " << retcode << " bytes from file\n";

  return retcode;
}

size_t xnd_write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
  if (stream == NULL)
    {
    std::cerr << "xnd_write_callback: can't write, stream is null. size = " << size << std::endl;
    return 0;
    }
  int written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

// write header
size_t xnd_writeheader_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
  if (stream == NULL)
    {
    std::cerr << "xnd_writeheader_callback: can't write, stream is null. size = " << size << std::endl;
    return 0;
    }
  int written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

size_t xnd_ProgressCallback(FILE* vtkNotUsed(outputFile), double dltotal,
                            double dlnow, double ultotal, double ulnow)
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
vtkXNDHandler::vtkXNDHandler()
{
}


//----------------------------------------------------------------------------
vtkXNDHandler::~vtkXNDHandler()
{
}


//----------------------------------------------------------------------------
void vtkXNDHandler::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf ( os, indent );
}



//----------------------------------------------------------------------------

//  curl commands for doing various xnat desktop things:
//  #-check for tags
//  curl http://localhost:8081/tags
//
//  #add a tag
//  curl -X POST 'http://localhost:8081/tags?NewTag1&NewTag2'
//
//  #delete a tag
//  curl -X DELETE 'http://localhost:8081/tags?NewTag1'
//
//  #create a tagged data entry
//  curl -X POST -H "Content-Type: application/x-xnat-metadata+xml" -H "Content-Disposition: x-xnat-metadata; filename=\"demo-howto\"" -d @demo/sample-metadata/cardiac-text.xml http://localhost:8000/data
//  # returns url, which is an empty file
//  # http://localhost:8000/data/demo-sample-project/subj02/CT/demo-howto
//
//  #add some data
//  curl -T XNAT-FileServer-HOWTO.txt http://localhost:8000/data/demo-sample-project/subj02/CT/demo-howto
//
//  # can get metadata with:
//  curl http://localhost:8000/data/demo-sample-project/subj02/CT/demo-howto?part=metadata
//
//  # can search for this in the database:
//  curl http://localhost:8000/search
//
//  # can search for modalities:
//  curl http://localhost:8000/search??modality
//
//  # can search for all resources by modality:
//  curl http://localhost:8000/search?modality=CT
//
//  # can get just the resource 
//  curl 'http://localhost:8000/search?modality=CT&no-metadata'
//
//  # now you can use the URIs found using search commands 
//  # to download the actual data
//  # http://localhost:8000/data/demo-sample-project/subj02/CT/demo-howto
//
//----------------------------------------------------------------------------


//--- for downloading
//----------------------------------------------------------------------------
void vtkXNDHandler::StageFileRead(const char * source,
                                  const char *destination )
{
  if (source == NULL || destination == NULL)
    {
    vtkErrorMacro("StageFileRead: source or dest is null!");
    return;
    }

  const char *hostname = this->GetHostName();
  if ( hostname == NULL )
    {
    vtkErrorMacro("StageFileRead: null host name");
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
  //--- write into temporary buffer before copying to destination file.
  //--- once download is successful, move it to destination.
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
    vtkErrorMacro ( "Unable to open destination file for downloading. No download performed." );
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
  curl_easy_setopt(this->CurlHandle, CURLOPT_HTTPGET, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_URL, source);
  curl_easy_setopt(this->CurlHandle, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEFUNCTION, NULL); // write_callback);
  curl_easy_setopt(this->CurlHandle, CURLOPT_CONNECTTIMEOUT, 10);
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEDATA, this->LocalFile);    

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
  else if ( retval != CURLE_OK)
    {
    const char *stringError = curl_easy_strerror(retval);
    vtkErrorMacro("StageFileRead: error running curl: " << stringError);
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
    vtkDebugMacro("StageFileRead: successful return from curl.");
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

  if ( useBucket)
    {    
    this->DeleteFileBucket();
    }
 }


//--- for uploading
//----------------------------------------------------------------------------
void vtkXNDHandler::StageFileWrite(const char *source,
                                   const char *destination )

{
  if (source == NULL || destination == NULL)
    {
    vtkErrorMacro("StageFileWrite: source or dest is null!");
    return;
    }

  const char *hostname = this->GetHostName();
  if ( hostname == NULL )
    {
    vtkErrorMacro("StageFileWrite: null host name");
    return;    
    }

  // Open file
  if ( this->LocalFile != NULL )
    {
    if ( fclose ( this->LocalFile ) != 0)
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
    this->LocalFile = NULL;
    }
  this->LocalFile = fopen(source, "rb");
  if ( this->LocalFile == NULL )
    {
    vtkErrorMacro("StageFileWrite: unable to open file " << source );
    return;
    }
  // read all the stuff in the file into a buffer to find size.
  // seems to work correctly as long as file is binary.
  fseek(this->LocalFile, 0, SEEK_END);
  long lSize = ftell(this->LocalFile);
  rewind(this->LocalFile);

  unsigned char *post_data = NULL;
  this->InitTransfer( );
  if ( this->CurlHandle == NULL )
    {
    vtkErrorMacro ( "Got NULL curl handle." );
    return;
    }

  curl_easy_setopt(this->CurlHandle, CURLOPT_PUT, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_URL, destination);
  curl_easy_setopt(this->CurlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_READFUNCTION, xnd_read_callback);
  curl_easy_setopt(this->CurlHandle, CURLOPT_READDATA, this->LocalFile);
  curl_easy_setopt(this->CurlHandle, CURLOPT_CONNECTTIMEOUT, 10);  
  curl_easy_setopt(this->CurlHandle, CURLOPT_INFILESIZE, lSize);
  CURLcode retval = curl_easy_perform(this->CurlHandle);
  
   if (retval == CURLE_OK)
    {
    vtkDebugMacro("StageFileWrite: successful return from curl");
    }
   else
    {
    const char *stringError = curl_easy_strerror(retval);
    vtkErrorMacro("StageFileWrite: error running curl: " << stringError);
    vtkErrorMacro("\t\tsource = " << source << ", destination = " << destination);
    }
   
   this->CloseTransfer();

  if (post_data)
    {
    free(post_data);
    }

  if ( this->LocalFile )
    {
    if ( fclose ( this->LocalFile ) != 0)
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
    this->LocalFile = NULL;
    }
}



//----------------------------------------------------------------------------
int vtkXNDHandler::PostTag ( const char *svr, const char *label,
                             const char *temporaryResponseFileName)
{

  const char *hostname = this->GetHostName();
  if ( hostname == NULL )
    {
    vtkErrorMacro("PostTag: null host name");
    return 0;
    }
  if ( svr == NULL )
    {
    vtkErrorMacro ("PostTag: got a null server.");
    return 0;
    }
  if ( label == NULL )
    {
    vtkErrorMacro ("PostTag: got a null tag to add.");
    return 0;
    }
  if (temporaryResponseFileName == NULL)
    {
    vtkErrorMacro("PostTag: temporaryResponseFileName is null.");
    return 0;
    }

  std::stringstream ssuri;
  ssuri << svr;
  ssuri << "/tags?";
  std::string tmp = ssuri.str();
  const char *uri = tmp.c_str();  

  //--- create tag postfields
  std::stringstream ss;
  ss << label;
  std::string tmp2 = ss.str();
  const char *pf = tmp2.c_str();

  this->InitTransfer();
  if ( this->CurlHandle == NULL )
    {
    vtkErrorMacro ( "Got NULL curl handle." );
    return 0;
    }

  // then need to set up a local file for capturing the return uri from the post
  const char *responseFileName = temporaryResponseFileName;
  FILE *responseFile = fopen(responseFileName, "wb");
  if (responseFile == NULL)
    {
    this->CloseTransfer();
    vtkErrorMacro("PostTag: unable to open a local file called " << responseFileName << " to write out to for capturing the uri");
    return 0;
    }
  
  //-- configure the curl handle
  curl_easy_setopt(this->CurlHandle, CURLOPT_POST, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_VERBOSE, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_URL, uri);
  curl_easy_setopt(this->CurlHandle, CURLOPT_POSTFIELDS, pf );  
  curl_easy_setopt(this->CurlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEFUNCTION, NULL); // write_callback);
  curl_easy_setopt(this->CurlHandle, CURLOPT_CONNECTTIMEOUT, 10);    
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEDATA, responseFile);
  
  CURLcode retval = curl_easy_perform(this->CurlHandle);

  if (retval == CURLE_OK)
    {
    vtkDebugMacro("PostTag: successful return from curl");
    }
  else
    {
    const char *stringError = curl_easy_strerror(retval);
    vtkErrorMacro("PostTag: error running curl: " << stringError);
    return 0;
    }

  this->CloseTransfer();

  if (responseFile)
    {
    if ( fclose ( responseFile ) != 0)
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkXNDHandler::PostMetadata( const char *serverPath,
                                      const char *headerFileName,
                                      const char *dataFileName,
                                      const char *metaDataFileName,
                                      const char *temporaryResponseFileName )
{
  
  // serverPath will contain $srv/data, for instance http://localhost:8081/data
  // metaDataFileName is a filename of a file that has metadata in it.
  // temporaryResponseFileName is the name of the file into which
  if ( serverPath == NULL )
    {
    vtkErrorMacro("PostMetadata: serverPath is null.");
    return 0;
    }
  if (metaDataFileName == NULL)
    {
    vtkErrorMacro("PostMetadata: metaDataFileName is null.");
    return 0;
    }
  if (temporaryResponseFileName == NULL)
    {
    vtkErrorMacro("PostMetadata: temporaryResponseFileName is null.");
    return 0;
    }
  if (headerFileName == NULL )
    {
    vtkErrorMacro ("PostMetadataTest: null header filename");
    return 0;
    }
 const char *hostname = this->GetHostName();
  if ( hostname == NULL )
    {
    vtkErrorMacro("PostMetadata: null host name");
    return 0;
    }

  //--- tried this foen with both 'r' and 'rb'
  this->LocalFile = fopen(metaDataFileName, "r");
  if ( this->LocalFile == NULL )
    {
    vtkErrorMacro("PostMetadata: unable to open meta data file " << metaDataFileName);
    return 0;
    }
  // then need to set up a local file for capturing the return uri from the
  // post
  const char *returnURIFileName = temporaryResponseFileName;
  FILE *returnURIFile = fopen(returnURIFileName, "wb");
  if (returnURIFile == NULL)
    {
    vtkErrorMacro("PostMetadata: unable to open a local file caled " << returnURIFileName << " to write out to for capturing the uri");
    return 0;
    }

  this->InitTransfer();
  if ( this->CurlHandle == NULL )
    {
    vtkErrorMacro ( "Got NULL curl handle." );
    return 0;
    }


  //-- add header
  std::string dataFileNameString = std::string(dataFileName);
  std::string header1 = "Content-Type: application/x-xnat-metadata+xml";
  std::string header2 = "Content-Disposition: x-xnat-metadata; filename=\"" + dataFileNameString + "\"";
  struct curl_slist *cl = NULL;
  cl = curl_slist_append(cl, header1.c_str());
  cl = curl_slist_append(cl, header2.c_str());
  cl = curl_slist_append(cl, "Transfer-Encoding: chunked");
  
  //-- configure the curl handle
  curl_easy_setopt(this->CurlHandle, CURLOPT_VERBOSE, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_HEADERFUNCTION, NULL);
  curl_easy_setopt(this->CurlHandle, CURLOPT_HTTPHEADER, cl);
  curl_easy_setopt(this->CurlHandle, CURLOPT_POST, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_HEADER, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_URL, serverPath);
  curl_easy_setopt(this->CurlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_READFUNCTION, xnd_read_callback);
  curl_easy_setopt(this->CurlHandle, CURLOPT_READDATA, this->LocalFile);
  curl_easy_setopt(this->CurlHandle, CURLOPT_POSTFIELDS, NULL );
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEFUNCTION, NULL); // write_callback);
  curl_easy_setopt(this->CurlHandle, CURLOPT_CONNECTTIMEOUT, 10);    
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEDATA, returnURIFile);

  CURLcode retval = curl_easy_perform(this->CurlHandle);

  if (retval == CURLE_OK)
    {
    vtkDebugMacro("PostMetadata: successful return from curl");
    }
   else
    {
    const char *stringError = curl_easy_strerror(retval);
    vtkErrorMacro("PostMetadata: error running curl: " << stringError);
    }

  curl_slist_free_all(cl);
  this->CloseTransfer();

  if ( this->LocalFile)
    {
    if ( fclose ( this->LocalFile ) != 0)
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
      this->LocalFile = NULL;
    }
  if (returnURIFile)
    {
    if ( fclose ( returnURIFile ) != 0)
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
    }

  return 1;
}







//----------------------------------------------------------------------------
int vtkXNDHandler::DeleteResource ( const char *uri, const char *temporaryResponseFileName )
{
  int result;
  if (uri == NULL )
    {
    vtkErrorMacro("vtkXNDHander::DeleteResource: uri is NULL!");
    return ( 0);
    }

  const char *responseFileName = temporaryResponseFileName;
  FILE *responseFile = fopen(responseFileName, "wb");
  if (responseFile == NULL)
    {
    vtkErrorMacro("vtkXNDHandler::DeleteResource unable to open a local file caled " << responseFileName << " to write out to for capturing the uri");
    return 0;
    }

  this->InitTransfer( );
  if ( this->CurlHandle == NULL )
    {
    vtkErrorMacro ( "Got NULL curl handle." );
    return 0;
    }

  //--- not sure what config options we need...
  curl_easy_setopt(this->CurlHandle, CURLOPT_HTTPGET, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_CUSTOMREQUEST, "DELETE");
  curl_easy_setopt(this->CurlHandle, CURLOPT_URL, uri);
  curl_easy_setopt(this->CurlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_VERBOSE, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEFUNCTION, NULL); // write_callback);
  curl_easy_setopt(this->CurlHandle, CURLOPT_CONNECTTIMEOUT, 10);    
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEDATA, responseFile);
  
  CURLcode retval = curl_easy_perform(this->CurlHandle);

  if (retval == CURLE_OK)
    {
    result = 1;
    vtkDebugMacro("vtkXNDHandler::DeleteResource successful return from curl");
    }
  else
    {
    if (retval == CURLE_BAD_FUNCTION_ARGUMENT)
      {
      result = 0;
      vtkErrorMacro("vtkXNDHandler::DeleteResource bad function argument to curl, did you init CurlHandle?");
      }
    else if (retval == CURLE_OUT_OF_MEMORY)
      {
      result = 0;
      vtkErrorMacro("vtkXNDHandler::DeleteResource curl ran out of memory!");
      }
    else
      {
      result = 0;
      const char *returnString = curl_easy_strerror(retval);
      vtkErrorMacro("vtkXNDHandler::DeleteResource error running curl: " << returnString);
      }
    }

  this->CloseTransfer();

  if (responseFile)
    {
    if ( fclose(responseFile) != 0 )
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
    }
  
  //--- if result = 1, delete response went fine. Otherwise, problem with delete.
  return ( result );
}



//--- for querying
//----------------------------------------------------------------------------
const char *vtkXNDHandler::QueryServer( const char *uri, const char *destination)
{
  const char *returnString;
  if (uri == NULL )
    {
    returnString = "QueryServer: uri is NULL!";
    vtkErrorMacro("QueryServer: uri is NULL!");
    return ( returnString );
    }

  this->LocalFile = fopen(destination, "w");
  if ( this->LocalFile == NULL )
    {
    vtkErrorMacro ( "Could not open file for saving the query response, so no queries were made." );
    return ( "Could not open file for saving query response. No queries made." );
    }

  this->InitTransfer( );
  if ( this->CurlHandle == NULL )
    {
    vtkErrorMacro ( "Got NULL curl handle." );
    return ("Could not init transfer." );
    }

  curl_easy_setopt(this->CurlHandle, CURLOPT_HTTPGET, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_URL, uri);
  curl_easy_setopt(this->CurlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEFUNCTION, NULL); // write_callback);
  curl_easy_setopt(this->CurlHandle, CURLOPT_WRITEDATA, this->LocalFile);
  curl_easy_setopt(this->CurlHandle, CURLOPT_CONNECTTIMEOUT, 10);    
  CURLcode retval = curl_easy_perform(this->CurlHandle);

  if (retval == CURLE_OK)
    {
    returnString = "OK";
    vtkDebugMacro("QueryServer: successful return from curl");
    }
  else
    {
    if (retval == CURLE_BAD_FUNCTION_ARGUMENT)
      {
      returnString = "Bad function argument to cURL.";
      vtkErrorMacro("QueryServer: bad function argument to curl, did you init CurlHandle?");
      }
    else if (retval == CURLE_OUT_OF_MEMORY)
      {
      returnString = "Transfer library (cURL) ran out of memory.";
      vtkErrorMacro("QueryServer: curl ran out of memory!");
      }
    else
      {
      returnString = curl_easy_strerror(retval);
      vtkErrorMacro("QueryServer: error running curl: " << returnString);
      }
    }
  this->CloseTransfer();
  if ( this->LocalFile)
    {
    if ( fclose(this->LocalFile) != 0 )
      {
      vtkErrorMacro ( "A remoteIO file was not properly closed." );
      }
    this->LocalFile = NULL;
    }
  return ( returnString );
}


//----------------------------------------------------------------------------
const char* vtkXNDHandler::GetXMLDeclaration()
{
  const char *returnString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  return  (returnString);
}


//----------------------------------------------------------------------------
const char* vtkXNDHandler::GetNameSpace()
{
  const char *returnString = "xmlns=\"http://nrg.wustl.edu/xe\"";
  return (returnString);
}


//--- for calling by the application. Return 1 for connected, 0 for not.
//----------------------------------------------------------------------------
bool vtkXNDHandler::CheckConnectionAndServer ( const char *uri )
{
  if ( uri == NULL )
    {
    // not used.
    vtkErrorMacro ( "CheckConnectionAndServer: got NULL uri." );
    }

  if (this->GetHostName() == NULL)
    {
    vtkErrorMacro ( "Got NULL Host Name on XNDHandler." );
    return 0;
    }

  const char *hostname = this->GetHostName();
  std::stringstream q;
  q << hostname;
  q << "/tags";
  std::string query = q.str();
  const char *errorString = this->CheckServerStatus(query.c_str() );
  if ((errorString != NULL) && (!strcmp (errorString, "OK")))
    {
    return 1;
    }
  else
    {
    vtkErrorMacro ( "No connection to host." );
    return 0;
    }
}



//----------------------------------------------------------------------------
const char *vtkXNDHandler::CheckServerStatus ( const char *uri )
{

  if ( uri == NULL )
    {
    vtkErrorMacro ( "CheckServerStatus: got NULL uri." );
    return ( "A NULL uri was specified." );
    }

  this->InitTransfer( );
  if ( this->CurlHandle == NULL )
    {
    vtkErrorMacro ( "Got NULL curl handle." );
    return ("Could not initialize check on server status.");
    }


  //--- not sure what config options we need...

  curl_easy_setopt(this->CurlHandle, CURLOPT_HTTPGET, 1);
  curl_easy_setopt(this->CurlHandle, CURLOPT_URL, uri);
  curl_easy_setopt(this->CurlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(this->CurlHandle, CURLOPT_CONNECTTIMEOUT, 10 );
  curl_easy_setopt(this->CurlHandle, CURLOPT_TIMEOUT, 10 );
  CURLcode retval = curl_easy_perform(this->CurlHandle);
  const char *returnString;

/*
  long httpCode;
  long responseCode;
  long connectCode;
  curl_easy_getinfo ( this->CurlHandle, CURLINFO_HTTP_CONNECTCODE, &connectCode );
  curl_easy_getinfo ( this->CurlHandle, CURLINFO_HTTP_CODE, &httpCode );
  curl_easy_getinfo ( this->CurlHandle, CURLINFO_RESPONSE_CODE, &responseCode );

  vtkDebugMacro ("HTTPCONNECTCODE = " << connectCode );
  vtkDebugMacro ("HTTPCODE = " << httpCode);
  vtkDebugMacro ("RESPONSECODE = " << responseCode);
*/
  
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
  this->CloseTransfer();
  return ( returnString );
}
