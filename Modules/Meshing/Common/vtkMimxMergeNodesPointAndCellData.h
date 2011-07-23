/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxMergeNodesPointAndCellData.h,v $
Language:  C++
Date:      $Date: 2008/03/31 13:12:19 $
Version:   $Revision: 1.4 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) The University of Iowa. All rights reserved.
See MIMXCopyright.txt or http://www.ccad.uiowa.edu/mimx/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// To merge all the data sets and the field data associated with the same.
// The input is a vtkDataSeCollection and the output will be an 
// unstructuredgrid.
// All the data sets should have a point data array "Node_Numbers" and
// a cell data array "Element_Numbers". The rest of the arrays will be
// named Element_Set_(number 1,2,..) associated with the cell data and 
// Node_Set_(number 1,2,..) associated with point data.
// The rules that will be followed for merging is as follows
// 1)   All the data Node_Numbers and Element_Numbers are merged.
// 2)   As far as Element_Set_ and Node_Set_ data is concerned
// All the data sets need not have the all the Node_Set_ and 
// Element_Set_ data. For eg, If a Node_Set_ or Element_Set_ is not 
// present in data set 1 and present in data set 2, the a new 
// Node_Set_ or Element_Set_ is created and attached to data set 1.

#ifndef __vtkMimxMergeNodesPointAndCellData_h
#define __vtkMimxMergeNodesPointAndCellData_h

#include "vtkMimxCommonWin32Header.h"

class vtkDataSetCollection;
class vtkIntArray;
class vtkUnstructuredGrid;

class VTK_MIMXCOMMON_EXPORT vtkMimxMergeNodesPointAndCellData
{
public:
        vtkMimxMergeNodesPointAndCellData();
        ~vtkMimxMergeNodesPointAndCellData();
        void SetDataSetCollection(vtkDataSetCollection*);
        int MergeDataSets();
        vtkUnstructuredGrid *GetMergedDataSet();
        int CheckNodeAndElementNumbersPresent();
        void SetNodesMerge(int status) {this->State = status; }
        void SetTolerance(double Tolerance) { this->Tol = Tolerance;}
        
private:
        vtkUnstructuredGrid *MergedDataSet;
        vtkDataSetCollection *DataSetCollection;
        int IsArrayPresent(int DataSetNum, const char *ArrayName);
        int IsNodeArrayPresent(int DataSetNum, const char *ArrayName);
        int State;
        double Tol;
};

#endif
