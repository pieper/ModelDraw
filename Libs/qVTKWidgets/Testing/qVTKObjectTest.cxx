#include "qVTKObjectTest.h"

// Qt includes
#include <QDebug>
#include <QApplication>

// VTK includes
#include <vtkObject.h>

//------------------------------------------------------------------------------
class qVTKObjectTestPrivate : public qCTKPrivate<qVTKObjectTest>
{
public:
  qVTKObjectTestPrivate();
  int PublicSlotCalled ;
  int ProtectedSlotCalled;
  int PrivateSlotCalled;
};

//------------------------------------------------------------------------------
qVTKObjectTestPrivate::qVTKObjectTestPrivate()
{
  this->PublicSlotCalled = 0;
  this->ProtectedSlotCalled = 0;
  this->PrivateSlotCalled = 0;
}

//------------------------------------------------------------------------------
qVTKObjectTest::qVTKObjectTest()
{
  QCTK_INIT_PRIVATE(qVTKObjectTest);
}

bool qVTKObjectTest::test()
{
  QCTK_D(qVTKObjectTest);
  // should do nothing but shouldn't fail neither
  qDebug() << "The following can generate error message.";
  qDebug() << "Disconnect:";
  this->qvtkDisconnect(0, static_cast<unsigned long>(-1), this, SLOT(onVTKObjectModifiedPublic()));
  qDebug() << "Connect:";
  QString connection = this->qvtkConnect(0, static_cast<unsigned long>(-1), this, SLOT(onVTKObjectModifiedPublic()));
  if (!connection.isEmpty())
    {
    qDebug() << "qVTKObject::qvtkConnect() failed: "<< connection;
    return false;
    }
  qDebug() << "Reconnect:";
  connection = this->qvtkReconnect(0, 0, static_cast<unsigned long>(-1), this, SLOT(onVTKObjectModifiedPublic()));
  if (!connection.isEmpty())
    {
    qDebug() << "qVTKObject::qvtkReconnect() failed: "<< connection;
    return false;
    }
  qDebug() << "End of possible error messages.";
  
  vtkObject* object = vtkObject::New();

  connection = this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                                 this, SLOT(onVTKObjectModifiedPublic()));
  if (connection.isEmpty() || object->GetReferenceCount() != 1)
    {
    qDebug() << "qVTKObject::qvtkConnect() failed: "<< connection;
    return false;
    }

  object->Modified();

  if (d->PublicSlotCalled != 1)
    {
    qDebug() << "qvtkConnect failed";
    return false;
    }
  
  this->resetSlotCalls();
  // should do nothing...
  connection = this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                                 this, SLOT(onVTKObjectModifiedPublic()));
  if (!connection.isEmpty())
    {
    qDebug() << __LINE__ << "qVTKObject::qvtkConnect() failed: "<< connection;
    return false;
    }

  object->Modified();
  
  if (d->PublicSlotCalled != 1)
    {
    qDebug() << __LINE__ << "qvtkConnect failed";
    return false;
    }

  this->resetSlotCalls();

  this->qvtkDisconnect(object, vtkCommand::WarningEvent, 
                       this, SLOT(onVTKObjectModifiedPublic()));
  object->Modified();

  if (d->PublicSlotCalled != 1)
    {
    qDebug() << __LINE__ << "qvtkDisconnect failed" << d->PublicSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  this->qvtkDisconnect(object, vtkCommand::ModifiedEvent, 
                       this, SLOT(onVTKObjectModifiedPublic()));
  QCoreApplication::instance()->processEvents();
  object->Modified();

  if (d->PublicSlotCalled != 0)
    {
    qDebug() << __LINE__ << "qvtkDisconnect failed" << d->PublicSlotCalled;
    return false;
    }
  this->resetSlotCalls();
  
  // Set a new connection (protected)
  connection = this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                                 this, SLOT( onVTKObjectModifiedProtected ( ) ));
  if (connection.isEmpty())
    {
    qDebug() << __LINE__ << "qVTKObject::qvtkConnect() failed: "<< connection;
    return false;
    }

  object->Modified();
  
  if (d->ProtectedSlotCalled != 1)
    {
    qDebug() << __LINE__ << "qVTKObject::qvtkConnect failed" << d->ProtectedSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  // remove the connection using flags, 0 means any event, qt object or slot
  this->qvtkDisconnect(object, vtkCommand::NoEvent, 0, 0);
  object->Modified();

  if (d->ProtectedSlotCalled != 0)
    {
    qDebug() << __LINE__ << "qvtkDisconnect failed" << d->ProtectedSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  // Set new connections
  this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                    this, SLOT(onVTKObjectModifiedProtected()));
  this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                    this, SLOT(onVTKObjectModifiedPrivate()));
  object->Modified();
  if (d->ProtectedSlotCalled != 1 || 
      d->PrivateSlotCalled != 1)
    {
    qDebug() << __LINE__ << "qvtkConnect failed" 
             << d->ProtectedSlotCalled 
             << d->PrivateSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  // remove the connection using flags, 0 means any event, qt object or slot
  this->qvtkDisconnect(object, vtkCommand::ModifiedEvent, this, 0);
  object->Modified();
  if (d->ProtectedSlotCalled != 0 || d->PrivateSlotCalled != 0)
    {
    qDebug() << __LINE__ << "qvtkDisconnect failed" 
             << d->ProtectedSlotCalled
             << d->PrivateSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  // Set new connections
  this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                    this, SLOT(onVTKObjectModifiedPublic ()));
  this->qvtkConnect(object, vtkCommand::WarningEvent, 
                    this, SLOT(onVTKObjectModifiedPublic( )));
  int disconnected = this->qvtkDisconnect(object, vtkCommand::NoEvent, 
                                          this, SLOT(onVTKObjectModifiedPublic() ));
  if (disconnected != 2)
    {
    qDebug() << __LINE__ << "qvtkDisconnect failed" << disconnected;
    return false;
    }

  object->InvokeEvent(vtkCommand::ModifiedEvent, 0);
  object->InvokeEvent(vtkCommand::WarningEvent, 0);

  if (d->PublicSlotCalled != 0)
    {
    qDebug() << __LINE__ << "qvtkConnect failed" 
             << d->PublicSlotCalled;
    return false;
    }
  this->resetSlotCalls();

  disconnected = this->qvtkDisconnectAll();
  if (disconnected != 0)
    {
    qDebug() << __LINE__ << "qvtkDisconnectAll failed" << disconnected;
    return false;
    }
  this->qvtkConnect(object, vtkCommand::ModifiedEvent, 
                    this, SLOT(deleteConnection()));
  object->InvokeEvent(vtkCommand::ModifiedEvent, 0);

  object->Delete();
  
  return true;
}

//------------------------------------------------------------------------------
void qVTKObjectTest::resetSlotCalls()
{
  QCTK_D(qVTKObjectTest);
  d->PublicSlotCalled = 0;
  d->ProtectedSlotCalled = 0;
  d->PrivateSlotCalled = 0;
}

//------------------------------------------------------------------------------
void qVTKObjectTest::onVTKObjectModifiedPublic()
{
  qDebug() << __FUNCTION__;
  qctk_d()->PublicSlotCalled = true;
}

//------------------------------------------------------------------------------
void qVTKObjectTest::deleteConnection()
{
  qDebug() << __FUNCTION__;
  this->qvtkDisconnect(0, vtkCommand::NoEvent, 0, 0);
}

//------------------------------------------------------------------------------
void qVTKObjectTest::onVTKObjectModifiedProtected()
{
  qDebug() << __FUNCTION__;
  qctk_d()->ProtectedSlotCalled = true;
}

//------------------------------------------------------------------------------
void qVTKObjectTest::onVTKObjectModifiedPrivate()
{
  qDebug() << __FUNCTION__;
  qctk_d()->PrivateSlotCalled = true;
}
