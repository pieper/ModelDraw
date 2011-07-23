#include "Slicer3Helper.h"


//-----------------------------------------------------------------------------
void Slicer3Helper::AddDataIOToScene(vtkMRMLScene* mrmlScene, vtkSlicerApplication *app, vtkSlicerApplicationLogic *appLogic, vtkDataIOManagerLogic *dataIOManagerLogic)
{

  if (!app)
    {
    app = vtkSlicerApplication::GetInstance();
    }

  if (!app || !appLogic)
    {
      cout << "Parameter of DataIO are not set according to app or appLogic bc one of them is NULL - this might cause issues when downloading data form the web!" << endl;
    }

  // Create Remote I/O and Cache handling mechanisms
  // and configure them using Application registry values
  vtkCacheManager *cacheManager = vtkCacheManager::New();
  if (app)
    {
      cacheManager->SetRemoteCacheLimit ( app->GetRemoteCacheLimit() );
      cacheManager->SetRemoteCacheFreeBufferSize ( app->GetRemoteCacheFreeBufferSize() );
      cacheManager->SetEnableForceRedownload ( app->GetEnableForceRedownload() );
      //cacheManager->SetRemoteCacheDirectory( app->GetRemoteCacheDirectory() );
    }
  cacheManager->SetMRMLScene ( mrmlScene );


  //cacheManager->SetEnableRemoteCacheOverwriting ( app->GetEnableRemoteCacheOverwriting() );
  //--- MRML collection of data transfers with access to cache manager
  vtkDataIOManager *dataIOManager = vtkDataIOManager::New();
  dataIOManager->SetCacheManager ( cacheManager );
  if (app)
    {
      dataIOManager->SetEnableAsynchronousIO ( app->GetEnableAsynchronousIO () );
    }


  //--- Data transfer logic
  // vtkDataIOManagerLogic *dataIOManagerLogic = vtkDataIOManagerLogic::New();
  dataIOManagerLogic->SetMRMLScene ( mrmlScene );
  if (appLogic)
    {
      dataIOManagerLogic->SetApplicationLogic ( appLogic );
    }
  dataIOManagerLogic->SetAndObserveDataIOManager ( dataIOManager );

  mrmlScene->SetDataIOManager ( dataIOManager );
  mrmlScene->SetCacheManager ( cacheManager );


  vtkCollection *URIHandlerCollection = vtkCollection::New();
  // add some new handlers
  mrmlScene->SetURIHandlerCollection( URIHandlerCollection );

#if !defined(REMOTEIO_DEBUG)
  // register all existing uri handlers (add to collection)
  vtkHTTPHandler *httpHandler = vtkHTTPHandler::New();
  httpHandler->SetPrefix ( "http://" );
  httpHandler->SetName ( "HTTPHandler");
  mrmlScene->AddURIHandler(httpHandler);
  httpHandler->Delete();

  vtkSRBHandler *srbHandler = vtkSRBHandler::New();
  srbHandler->SetPrefix ( "srb://" );
  srbHandler->SetName ( "SRBHandler" );
  mrmlScene->AddURIHandler(srbHandler);
  srbHandler->Delete();

  vtkXNATHandler *xnatHandler = vtkXNATHandler::New();
  vtkSlicerXNATPermissionPrompterWidget *xnatPermissionPrompter = vtkSlicerXNATPermissionPrompterWidget::New();
  if (app)
    {
      xnatPermissionPrompter->SetApplication ( app );
    }
  xnatPermissionPrompter->SetPromptTitle ("Permission Prompt");
  xnatHandler->SetPrefix ( "xnat://" );
  xnatHandler->SetName ( "XNATHandler" );
  xnatHandler->SetRequiresPermission (1);
  xnatHandler->SetPermissionPrompter ( xnatPermissionPrompter );
  mrmlScene->AddURIHandler(xnatHandler);
  xnatPermissionPrompter->Delete();
  xnatHandler->Delete();

  vtkHIDHandler *hidHandler = vtkHIDHandler::New();
  hidHandler->SetPrefix ( "hid://" );
  hidHandler->SetName ( "HIDHandler" );
  mrmlScene->AddURIHandler( hidHandler);
  hidHandler->Delete();

  vtkXNDHandler *xndHandler = vtkXNDHandler::New();
  xndHandler->SetPrefix ( "xnd://" );
  xndHandler->SetName ( "XNDHandler" );
  mrmlScene->AddURIHandler( xndHandler);
  xndHandler->Delete();

  //add something to hold user tags
  vtkTagTable *userTagTable = vtkTagTable::New();
  mrmlScene->SetUserTagTable( userTagTable );
  userTagTable->Delete();
#endif
}

void Slicer3Helper::RemoveDataIOFromScene(vtkMRMLScene* mrmlScene, vtkDataIOManagerLogic *dataIOManagerLogic)
{
  if ( dataIOManagerLogic != NULL )
    {
      dataIOManagerLogic->SetAndObserveDataIOManager ( NULL );
      dataIOManagerLogic->SetMRMLScene ( NULL );
    }

  vtkDataIOManager* dataIOManager = mrmlScene->GetDataIOManager();
  if ( dataIOManager != NULL )
    {
      mrmlScene->SetDataIOManager(NULL);
      dataIOManager->SetCacheManager(NULL);
      dataIOManager->Delete();
    }

  vtkCacheManager* cacheManager = mrmlScene->GetCacheManager();
  if ( cacheManager != NULL )
    {
      mrmlScene->SetCacheManager(NULL);
      cacheManager->SetMRMLScene ( NULL );
      cacheManager->Delete();
    }

  vtkCollection* URIHandlerCollection = mrmlScene->GetURIHandlerCollection();
  if (URIHandlerCollection != NULL )
    {
      mrmlScene->SetURIHandlerCollection(NULL);
      URIHandlerCollection->Delete();
    }

  mrmlScene->SetUserTagTable( NULL );

}

//-----------------------------------------------------------------------------
const char* Slicer3Helper::GetSvnRevision()
{
  return vtkSlicerApplication::GetInstance()->GetSvnRevision();
}

//-----------------------------------------------------------------------------
const char* Slicer3Helper::GetTemporaryDirectory()
{
  return vtkSlicerApplication::GetInstance()->GetTemporaryDirectory();
}
