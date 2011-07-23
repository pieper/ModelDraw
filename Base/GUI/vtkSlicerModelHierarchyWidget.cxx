#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

#include "vtkMRMLModelHierarchyNode.h"
#include "vtkMRMLModelDisplayNode.h"

#include "vtkSlicerModelHierarchyWidget.h"
#include "vtkSlicerModelHierarchyLogic.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerModelDisplayWidget.h"

#include "vtkKWCheckButton.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWIcon.h"
#include "vtkKWLabel.h"
#include "vtkKWLabelWithLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWSimpleEntryDialog.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWTree.h"
#include "vtkKWTreeWithScrollbars.h"

#include <sstream>

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkSlicerModelHierarchyWidget );
vtkCxxRevisionMacro ( vtkSlicerModelHierarchyWidget, "$Revision: 15783 $");


//---------------------------------------------------------------------------
vtkSlicerModelHierarchyWidget::vtkSlicerModelHierarchyWidget ( )
{
  this->TreeWidget = NULL;
  this->ContextMenu = NULL;
  this->NameDialog = NULL;
  this->ModelDisplaySelectorWidget = NULL;
  this->ModelDisplayWidget = NULL;

  this->ModelDisplayNode = NULL;
  this->ModelHierarchyLogic = NULL;
  this->UpdatingTree = 0;
}

//---------------------------------------------------------------------------
vtkSlicerModelHierarchyWidget::~vtkSlicerModelHierarchyWidget ( )
{
  this->SetModelHierarchyLogic(NULL);

  if (this->TreeWidget)
    {
    this->TreeWidget->SetParent(NULL);
    this->TreeWidget->Delete();
    this->TreeWidget = NULL;
    }

  if (this->ContextMenu)
    {
    this->ContextMenu->SetParent(NULL);
    this->ContextMenu->Delete();
    this->ContextMenu = NULL;
    }

  if (this->NameDialog)
    {
    this->NameDialog->SetParent(NULL);
    this->NameDialog->Delete();
    this->NameDialog = NULL;
    }

  if (this->ModelDisplaySelectorWidget)
    {
    this->ModelDisplaySelectorWidget->SetParent(NULL);
    this->ModelDisplaySelectorWidget->Delete();
    this->ModelDisplaySelectorWidget = NULL;
    }

  if (this->ModelDisplayWidget)
    {
    this->ModelDisplayWidget->SetParent(NULL);
    this->ModelDisplayWidget->Delete();
    this->ModelDisplayWidget = NULL;
    }

  if (this->ModelDisplayNode)
    {
    vtkSetAndObserveMRMLNodeMacro(this->ModelDisplayNode, NULL);
    }

  if (this->ModelHierarchyLogic)
    {
    this->ModelHierarchyLogic->Delete();
    }

  if (this->MRMLScene)
    {
    this->SetMRMLScene(NULL);
    }
}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );

    os << indent << "vtkSlicerModelHierarchyWidget: " << this->GetClassName ( ) << "\n";
    // print widgets?
}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::ProcessWidgetEvents ( vtkObject *caller,
                                                    unsigned long event, 
                                                    void *callData )
{
  vtkKWTree *tree = this->TreeWidget->GetWidget();
  if (caller == tree)
    {
    // Selection changed

    vtksys_stl::string selected(
    this->TreeWidget->GetWidget()->GetSelection());

    //std::cout << "Selected = " << selected << "\n";

    if (event == vtkKWTree::SelectionChangedEvent)
      {
      // For example, one could populate the node inspector
      this->SetSelectesLeaves();
      if (this->SelectedLeaves.size() > 0 )
        {
        // temporay allow to call this again with the selector event
        this->SetInWidgetCallbackFlag(0);
        this->SelectNodeCallback(this->SelectedLeaves[0].c_str());
        this->SetInWidgetCallbackFlag(1);
        }
      }

    // Right click: context menu

    else if (event == vtkKWTree::RightClickOnNodeEvent)
      {
      // This code above should be in something like TriggerContextMenu

      if (!this->ContextMenu)
        {
        this->ContextMenu = vtkKWMenu::New();
        }
      if (!this->ContextMenu->IsCreated())
        {
        this->ContextMenu->SetParent(this);
        this->ContextMenu->Create();
        }
      this->ContextMenu->DeleteAllItems();

      int px, py;
      vtkKWTkUtilities::GetMousePointerCoordinates(tree, &px, &py);

      char command[125];
      
      vtkMRMLNode *node = this->GetMRMLScene()->GetNodeByID((const char *)callData);

      if (!this->IsLeafSelected((const char *)callData))
        {
        this->TreeWidget->GetWidget()->ClearSelection();
        this->TreeWidget->GetWidget()->SelectNode((const char *)callData);
        selected = this->TreeWidget->GetWidget()->GetSelection();
        this->SelectedLeaves.clear();
        this->SelectedLeaves.push_back((const char *)callData);
        }

      if (this->SelectedLeaves.size() > 1)
        {
        // multiple nodes selected
        }
      else if (strcmp(selected.c_str(), "Scene")) 
        {
        // single node selected
        if (node != NULL && node->IsA("vtkMRMLModelHierarchyNode") )
          {
          // hierarchy
          sprintf(command, "InsertHierarchyNodeCallback {%s}", (const char *)callData);
          this->ContextMenu->AddCommand("Insert New Hierarchy Node", this, command);

          sprintf(command, "DeleteNodeCallback {%s}", (const char *)callData);
          this->ContextMenu->AddCommand("Delete", this, command);

          sprintf(command, "RenameNodeCallback {%s}", (const char *)callData);
          this->ContextMenu->AddCommand("Rename", this, command);

          //sprintf(command, "SelectNodeCallback {%s}", (const char *)callData);
          //this->ContextMenu->AddCommand("Edit Display", this, command);
          
          sprintf(command, "SelectReparentCallback {%s}", (const char *)callData);
          this->ContextMenu->AddCommand("Select for Reparenting", this, command);

          if (this->SelectedForReparenting.size() > 0)
            {
            sprintf(command, "ReparentCallback {%s}", (const char *)callData);
            std::stringstream ss;
            vtkMRMLNode *rnode = this->GetMRMLScene()->GetNodeByID(this->SelectedForReparenting[0].c_str());
            if (rnode)
              {
              ss << "Insert " << rnode->GetName();
              this->ContextMenu->AddCommand(ss.str().c_str(), this, command);
              }
            }

          vtkMRMLModelHierarchyNode *hnode = vtkMRMLModelHierarchyNode::SafeDownCast(node);
          vtkMRMLDisplayNode *dnode = hnode->GetDisplayNode();
          if (dnode)
            {
            int visibility = dnode->GetVisibility();
            sprintf(command, "HierarchyVisibilityCallback {%s}", (const char *)callData);
            int tag = this->ContextMenu->AddCheckButton ("Visibility", this, command);
            if (visibility)
              {
              this->ContextMenu->SelectItem(tag);
              }
            sprintf(command, "ColorCallback {%s}", (const char *)callData);
            this->ContextMenu->AddCheckButton ("Color...", this, command);
            }

          this->SetInWidgetCallbackFlag(0);
          this->SelectNodeCallback(this->SelectedLeaves[0].c_str());
          this->SetInWidgetCallbackFlag(1);

          }
        else if (node != NULL && node->IsA("vtkMRMLModelNode"))
          {
          //sprintf(command, "SelectNodeCallback {%s}", (const char *)callData);
          //this->ContextMenu->AddCommand("Edit Display", this, command);
 
          sprintf(command, "SelectReparentCallback {%s}", (const char *)callData);
          this->ContextMenu->AddCommand("Select for Reparenting", this, command);

          vtkMRMLModelNode *mnode = vtkMRMLModelNode::SafeDownCast(node);
          vtkMRMLDisplayNode *dnode = mnode->GetDisplayNode();
          if (dnode)
            {
            int visibility = dnode->GetVisibility();
            sprintf(command, "ModelVisibilityCallback {%s}", (const char *)callData);
            int tag = this->ContextMenu->AddCheckButton ("Visibility", this, command);
            if (visibility)
              {
              this->ContextMenu->SelectItem(tag);
              }

            sprintf(command, "ColorCallback {%s}", (const char *)callData);
            this->ContextMenu->AddCheckButton ("Color...", this, command);
            }
          this->SetInWidgetCallbackFlag(0);
          this->SelectNodeCallback(this->SelectedLeaves[0].c_str());
          this->SetInWidgetCallbackFlag(1);
          }
        }
      else if ( node == NULL )
        {
        // Scene selected
        sprintf(command, "InsertHierarchyNodeCallback {%s}", (const char *)callData);
        this->ContextMenu->AddCommand("Insert New Hierarchy Node", this, command);

        if (this->SelectedForReparenting.size() > 0)
          {
          sprintf(command, "ReparentCallback {%s}", (const char *)callData);
          std::stringstream ss;
          vtkMRMLNode *rnode = this->GetMRMLScene()->GetNodeByID(this->SelectedForReparenting[0].c_str());
          if (rnode)
            {
            ss << "Insert " << rnode->GetName();
            this->ContextMenu->AddCommand(ss.str().c_str(), this, command);
            }
          }
          
        sprintf(command, "AllVisibilityCallback 1");
        this->ContextMenu->AddCommand("Show All", this, command);
        
        sprintf(command, "AllVisibilityCallback 0");
        this->ContextMenu->AddCommand("Show None", this, command);
        
        sprintf(command, "SearchNodeCallback");
        this->ContextMenu->AddCommand("Search...", this, command);

        }
      this->ContextMenu->PopUp(px, py);
      }
      return;
    }
  if (vtkSlicerNodeSelectorWidget::SafeDownCast(caller) == this->ModelDisplaySelectorWidget && 
        event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
    {
    vtkMRMLModelNode *model = 
        vtkMRMLModelNode::SafeDownCast(this->ModelDisplaySelectorWidget->GetSelected());

    vtkMRMLModelHierarchyNode *hmodel = 
        vtkMRMLModelHierarchyNode::SafeDownCast(this->ModelDisplaySelectorWidget->GetSelected());

    if (model != NULL && this->ModelDisplayWidget)
      {
      this->ModelDisplayWidget->SetModelDisplayNode(model->GetModelDisplayNode());
      this->ModelDisplayWidget->SetModelNode(model);
      this->ModelDisplayWidget->SetModelHierarchyNode(NULL);
      }
    else if (hmodel != NULL && this->ModelDisplayWidget)
      {
      this->ModelDisplayWidget->SetModelDisplayNode(hmodel->GetDisplayNode());
      this->ModelDisplayWidget->SetModelNode(NULL);
      this->ModelDisplayWidget->SetModelHierarchyNode(hmodel);
      }
    return;
    }
} 

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::SelectReparentCallback(const char *vtkNotUsed(id))
{
  this->SelectedForReparenting.clear();
  this->SelectedForReparenting = this->SelectedLeaves;

}
//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::ReparentCallback(const char *id)
{
  for (unsigned int i=0; i<this->SelectedForReparenting.size(); i++)
    {
    this->NodeParentChangedCallback(this->SelectedForReparenting[i].c_str(), id, NULL);
    }
  this->SelectedForReparenting.clear();
}


//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::AllVisibilityCallback(int visibility)
{
  std::vector<vtkMRMLNode *> hnodes;
  int nnodes = this->MRMLScene->GetNodesByClass("vtkMRMLModelHierarchyNode", hnodes);
 
  for (unsigned int i=0; i<hnodes.size(); i++)
    {
    vtkMRMLModelHierarchyNode *hnode = vtkMRMLModelHierarchyNode::SafeDownCast(hnodes[i]);
    vtkMRMLDisplayNode *dnode = hnode->GetDisplayNode();
    if (dnode)
      {
      dnode->SetVisibility(visibility);
      }
    
    // chnage children visibility 
    std::vector< vtkMRMLModelHierarchyNode *> childrenNodes;
    this->ModelHierarchyLogic->GetHierarchyChildrenNodes(hnode, childrenNodes);
    for (unsigned int ii=0; ii<childrenNodes.size(); ii++)
      {
      vtkMRMLModelHierarchyNode *cnode = childrenNodes[ii];
      vtkMRMLDisplayNode *cdnode = cnode->GetDisplayNode();
      if (cdnode)
        {
        cdnode->SetVisibility(visibility);
        }
      vtkMRMLModelNode *mnode = cnode->GetModelNode();
      if (mnode)
        {
        int ndnodes = mnode->GetNumberOfDisplayNodes();
        for (int d=0; d<ndnodes; d++)
          {
          dnode = mnode->GetNthDisplayNode(d);
          if (dnode)
            {
            dnode->SetVisibility(visibility);
            }
          }
        }
      } //for
   }
   
  hnodes.clear();
  nnodes = this->MRMLScene->GetNodesByClass("vtkMRMLModelNode", hnodes);
 
  for (unsigned int i=0; i<hnodes.size(); i++)
    {
    vtkMRMLModelNode *hnode = vtkMRMLModelNode::SafeDownCast(hnodes[i]);
    int ndnodes = hnode->GetNumberOfDisplayNodes();
    for (int d=0; d<ndnodes; d++)
      {
      vtkMRMLDisplayNode *dnode = hnode->GetNthDisplayNode(d);
      if (dnode)
        {
        dnode->SetVisibility(visibility);
        }
      }
   }
}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::HierarchyVisibilityCallback(const char *vtkNotUsed(id))
{
  int visibility = 0;
  for (unsigned int i=0; i<this->SelectedLeaves.size(); i++)
    {
    vtkMRMLNode *node = this->GetMRMLScene()->GetNodeByID(this->SelectedLeaves[i].c_str());
    if (node != NULL)
      {
      vtkMRMLModelHierarchyNode *hnode = vtkMRMLModelHierarchyNode::SafeDownCast(node);
      vtkMRMLDisplayNode *dnode = hnode->GetDisplayNode();
      if (dnode)
        {
        visibility = dnode->GetVisibility();
        visibility = (int)(!visibility);
        dnode->SetVisibility(visibility);
        }
      
      // chnage children visibility 
      std::vector< vtkMRMLModelHierarchyNode *> childrenNodes;
      this->ModelHierarchyLogic->GetHierarchyChildrenNodes(hnode, childrenNodes);
      for (unsigned int ii=0; ii<childrenNodes.size(); ii++)
        {
        vtkMRMLModelHierarchyNode *cnode = childrenNodes[ii];
        vtkMRMLDisplayNode *cdnode = cnode->GetDisplayNode();
        if (cdnode)
          {
          cdnode->SetVisibility(visibility);
          }

        vtkMRMLModelNode *mnode = cnode->GetModelNode();
        if (mnode)
          {
          int ndnodes = mnode->GetNumberOfDisplayNodes();
          for (int d=0; d<ndnodes; d++)
            {
            dnode = mnode->GetNthDisplayNode(d);
            if (dnode)
              {
              dnode->SetVisibility(visibility);
              }
            }
          }
        } //for
     } //if
   }
}


//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::ModelVisibilityCallback(const char *vtkNotUsed(id))
{
  for (unsigned int i=0; i<this->SelectedLeaves.size(); i++)
    {
    vtkMRMLNode *node = this->GetMRMLScene()->GetNodeByID(this->SelectedLeaves[i].c_str());
    if (node != NULL)
      {
      vtkMRMLModelNode *mnode = vtkMRMLModelNode::SafeDownCast(node);
      int ndnodes = mnode->GetNumberOfDisplayNodes();
      for (int d=0; d<ndnodes; d++)
        {
        vtkMRMLDisplayNode *dnode = mnode->GetNthDisplayNode(d);
        if (dnode)
          {
          int visibility = dnode->GetVisibility();
          dnode->SetVisibility((int)(!visibility));
          }
        }
      }
    }
}
//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::ColorCallback(const char *vtkNotUsed(id))
{
  vtkMRMLDisplayNode *dnode = NULL;

  if ( this->SelectedLeaves.size() > 0)
    {
    vtkMRMLNode *node = this->GetMRMLScene()->GetNodeByID(this->SelectedLeaves[0].c_str());
    if (node != NULL)
      {
      vtkMRMLModelNode *mnode = vtkMRMLModelNode::SafeDownCast(node);
      vtkMRMLModelHierarchyNode *hnode = vtkMRMLModelHierarchyNode::SafeDownCast(node);
      if (mnode)
        {
        dnode = mnode->GetDisplayNode();
        }
      else if (hnode)
        {
        dnode = hnode->GetDisplayNode();
        }
      if (dnode)
        {
        double *rgb = dnode->GetColor();
        if (vtkKWTkUtilities::QueryUserForColor(
              this->GetApplication(),
              this,
              "Select Color",
              rgb[0], rgb[1], rgb[2],
              rgb, rgb+1, rgb+2) )
          {
          dnode->SetColor(rgb);
          // TODO: need to call Modified, Set does not do it for some reason!!!
          dnode->Modified();
          }
        } // if dnode
      } // if (node 
    } //for



}
//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::DeleteNodeCallback(const char *vtkNotUsed(id))
{
  // cout << "I want to delete MRML node " << id << endl;
  // delete node, then repopulate tree
  for (unsigned int i=0; i<this->SelectedLeaves.size(); i++)
    {
    vtkMRMLNode *node = this->GetMRMLScene()->GetNodeByID(this->SelectedLeaves[i].c_str());
    if (node != NULL)
      {
      this->GetMRMLScene()->RemoveNode(node);
      }
    }
  this->UpdateTreeFromMRML();
}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::RenameNodeCallback(const char *vtkNotUsed(id))
{
  // cout << "I want to delete MRML node " << id << endl;
  // delete node, then repopulate tree
  for (unsigned int i=0; i<this->SelectedLeaves.size(); i++)
    {
    vtkMRMLNode *node = this->GetMRMLScene()->GetNodeByID(this->SelectedLeaves[i].c_str());
    if (node != NULL)
      {
      vtkKWEntryWithLabel *entry = this->NameDialog->GetEntry();
      entry->GetWidget()->SetValue(node->GetName());
      int result = this->NameDialog->Invoke();
      if (result) 
        {
        node->SetName(entry->GetWidget()->GetValue());
        }
      }
    }
  this->UpdateTreeFromMRML();
}


//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::SearchNodeCallback()
{

  vtkKWEntryWithLabel *entry = this->NameDialog->GetEntry();
  if (this->SelectedLeaves.size() > 0)
    {
    entry->GetWidget()->SetValue("");
    }
    
  int result = this->NameDialog->Invoke();
  bool found = false;
  if (result) 
    {
    std::string search = entry->GetWidget()->GetValue();
    std::vector<vtkMRMLNode *> nodes;
    int nnodes = this->MRMLScene->GetNodesByClass("vtkMRMLModelHierarchyNode", nodes);
    for (int i=0; i<nnodes; i++)
      {
      vtkMRMLModelHierarchyNode *node =  vtkMRMLModelHierarchyNode::SafeDownCast(nodes[i]);
      if (node && node->GetName() && std::string(node->GetName()).find(search) != std::string::npos)
        {
        // ignore the extra hierarchy nodes above models
        //if ( node->GetHideFromEditors())
        if ( node->GetModelNodeID() != NULL)
          {
          continue;
          }
        this->SelectedLeaves.clear();
        this->SelectedLeaves.push_back(node->GetID());
        this->SelectNodeCallback(this->SelectedLeaves[0].c_str());
        found = true;
        break;
        }
      } // for
    if (!found)
      {
      nnodes = this->MRMLScene->GetNodesByClass("vtkMRMLModelNode", nodes);
      for (int i=0; i<nnodes; i++)
        {
        vtkMRMLModelNode *node =  vtkMRMLModelNode::SafeDownCast(nodes[i]);
        if (node && node->GetName() && std::string(node->GetName()).find(search) != std::string::npos)
          {
          if ( node->GetHideFromEditors())
            {
            continue;
            }
          this->SelectedLeaves.clear();
          this->SelectedLeaves.push_back(node->GetID());
          this->SelectNodeCallback(this->SelectedLeaves[0].c_str());
          break;
          }
        } // for
      } // if (!found)
    } // if (result)
}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::SelectNodeCallback(const char *vtkNotUsed(id))
{
  for (unsigned int i=0; i<this->SelectedLeaves.size(); i++)
    {
    vtkMRMLNode *node = this->GetMRMLScene()->GetNodeByID(this->SelectedLeaves[i].c_str());
    if (node != NULL)
      {
      this->TreeWidget->GetWidget()->SelectNode(this->SelectedLeaves[i].c_str());
      this->ModelDisplaySelectorWidget->SetSelected(node);
      /**
       if (node->IsA("vtkMRMLModelNode")) 
        {
        this->InvokeEvent(vtkSlicerModelHierarchyWidget::SelectedEvent, node);
        }
        **/
      }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::SelectNode(vtkMRMLNode *node)
{
  if (node != NULL)
    {
    if (node->GetID())
      {
      this->TreeWidget->GetWidget()->SelectNode(node->GetID());
      }
    this->ModelDisplaySelectorWidget->SetSelected(node);
    }
}
//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::InsertHierarchyNodeCallback(const char *id)
{

  vtkMRMLModelHierarchyNode *parentNode = vtkMRMLModelHierarchyNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(id));
  vtkSmartPointer<vtkMRMLModelHierarchyNode> node = 
    vtkSmartPointer<vtkMRMLModelHierarchyNode>::New();

  this->GetMRMLScene()->AddNodeNoNotify(node);
  vtkKWEntryWithLabel *entry = this->NameDialog->GetEntry();
  entry->GetWidget()->SetValue(node->GetName());
  int result = this->NameDialog->Invoke();
  if (!result) 
    {
    this->MRMLScene->RemoveNode(node);
    node = NULL;
    }
  else 
    {
    node->SetName(entry->GetWidget()->GetValue());
    node->SetHideFromEditors(0);
    node->SetSelectable(1);
    if (parentNode != NULL)
      {
      node->SetParentNodeID(parentNode->GetID());
      this->ModelHierarchyLogic->HierarchyIsModified();
      }
    vtkSmartPointer<vtkMRMLModelDisplayNode> dnode = 
      vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
    this->GetMRMLScene()->AddNodeNoNotify(dnode);
    node->SetAndObserveDisplayNodeID(dnode->GetID());
    //this->UpdateTreeFromMRML();
    this->GetMRMLScene()->InvokeEvent(vtkMRMLScene::NodeAddedEvent, node);
    }
}


//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::OpenHierarchyCommand(const char *id)
{
  vtkMRMLModelHierarchyNode *node = vtkMRMLModelHierarchyNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(id));
  if (node != NULL) 
    {
    node->SetExpanded(1);
    /**
    // make children visible
    if (this->UpdatingTree == 0)
      {
      vtkMRMLModelHierarchyNode *hnode = NULL;
      vtkMRMLModelNode *mnode = NULL;
      vtkMRMLDisplayNode *dnode = NULL;
      int nnodes = this->MRMLScene->GetNumberOfNodesByClass("vtkMRMLModelHierarchyNode");

      for (int n=0; n<nnodes; n++)
        {
        hnode = vtkMRMLModelHierarchyNode::SafeDownCast (
              this->MRMLScene->GetNthNodeByClass(n, "vtkMRMLModelHierarchyNode"));
        vtkMRMLModelHierarchyNode *pnode = vtkMRMLModelHierarchyNode::SafeDownCast (hnode->GetParentNode());
        if (pnode == NULL || pnode != node)
          {
          continue;
          }
        
        mnode = hnode->GetModelNode();
        if (mnode)
          {
          dnode = mnode->GetDisplayNode();
          }
        else
          {
          dnode = hnode->GetDisplayNode();
          }
        if (dnode)
          {
          dnode->SetVisibility(1);
          }
        }
      }
      **/
    }
}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::CloseHierarchyCommand(const char *id)
{
  vtkMRMLModelHierarchyNode *node = vtkMRMLModelHierarchyNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(id));
  if (node != NULL) 
  {
    node->SetExpanded(0);
  }
}

//----------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::NodeParentChangedCallback(
  const char *nodeID, const char *parentID, const char*)
{
  if (!strcmp(parentID, nodeID))
    {
    return;
    }
  vtkMRMLModelNode *mnode = vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));
  vtkMRMLModelHierarchyNode *hnode = vtkMRMLModelHierarchyNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));
  if (!strcmp(parentID, "Scene"))
    {
    if (hnode != NULL)
      {
      vtkMRMLModelHierarchyNode *parentNode = vtkMRMLModelHierarchyNode::SafeDownCast(hnode->GetParentNode());
      if (parentNode != NULL)
        {
        hnode->SetParentNodeID(NULL);
        //node->InvokeEvent(vtkMRMLModelNode::HierarchyModifiedEvent);
        }
      }
    else if (mnode != NULL)
      {
      vtkMRMLModelHierarchyNode *parentNode = this->ModelHierarchyLogic->GetModelHierarchyNode ( mnode->GetID());
      if (parentNode != NULL)
        {
        vtkMRMLModelDisplayNode *dnode = parentNode->GetDisplayNode();
        if (dnode)
          {
          this->GetMRMLScene()->RemoveNode(dnode);
          }
        this->GetMRMLScene()->RemoveNode(parentNode);
        //node->InvokeEvent(vtkMRMLModelNode::HierarchyModifiedEvent);
        }
      }
    }
  else 
    {
    // reparent to Hierarchy node
    vtkMRMLModelHierarchyNode *parentNode = vtkMRMLModelHierarchyNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(parentID));
    if (parentNode != NULL)
      {
      if (hnode != NULL)
        {
        hnode->SetParentNodeID(parentNode->GetID());
        //node->InvokeEvent(vtkMRMLModelNode::HierarchyModifiedEvent);
        }
      else if (mnode != NULL)
        {
        vtkMRMLModelHierarchyNode *parentHierNode = this->ModelHierarchyLogic->GetModelHierarchyNode(mnode->GetID());
        if (!parentHierNode)
          {
          parentHierNode = vtkMRMLModelHierarchyNode::New();
          parentHierNode->SetSelectable(0);
          parentHierNode->SetHideFromEditors(1);
          this->GetMRMLScene()->AddNode(parentHierNode);
          }
        vtkMRMLModelHierarchyNode *oldParentNode = vtkMRMLModelHierarchyNode::SafeDownCast(parentHierNode->GetParentNode());
        if (oldParentNode)
          {
          oldParentNode->SetModelNodeID(NULL);
          }           
        parentHierNode->SetModelNodeID(mnode->GetID());
        parentHierNode->SetParentNodeID(parentNode->GetID());
        //node->InvokeEvent(vtkMRMLModelNode::HierarchyModifiedEvent);
        }
      }
    }
  this->ModelHierarchyLogic->HierarchyIsModified();
  this->UpdateTreeFromMRML();
}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::ProcessMRMLEvents(vtkObject *vtkNotUsed(caller),
                                                      unsigned long event, 
                                                      void *callData )
{
  if (this->MRMLScene == NULL || this->MRMLScene->GetIsClosed())
    {
    return;
    }

  if (event == vtkMRMLScene::SceneLoadEndEvent)
    {
    this->UpdateTreeFromMRML();
    }

  if (((event == vtkMRMLScene::NodeAddedEvent || event == vtkMRMLScene::NodeRemovedEvent) && 
      (reinterpret_cast<vtkMRMLModelNode *>(callData) || reinterpret_cast<vtkMRMLModelHierarchyNode *>(callData))) ||
      event == vtkMRMLScene::NewSceneEvent || event == vtkMRMLScene::SceneCloseEvent)
    {
    this->UpdateTreeFromMRML();
    }
}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::RemoveWidgetObservers ( ) 
{
  if (this->TreeWidget) 
    {
    this->TreeWidget->GetWidget()->RemoveObservers(
      vtkKWTree::SelectionChangedEvent, 
      (vtkCommand *)this->GUICallbackCommand);  
 
    this->TreeWidget->GetWidget()->RemoveObservers(
      vtkKWTree::RightClickOnNodeEvent, 
      (vtkCommand *)this->GUICallbackCommand);  
    }
  if (this->ModelDisplaySelectorWidget)
    {
    this->ModelDisplaySelectorWidget->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );
    }

}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::CreateWidget ( )
{
  // Check if already created

  if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }

  // Call the superclass to create the whole widget

  this->Superclass::CreateWidget();

  // MRML Tree

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New ( );
  frame->SetParent ( this->GetParent() );
  frame->Create ( );
  frame->SetLabelText ("Model Hierarchy");
  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                 frame->GetWidgetName() );

  this->TreeWidget = vtkKWTreeWithScrollbars::New() ;
  this->TreeWidget->SetParent ( frame->GetFrame() );
  this->TreeWidget->VerticalScrollbarVisibilityOn();
  this->TreeWidget->HorizontalScrollbarVisibilityOff();
  //this->TreeWidget->ResizeButtonsVisibilityOn();
  
  this->TreeWidget->Create ( );
  //this->TreeWidget->SetBalloonHelpString("MRML Tree");
  //this->TreeWidget->SetBorderWidth(2);
  //this->TreeWidget->SetReliefToGroove();
  this->Script ( "pack %s -side top -anchor nw -expand y -fill both -padx 2 -pady 2",
                 this->TreeWidget->GetWidgetName());

  vtkKWTree *tree = this->TreeWidget->GetWidget();
  tree->SelectionFillOn();
  //tree->SetSelectionModeToMultiple ();
  tree->SetSelectionModeToSingle();
  tree->SetNodeParentChangedCommand(this, "NodeParentChangedCallback");
  tree->SetOpenCommand(this, "OpenHierarchyCommand");
  tree->SetCloseCommand(this, "CloseHierarchyCommand");
  tree->EnableReparentingOn();

  tree->AddObserver(
    vtkKWTree::SelectionChangedEvent, 
    (vtkCommand *)this->GUICallbackCommand );

  tree->AddObserver(
    vtkKWTree::RightClickOnNodeEvent, 
    (vtkCommand *)this->GUICallbackCommand);
  vtkKWFrameWithLabel *dframe = vtkKWFrameWithLabel::New ( );
  dframe->SetParent ( frame );
  dframe->Create ( );
  dframe->SetLabelText ("Model Display");
 
  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                 dframe->GetWidgetName() );

  this->ModelDisplaySelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
  this->ModelDisplaySelectorWidget->SetParent ( dframe->GetFrame() );
  this->ModelDisplaySelectorWidget->Create ( );
  this->ModelDisplaySelectorWidget->SetNodeClass("vtkMRMLModelHierarchyNode", NULL, NULL, NULL);
  this->ModelDisplaySelectorWidget->AddNodeClass("vtkMRMLModelNode", NULL, NULL, NULL);
  this->ModelDisplaySelectorWidget->SetChildClassesEnabled(0);
  this->ModelDisplaySelectorWidget->SetShowHidden(1);
  this->ModelDisplaySelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->ModelDisplaySelectorWidget->SetBorderWidth(2);
  // this->ModelDisplaySelectorWidget->SetReliefToGroove();
  this->ModelDisplaySelectorWidget->SetPadX(2);
  this->ModelDisplaySelectorWidget->SetPadY(2);
  this->ModelDisplaySelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->ModelDisplaySelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
  this->ModelDisplaySelectorWidget->SetLabelText( "Set Model: ");
  this->ModelDisplaySelectorWidget->SetBalloonHelpString("select a model or a hierarchy.");
  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                 this->ModelDisplaySelectorWidget->GetWidgetName());


  this->ModelDisplayWidget = vtkSlicerModelDisplayWidget::New ( );
  this->ModelDisplayWidget->SetAndObserveMRMLScene(this->GetMRMLScene() );
  this->ModelDisplayWidget->SetModelHierarchyLogic(this->GetModelHierarchyLogic());

  this->ModelDisplayWidget->SetParent ( dframe->GetFrame() );
  this->ModelDisplayWidget->Create ( );
  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                this->ModelDisplayWidget->GetWidgetName(),
                dframe->GetFrame()->GetWidgetName());

  vtkIntArray *events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::NewSceneEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  events->InsertNextValue(vtkMRMLScene::SceneLoadEndEvent);
  this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
  events->Delete();

  this->NameDialog  = vtkKWSimpleEntryDialog::New();
  this->NameDialog->SetParent ( this->GetParent());
  this->NameDialog->SetTitle("Model Hierarchy Name");
  this->NameDialog->SetSize(400, 200);
  this->NameDialog->SetStyleToOkCancel();
  vtkKWEntryWithLabel *entry = this->NameDialog->GetEntry();
  entry->SetLabelText("Hierarchy Name");
  entry->GetWidget()->SetValue("");
  this->NameDialog->Create ( );

  this->UpdateTreeFromMRML();

  this->ModelDisplaySelectorWidget->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );
  
  this->ProcessWidgetEvents (this->ModelDisplaySelectorWidget,
                             vtkSlicerNodeSelectorWidget::NodeSelectedEvent, NULL );

  frame->Delete();
  dframe->Delete();

}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::UpdateTreeFromMRML()
  
{
  this->UpdatingTree = 1;
  this->ModelHierarchyLogic->SetMRMLScene(this->MRMLScene);

  vtksys_stl::string selected_node(
    this->TreeWidget->GetWidget()->GetSelection());
  this->TreeWidget->GetWidget()->DeleteAllNodes();

  vtkMRMLScene *scene = this->GetMRMLScene();
  vtkMRMLNode *node = NULL;

  // create Root node
  this->TreeWidget->GetWidget()->AddNode(NULL, "Scene", "Scene");
  this->TreeWidget->GetWidget()->OpenNode("Scene");

  int nnodes = scene->GetNumberOfNodes();
  for (int n=0; n<nnodes; n++)
    {
    node = scene->GetNthNode(n);
    this->AddNodeToTree(node);
    }

  // check that the selected node is still in the tree
  if (this->TreeWidget->GetWidget()->HasNode(selected_node.c_str()))
    {
    this->TreeWidget->GetWidget()->SelectNode(selected_node.c_str());
    }
  else
    {
    if (selected_node != "")
      {
      vtkWarningMacro("Selected node no longer in tree: " << selected_node.c_str());
      }
    }
  // At this point you probably want to reset the MRML node inspector fields
  // in case nothing in the tree is selected anymore (here, we delete all nodes
  // each time, so nothing will be selected, but it's not a bad thing to 
  // try to save the old selection, or just update the tree in a smarter
  // way).

  if ( selected_node == vtksys_stl::string("Scene"))
    {
    this->TreeWidget->GetWidget()->OpenNode("Scene");
    //this->TreeWidget->GetWidget()->OpenFirstNode ();
    }
  this->UpdatingTree = 0;

}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::AddNodeToTree(vtkMRMLNode *node)
{
  if (!node)
    {
    return;
    }

  if ((!node->IsA("vtkMRMLModelNode") && !node->IsA("vtkMRMLModelHierarchyNode")) || 
      ((node->IsA("vtkMRMLModelNode") && node->GetHideFromEditors())))
   {
    return;
   }

  vtkKWTree *tree = this->TreeWidget->GetWidget();

  if (tree->HasNode(node->GetID()))
    {
    return;
    }

  char *ID = node->GetID();
    
  vtksys_stl::string node_text(node->GetName());
  if (node_text.size())
    {
    /**
    node_text += " (";
    node_text += ID;
    node_text += ")";
    **/
    }
  else
    {
    node_text = "(";
    node_text += ID;
    node_text += ")";
    }
  /*
  node_text += ": ";
  node_text += node->GetClassName();
  */
  
  const char *parent_node = "Scene";

  int open = 0;

  vtkMRMLModelHierarchyNode* parentNode = NULL;
  vtkMRMLModelHierarchyNode *mhnode = vtkMRMLModelHierarchyNode::SafeDownCast(node);

  if ( node->IsA("vtkMRMLModelNode"))
    {
    parentNode = this->ModelHierarchyLogic->GetModelHierarchyNode(node->GetID());
    if (parentNode)
      {
      parentNode = vtkMRMLModelHierarchyNode::SafeDownCast(parentNode->GetParentNode());
      }
    }
  else if ( node->IsA("vtkMRMLModelHierarchyNode") )
    {
    parentNode = vtkMRMLModelHierarchyNode::SafeDownCast(mhnode->GetParentNode());
    open = mhnode->GetExpanded();
    }

  if (parentNode)
    {
    if (!strcmp(parentNode->GetID(), node->GetID()))
      {
      return;
      }
    parent_node = parentNode->GetID();
    this->AddNodeToTree(parentNode);
    }
  if (!mhnode || mhnode->GetModelNode() == NULL)
    {
    tree->AddNode(parent_node, ID, node_text.c_str());
    /**
    // add checkbox
    vtkKWCheckButton *cbox = vtkKWCheckButton::New();
    cbox->SetText(node_text.c_str());
    cbox->SetParent ( tree );
    cbox->Create ( );
    tree->SetNodeWindow(ID, cbox);
    this->Script("bind %s <ButtonPress-3> {%s ProcessRightClick %s}",  cbox->GetWidgetName(), this->GetTclName(), ID);

    //cbox->Delete();
    **/
    if (open)
      {
      tree->OpenNode(ID);
      }
    else
      {
      tree->CloseNode(ID);
      }
    }
}

void vtkSlicerModelHierarchyWidget::ProcessRightClick(const char *id)
{
std::cout << id << std::endl;
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkSlicerModelHierarchyWidget::GetSelectedNodeInTree()
{
  vtksys_stl::string selected_node(
    this->TreeWidget->GetWidget()->GetSelection());

  vtkMRMLScene *scene = this->GetMRMLScene();
  vtkMRMLNode *node = NULL, *first_selected_node = NULL;

  int nnodes = scene->GetNumberOfNodes();
  for (int n=0; n<nnodes; n++)
    {
    node = scene->GetNthNode(n);
    if (!strcmp(node->GetID(), selected_node.c_str()))
      {
      first_selected_node = node;
      break;
      }
    }
  return first_selected_node;
}

//---------------------------------------------------------------------------
void vtkSlicerModelHierarchyWidget::SetSelectesLeaves()
{
  this->SelectedLeaves.clear();

  vtksys_stl::string selectedLeaves(
    this->TreeWidget->GetWidget()->GetSelection());

  vtksys_stl::string::size_type locStart = 0;
  vtksys_stl::string::size_type locEnd = 0;
  do 
    {
    locEnd = selectedLeaves.find( " ", locStart );
    if (locEnd != vtksys_stl::string::npos)
      {
      vtksys_stl::string selectedLeaf = selectedLeaves.substr(locStart, locEnd-locStart);
      this->SelectedLeaves.push_back(selectedLeaf);
      locStart = locEnd+1;
      }
    }
  while (locEnd != vtksys_stl::string::npos) ;
  vtksys_stl::string selectedLeaf = selectedLeaves.substr(locStart);
  this->SelectedLeaves.push_back(selectedLeaf);
}

//---------------------------------------------------------------------------
int vtkSlicerModelHierarchyWidget::IsLeafSelected(const char *leaf)
{
  vtksys_stl::string sleaf(leaf);

  for (unsigned int i=0; i<this->SelectedLeaves.size(); i++)
    {
    if (this->SelectedLeaves[i].compare(sleaf) == 0)
      {
      return 1;
      }
    }
  return 0;
}
