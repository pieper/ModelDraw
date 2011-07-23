/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLScene.h,v $
Date:      $Date: 2006/03/17 15:10:09 $
Version:   $Revision: 1.18 $

=========================================================================auto=*/
//
//
///  vtkMRMLScene - A set of MRML Nodes that supports serialization and undo/redo
/// 
/// vtkMRMLScene represents and provides methods to manipulate a list of
/// MRML objects. The list is core and duplicate entries are not prevented.
//
/// .SECTION see also
/// vtkMRMLNode vtkCollection 

#ifndef __vtkMRMLScene_h
#define __vtkMRMLScene_h

#include <list>
#include <map>
#include <vector>
#include <string>

#include "vtkCollection.h"
#include "vtkObjectFactory.h"

#include "vtkMRML.h"
//#include "vtkMRMLNode.h"
class vtkMRMLNode;
#include "vtkCacheManager.h"
#include "vtkDataIOManager.h"
#include "vtkTagTable.h"

class vtkCallbackCommand;
class vtkGeneralTransform;
class vtkURIHandler;

class VTK_MRML_EXPORT vtkMRMLScene : public vtkCollection
{
public:
  static vtkMRMLScene *New();
  vtkTypeMacro(vtkMRMLScene,vtkCollection);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// 
  /// Set URL (file name) of the scene
  void SetURL(const char *url) {
    this->URL = std::string(url); 
  };
  
  /// 
  /// Get URL (file name) of the scene  
  const char *GetURL() {
    return this->URL.c_str();
  };
  
  /// 
  /// Set Root directory, where URL is pointing
  void SetRootDirectory(const char *dir) {
    this->RootDirectory = std::string(dir); 
  };
  
  /// 
  /// Get Root directory, where URL is pointing  
  const char *GetRootDirectory() {
    return this->RootDirectory.c_str();
  };
  
  /// 
  /// Create new scene from URL
  int Connect();
  
  /// 
  /// Add the scene from URL
  int Import();

  /// 
  /// Save scene into URL
  int Commit(const char* url=NULL);

  /// 
  /// Remove nodes and clear undo/redo stacks
  void Clear(int removeSingletons);

  /// 
  /// Reset all nodes to their constructor's state
  void ResetNodes();
  
  /// 
  /// Create node with a given class
  vtkMRMLNode* CreateNodeByClass(const char* className);

  /// 
  /// Register node class with the Scene so that it can create it from
  /// a class name
  /// -- this maintains a registered pointer to the node, so users should Delete()
  ///    the node after calling this.  The node is Deleted when the scene is destroyed.
  void RegisterNodeClass(vtkMRMLNode* node);
  
  /// 
  /// Add a path to the list.
  const char* GetClassNameByTag(const char *tagName);

  /// 
  /// Add a path to the list.
  const char* GetTagByClassName(const char *className);

  /// 
  /// return collection of nodes
  vtkCollection* GetCurrentScene()
    {
    return this->CurrentScene;
    };

  /// 
  /// Called by another class to request that the node's id be set to the given
  /// string
  /// If the id is not in use, set it, otherwise, useit as a base for a unique
  /// id and then set it
  void RequestNodeID(vtkMRMLNode *node, const char *ID);
  
  /// 
  /// Add a node to the scene and send NewNode and SceneModified events.
  vtkMRMLNode* AddNode(vtkMRMLNode *n);

  /// 
  /// Add a copy of a node to the scene.
  vtkMRMLNode* CopyNode(vtkMRMLNode *n);

  /// 
  /// Add a node to the scene without invoking a NodeAdded Event
  vtkMRMLNode* AddNodeNoNotify(vtkMRMLNode *n);

  /// 
  /// Invoke a NodeAddedEvent (used, for instnace, after adding a bunch of nodes with AddNodeNoNotify
  void NodeAdded(vtkMRMLNode *n);
  void NodeAdded() {this->NodeAdded(NULL);};
  
  /// 
  /// Remove a path from the list.
  void RemoveNode(vtkMRMLNode *n); 

  /// 
  /// Remove a path from the list without invoking NodeRemovedEvent
  /// - use this when there are no references to the passed node (e.g. singletons 
  ///   on scene load)
  void RemoveNodeNoNotify(vtkMRMLNode *n); 
  
  /// 
  /// Determine whether a particular node is present. Returns its position
  /// in the list.
  int IsNodePresent(vtkMRMLNode *n) {
    return this->CurrentScene->vtkCollection::IsItemPresent((vtkObject *)n);};

  /// 
  /// Initialize a traversal (not reentrant!)
  void InitTraversal() { 
    if (this && this->CurrentScene) 
      {
      this->CurrentScene->InitTraversal(); 
      }
  };
  
  /// 
  /// Get next node in the scene.
  vtkMRMLNode *GetNextNode() {
    return (vtkMRMLNode *)(this->CurrentScene->GetNextItemAsObject());};
  
  /// 
  /// Get next node of the class in the scene.
  vtkMRMLNode *GetNextNodeByClass(const char* className);

  /// 
  /// Get nodes having the specified name
  vtkCollection *GetNodesByName(const char* name);

  /// 
  /// Get node given a unique ID
  vtkMRMLNode *GetNodeByID(const char* name);
  //BTX
  vtkMRMLNode *GetNodeByID(std::string name);
  //ETX
  
  /// 
  /// Get nodes of a specified class having the specified name
  vtkCollection *GetNodesByClassByName(const char* className, const char* name);
  
  /// 
  /// Get number of nodes in the scene
  int GetNumberOfNodes () { return this->CurrentScene->GetNumberOfItems(); };

  /// 
  /// Get n-th node in the scene
  vtkMRMLNode* GetNthNode(int n);
  
  /// 
  /// Get n-th node of a specified class  in the scene 
  vtkMRMLNode* GetNthNodeByClass(int n, const char* className );
  
  /// 
  /// Get number of nodes of a specified class in the scene
  int GetNumberOfNodesByClass(const char* className);
  
  /// 
  /// Get vector of nodes of a specified class in the scene
  //BTX
  int GetNodesByClass(const char *className, std::vector<vtkMRMLNode *> &nodes);
  //ETX
  
  //BTX
  std::list<std::string> GetNodeClassesList();
  //ETX
  
  /// 
  /// returns list of names
  const char* GetNodeClasses();
  
  /// 
  /// Get the number of registered node classes (is probably greater than the current number 
  /// of nodes instantiated in the scene) 
  int GetNumberOfRegisteredNodeClasses();
  /// 
  /// Get the nth registered node class, returns NULL if n is out of the range 0-GetNumberOfRegisteredNodeClasses
  /// Useful for iterating through nodes to find all the possible storage nodes.
  vtkMRMLNode * GetNthRegisteredNodeClass(int n);

  const char* GetUniqueNameByString(const char* className);
  /// 
  /// Explore the MRML tree to find the next unique index for use as an ID,
  /// starting from 1
  int GetUniqueIDIndexByClass(const char* className);
  /// 
  /// Explore the MRML tree to find the next unique index for use as an ID,
  /// starting from hint
  int GetUniqueIDIndexByClassFromIndex(const char* className, int hint);
  
  /// 
  /// insert a node in the scene after a specified node
  void InsertAfterNode( vtkMRMLNode *item, vtkMRMLNode *newItem);
  /// 
  /// insert a node in the scene before a specified node
  void InsertBeforeNode( vtkMRMLNode *item, vtkMRMLNode *newItem);
  
  /// 
  /// Ger transformation between two nodes
  int GetTransformBetweenNodes( vtkMRMLNode *node1, vtkMRMLNode *node2, 
                                vtkGeneralTransform *xform );

  /// 
  /// Set undo on/off
  void SetUndoOn() {UndoFlag=true;};
  void SetUndoOff() {UndoFlag=false;};
  bool GetUndoFlag() {return UndoFlag;};
  void SetUndoFlag(bool flag) {UndoFlag = flag;};

  /// 
  /// undo, set the scene to previous state
  void Undo();
  
  /// 
  /// redo, set the scene to previously undone
  void Redo();

  /// 
  /// clear Undo stack, delete undo history
  void ClearUndoStack();
  
  /// 
  /// clear Redo stack, delete redo history
  void ClearRedoStack();

  ///  
  /// returns number of undo steps in the history buffer
  int GetNumberOfUndoLevels() { return (int)this->UndoStack.size();};
  
  ///  
  /// returns number of redo steps in the history buffer
  int GetNumberOfRedoLevels() { return (int)this->RedoStack.size();};

  ///  
  /// Save current state in the undo buffer
  void SaveStateForUndo();
  ///  
  /// Save current state of the node in the undo buffer
  void SaveStateForUndo(vtkMRMLNode *node);
  ///  
  /// Save current state of the nodes in the undo buffer
  void SaveStateForUndo(vtkCollection *nodes);
  //BTX
  void SaveStateForUndo(std::vector<vtkMRMLNode *> nodes);
  //ETX

  ///
  /// add a reference node id from a refrencingNode
  /// scene internally keeps references from/to nodes
  void AddReferencedNodeID(const char *id, vtkMRMLNode *refrencingNode)
  {
    if (id && refrencingNode && refrencingNode->GetScene() && refrencingNode->GetID()) 
      {
      this->ReferencedIDs.push_back(id);
      this->ReferencingNodes.push_back(refrencingNode);
      }
  };

  ///
  /// clear all node references
  /// scene internally keeps references from/to nodes
  void ClearReferencedNodeID()
  {
    this->ReferencedIDs.clear();
    this->ReferencingNodes.clear();
    this->ReferencedIDChanges.clear();
  };

  ///
  /// remove a reference node id from a refrencingNode
  /// scene internally keeps references from/to nodes 
  void RemoveReferencedNodeID(const char *id, vtkMRMLNode *refrencingNode);

  ///
  /// remove all references from a node 
  /// scene internally keeps references from/to nodes
  void RemoveNodeReferences(vtkMRMLNode *node);

  ///
  /// remove all references to a node 
  /// scene internally keeps references from/to nodes
  void RemoveReferencesToNode(vtkMRMLNode *node);

  ///
  /// update all references from/to nodes in the scene
  /// scene internally keeps references from/to nodes
  void UpdateNodeReferences();

  ///
  /// update all references from/to nodes in the collection
  /// scene internally keeps references from/to nodes
  void UpdateNodeReferences(vtkCollection* chekNodes);

  ///
  /// copy all references from/to nodes in the scene
  /// scene internally keeps references from/to nodes
  void CopyNodeReferences(vtkMRMLScene *scene);

  ///
  /// update all references from/to nodes in the scene after loading a scene
  /// scene internally keeps references from/to nodes
  void UpdateNodeChangedIDs();

  ///
  /// remove all references from/to non-existing nodes in the scene
  /// scene internally keeps references from/to nodes
  void RemoveUnusedNodeReferences();

  ///
  /// add a reserved id that cannot be used when adding nodes
  void AddReservedID(const char *id);

  ///
  /// remove all reserved id that cannot be used when adding nodes
  void RemoveReservedIDs() {
    this->ReservedIDs.clear();
  };

  ///  
  /// get the new id of the node that is different from one in the mrml file
  /// or NULL if id has not changed
  const char* GetChangedID(const char* id);

  ///  
  /// Return collection of all nodes referenced directly or indirectly by a node.
  vtkCollection* GetReferencedNodes(vtkMRMLNode *node);

  ///
  /// Get a sub-scene containing all nodes directly or indirectly reference by
  /// the input node
  void GetReferencedSubScene(vtkMRMLNode *node, vtkMRMLScene* newScene);


//BTX
  /// 
  /// Get/Set the active Scene 
  static void SetActiveScene(vtkMRMLScene *);
  static vtkMRMLScene *GetActiveScene();
//ETX

//BTX
  ///
  /// Scene evenets
  enum
    {
      NodeAddedEvent = 66000,
      NodeRemovedEvent = 66001,
      NewSceneEvent = 66002,
      SceneCloseEvent = 66003,
      SceneClosingEvent = 66004,
      SceneLoadingErrorEvent = 66005,
      SceneEditedEvent = 66006,
      MetadataAddedEvent = 66007,
      LoadProgressFeedbackEvent = 66008,
      SaveProgressFeedbackEvent = 66009,
      SceneLoadStartEvent = 66010,
      SceneLoadEndEvent = 66011,
      SceneRestoredEvent = 66008,
      NodeAboutToBeAddedEvent = 66012,
      NodeAboutToBeRemovedEvent = 66013
    };
//ETX

  ///
  /// returns 1 if file path is relative
  int IsFilePathRelative(const char * filepath);

  ///
  /// get/set error code
  vtkSetMacro(ErrorCode,unsigned long);
  vtkGetMacro(ErrorCode,unsigned long);

  ///
  /// get/set load from XML string flag
  vtkSetMacro(LoadFromXMLString,int);
  vtkGetMacro(LoadFromXMLString,int);

  ///
  /// get/set save to XML string flag
  vtkSetMacro(SaveToXMLString,int);
  vtkGetMacro(SaveToXMLString,int);

//BTX
  ///
  /// set the string in which to save XML
  void SetSceneXMLString(const std::string &xmlString) {
    this->SceneXMLString = xmlString;
  };

  ///
  /// get the string in which to save XML
  std::string GetSceneXMLString() {
    return this->SceneXMLString;
  };
//ETX

  ///
  /// get/set read raw data on load flag
  vtkSetMacro(ReadDataOnLoad,int);
  vtkGetMacro(ReadDataOnLoad,int);

//BTX
  ///
  /// set error message
  void SetErrorMessage(const std::string &error) {
    this->ErrorMessage = error;
  };

  ///
  /// get error message
  std::string GetErrorMessage() {
    return this->ErrorMessage;
  };
//ETX

  ///
  /// set error message
  void SetErrorMessage(const char * message)
    {
    this->SetErrorMessage(std::string(message));
    }

  ///
  /// get error message
  const char *GetErrorMessagePointer()
    {
    return (this->GetErrorMessage().c_str());
    }

  ///
  /// get scene modified time
  unsigned long GetSceneModifiedTime()
    {
    if (this->CurrentScene && this->CurrentScene->GetMTime() > this->SceneModifiedTime)
      {
      this->SceneModifiedTime = this->CurrentScene->GetMTime();
      }
    return this->SceneModifiedTime;
    };
    
  ///
  /// increment scene modified time
  void IncrementSceneModifiedTime()
    {
    this->SceneModifiedTime ++;
    };

  ///
  /// invoke SceneEditedEvent event
  void Edited()
    {
    this->InvokeEvent( vtkMRMLScene::SceneEditedEvent );
    }


  ///
  /// get/set CacheManager
  vtkGetObjectMacro ( CacheManager, vtkCacheManager );
  vtkSetObjectMacro ( CacheManager, vtkCacheManager );
  ///
  /// get/set DataIOManager
  vtkGetObjectMacro ( DataIOManager, vtkDataIOManager );
  vtkSetObjectMacro ( DataIOManager, vtkDataIOManager );
  ///
  /// get/set URIHandlerCollection
  vtkGetObjectMacro ( URIHandlerCollection, vtkCollection );
  vtkSetObjectMacro ( URIHandlerCollection, vtkCollection );
  ///
  /// get/set UserTagTable
  vtkGetObjectMacro ( UserTagTable, vtkTagTable);
  vtkSetObjectMacro ( UserTagTable, vtkTagTable);  

  /// 
  /// find a URI handler in the collection that can work on the passed URI
  /// returns NULL on failure
  vtkURIHandler *FindURIHandler(const char *URI);
  /// 
  /// Returns a URIhandler of a specific type if its name is known.
  vtkURIHandler *FindURIHandlerByName ( const char *name );
  /// 
  /// Add a uri handler to the collection.
  void AddURIHandler(vtkURIHandler *handler);

  /// 
  /// IsClosed is true during scene loads
  /// By checking this flag, logic and gui code can choose
  /// to ignore transient modified events and related events
  vtkGetMacro( IsClosed, int );
  vtkSetMacro( IsClosed, int );
  
  /// 
  /// the version of the last loaded scene file
  vtkGetStringMacro(LastLoadedVersion);
  vtkSetStringMacro(LastLoadedVersion);

  /// 
  /// the current software version
  vtkGetStringMacro(Version);
  vtkSetStringMacro(Version);

  ///
  /// returns 1 if any of the nodes has been modified since read
  int IsModifiedSinceRead();

  ///
  /// copy registered nodes from this scene to the argument scene
  void CopyRegisteredNodesToScene(vtkMRMLScene *scene);

protected:
  vtkMRMLScene();
  ~vtkMRMLScene();
  vtkMRMLScene(const vtkMRMLScene&);
  void operator=(const vtkMRMLScene&);
  
  void PushIntoUndoStack();
  void PushIntoRedoStack();

  void CopyNodeInUndoStack(vtkMRMLNode *node);
  void CopyNodeInRedoStack(vtkMRMLNode *node);

  void AddReferencedNodes(vtkMRMLNode *node, vtkCollection *refNodes);

  /// 
  /// Handle vtkMRMLScene::DeleteEvent: clear the scene
  static void SceneCallback( vtkObject *caller, unsigned long eid, 
                             void *clientData, void *callData );
  vtkCollection* CurrentScene;
  
  /// data i/o handling members
  vtkCacheManager *CacheManager;
  vtkDataIOManager *DataIOManager;
  vtkCollection *URIHandlerCollection;
  vtkTagTable *UserTagTable;

  unsigned long SceneModifiedTime;
  
  int UndoStackSize;
  bool UndoFlag;
  
  bool InUndo;

  //BTX
  std::list< vtkCollection* >  UndoStack;
  std::list< vtkCollection* >  RedoStack;
  //ETX
  
  //BTX
  std::string         URL;
  std::map< std::string, int> UniqueIDByClass;
  std::vector< std::string >  UniqueIDs;
  std::vector< vtkMRMLNode* > RegisteredNodeClasses;
  std::vector< std::string >  RegisteredNodeTags;
  std::string          RootDirectory;

  std::vector< std::string > ReferencedIDs;
  std::vector< vtkMRMLNode* > ReferencingNodes;
  std::map< std::string, std::string> ReferencedIDChanges;
  
  //vtksys::hash_map<const char*, vtkMRMLNode*> NodeIDs;
  std::map<std::string, vtkMRMLNode*> NodeIDs;
  std::map<std::string, int> ReservedIDs;

  std::string ErrorMessage;

  std::string SceneXMLString;
  //ETX
  
  int LoadFromXMLString;

  int SaveToXMLString;

  int ReadDataOnLoad;

  void UpdateNodeIDs();

  unsigned long NodeIDsMTime;

  void RemoveAllNodesExceptSingletons();

  vtkSetStringMacro(ClassNameList);
  vtkGetStringMacro(ClassNameList);

  char *Version;

  char *LastLoadedVersion;
  vtkCallbackCommand *DeleteEventCallback;
  
private:
  /// hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->CurrentScene->vtkCollection::AddItem(o); this->Modified();};
  void RemoveItem(vtkObject *o) { this->CurrentScene->vtkCollection::RemoveItem(o); this->Modified();};
  void RemoveItem(int i) { this->CurrentScene->vtkCollection::RemoveItem(i); this->Modified();};
  int  IsItemPresent(vtkObject *o) { return this->CurrentScene->vtkCollection::IsItemPresent(o);};
  
  int LoadIntoScene(vtkCollection* scene);

  unsigned long ErrorCode;

  char* ClassNameList;

  static vtkMRMLScene *ActiveScene;

  int IsClosed;
};

#endif
