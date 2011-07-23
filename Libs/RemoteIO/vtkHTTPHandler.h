#ifndef __vtkHTTPHandler_h
#define __vtkHTTPHandler_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <slicerlibcurl/slicerlibcurl.h>

#include <vtkRemoteIOConfigure.h>
#include "vtkRemoteIO.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"

//--- derived from libMRML class
#include "vtkURIHandler.h"

class VTK_RemoteIO_EXPORT vtkHTTPHandler : public vtkURIHandler 
{
  public:
  
  /// The Usual vtk class functions
  static vtkHTTPHandler *New();
  vtkTypeRevisionMacro(vtkHTTPHandler, vtkURIHandler);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// 
  /// This methods returns 1 if the handler matches the uri's required 
  /// protocol and returns 0 if it's not appropriate for the uri.
  virtual int CanHandleURI ( const char *uri );

  /// 
  /// Some web servers don't handle 'keep alive' socket transactions
  /// in a way that's compatible with curl on windows.  When this flag is set
  /// curl will do one transaction per connection with the side-effect
  /// that more network resources are used (so avoid this if you can).
  vtkSetMacro(ForbidReuse, int);
  vtkGetMacro(ForbidReuse, int);

  /// 
  /// This function wraps curl functionality to download a specified URL to a specified dir
  void StageFileRead(const char * source, const char * destination);
  //BTX
  using vtkURIHandler::StageFileRead;
  //ETX
  void StageFileWrite(const char * source, const char * destination);
  //BTX
  using vtkURIHandler::StageFileWrite;
  //ETX
  ///
  /// This function can be called by the application directly before download attempt.
  /// re-implement in any subclass to condition the appropriate
  /// query and parse the appropriate response.
  /// returns 1 if connected, 0 if fail.
  virtual bool CheckConnectionAndServer ( const char *uri );

  ///
  /// This function checks to see if the remote host can be reached.
  /// This should be called prior to any attempt to GET or PUT,
  /// and its return value is either "OK" or an error string that
  /// can be posted to the user.
  virtual const char *CheckServerStatus ( const char *uri );

  virtual void InitTransfer ( );
  virtual int CloseTransfer ( );

  CURL* CurlHandle;  

 private:

 protected:
  vtkHTTPHandler();
  virtual ~vtkHTTPHandler();
  vtkHTTPHandler(const vtkHTTPHandler&);
  void operator=(const vtkHTTPHandler&);
  int ForbidReuse;

};

#endif

