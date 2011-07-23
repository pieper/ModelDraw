/*=auto=========================================================================

Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile$
Date:      $Date: 2007-12-20 16:55:07 -0500 (Thu, 20 Dec 2007) $
Version:   $Revision: 5304 $

=========================================================================auto=*/

#include "EventBrokerTestCLP.h"

#include "vtkEventBroker.h"
#include "vtkObservation.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkImageEllipsoidSource.h"
#include "vtkImageViewer.h"
#include "vtkTimerLog.h"

//----------------------------------------------------------------------------
void Callback(vtkObject *caller, unsigned long eid, void *clientData, void *vtkNotUsed(callData))
{
  std::cerr << "Got an event " << eid << " from " << caller->GetClassName() << "\n";

  vtkImageViewer *viewer = reinterpret_cast<vtkImageViewer *>(clientData);
  viewer->Render();
}

int main(int argc, char * argv[])
{
  PARSE_ARGS;

  vtkEventBroker *broker = vtkEventBroker::GetInstance();
  vtkCallbackCommand *callback = vtkCallbackCommand::New();
  vtkImageEllipsoidSource *ellip = vtkImageEllipsoidSource::New();
  vtkImageViewer *viewer = vtkImageViewer::New();

  viewer->SetColorWindow(200);
  viewer->SetColorLevel(100);
  viewer->SetInput( ellip->GetOutput() );

  callback->SetCallback( Callback ); 
  callback->SetClientData( reinterpret_cast<void *> (viewer) );

  size_t i, numberOfObservations = 1;
  for (i = 0; i < numberOfObservations; i++)
    {
    broker->AddObservation( ellip, vtkCommand::ModifiedEvent, viewer, callback);
    }

  broker->AddObservation( ellip, vtkCommand::ModifiedEvent, ellip, callback);

  if ( logFileName != "" )
    {
    broker->SetLogFileName( logFileName.c_str() );
    broker->EventLoggingOn();
    }

  double startTime = vtkTimerLog::GetUniversalTime();
  std::cerr << "Starting at: " << startTime << "\n";

  std::cerr << "Three synchonous events:\n";
  broker->SetEventModeToSynchronous();
  ellip->Modified();
  ellip->Modified();
  ellip->Modified();

  std::cerr << "Three asynchonous events:\n";
  broker->SetEventModeToAsynchronous();
  ellip->Modified();
  ellip->Modified();
  ellip->Modified();
  broker->ProcessEventQueue();

  double endTime = vtkTimerLog::GetUniversalTime();
  double time = endTime - startTime;
  std::cerr << "Ending at: " << endTime << "\n";
  std::cerr << "Delta: " << time << "\n";

  if ( graphFile != "" )
    {
    broker->GenerateGraphFile( graphFile.c_str() );
    }
    broker->GenerateGraphFile( "/tmp/1.dot" );

  // Get observations
  // - check that we can find the one we have created
  std::vector< vtkObservation *> obs;
  obs = broker->GetObservationsForSubjectByTag (ellip, 0);
  if ( obs.size() != numberOfObservations + 1 )
    {
    std::cerr << "Couldn't find observations for ellip\n";
    }
  obs = broker->GetObservations (ellip, ellip);
  if ( obs.size() != 1 )
    {
    std::cerr << "Couldn't find observation for ellip on itself\n";
    }
  obs = broker->GetObservations (ellip, viewer);
  if ( obs.size() != numberOfObservations )
    {
    std::cerr << "Couldn't find observations for viewer\n";
    }


  viewer->SetInput( NULL );
  obs = broker->GetObservationsForSubjectByTag (ellip, 0);
  if ( obs.size() != numberOfObservations + 1 )
    {
    std::cerr << "Couldn't find observations for ellip\n";
    }
  ellip->Delete();


  // Get observations
  // - check that everything cleaned up
  obs = broker->GetObservationsForSubjectByTag (ellip, 0);
  if ( obs.size() != 0 )
    {
    std::cerr << "observations still exist for ellip\n";
    }
  obs = broker->GetObservations (ellip, ellip);
  if ( obs.size() != 0 )
    {
    std::cerr << "observations still exist for ellip on itself\n";
    }
  obs = broker->GetObservations (ellip, viewer);
  if ( obs.size() != 0 )
    {
    std::cerr << "observations still exist for viewer\n";
    }

  if ( graphFile != "" )
    {
    broker->GenerateGraphFile( graphFile.c_str() );
    }
    broker->GenerateGraphFile( "/tmp/2.dot" );

  viewer->Delete();
  callback->Delete();

  return (EXIT_SUCCESS);
}
