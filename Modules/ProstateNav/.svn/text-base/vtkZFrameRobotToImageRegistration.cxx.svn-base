/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkRobotRegistration.cxx $
  Date:      $Date: 2009-01-05 13:28:20 -0500 (Mon, 05 Jan 2009) $
  Version:   $Revision: 8267 $

==========================================================================*/

/*
 * ZFrame Fiducial Registration Algorithm
 * The method and code is originally developoed by Simon DiMaio from BWH.
 */

#include "vtkObjectFactory.h"

#include "vtkZFrameRobotToImageRegistration.h"

#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLLinearTransformNode.h"

#include "vtkMRMLVolumeNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLVolumeHeaderlessStorageNode.h"
#include "vtkMRMLVolumeArchetypeStorageNode.h"

#include <string.h>

// Test
#include <fstream>

#include "ZFrameCalibration.h"

#define MEPSILON        (1e-10)

vtkStandardNewMacro(vtkZFrameRobotToImageRegistration);
vtkCxxRevisionMacro(vtkZFrameRobotToImageRegistration, "$Revision: 8267 $");


//#define DEBUG_ZFRAME_REGISTRATION 1

//---------------------------------------------------------------------------
vtkZFrameRobotToImageRegistration::vtkZFrameRobotToImageRegistration()
{
  this->SliceRangeLow = -1;
  this->SliceRangeHigh = -1;
  this->ZFrameBaseOrientation = vtkMatrix4x4::New();
  this->ZFrameBaseOrientation->Identity();
}


//---------------------------------------------------------------------------
vtkZFrameRobotToImageRegistration::~vtkZFrameRobotToImageRegistration()
{
  this->ZFrameBaseOrientation->Delete();
  this->ZFrameBaseOrientation = NULL;
}


//---------------------------------------------------------------------------
void vtkZFrameRobotToImageRegistration::PrintSelf(ostream& os, vtkIndent indent)
{
}


//---------------------------------------------------------------------------
void vtkZFrameRobotToImageRegistration::SetSliceRange(int param1, int param2)
{
  this->SliceRangeLow = param1;
  this->SliceRangeHigh = param2;
}


//---------------------------------------------------------------------------
int vtkZFrameRobotToImageRegistration::DoRegistration()
{
  
  if (this->FiducialVolume && this->RobotToImageTransform)
    {
    //simond debug frame delay
    // Get the image size attributes from the event.
    
    vtkImageData* image = this->FiducialVolume->GetImageData();
    int dimensions[3];
    image->GetDimensions(dimensions);
    
    // Get image position and orientation
    vtkMatrix4x4* rtimgTransform = vtkMatrix4x4::New(); 
    this->FiducialVolume->GetIJKToRASMatrix(rtimgTransform);

    // Convert image positiona and orientation to zf::Matrix4x4
    zf::Matrix4x4 imageTransform;
    imageTransform[0][0] = rtimgTransform->GetElement(0, 0);
    imageTransform[1][0] = rtimgTransform->GetElement(1, 0);
    imageTransform[2][0] = rtimgTransform->GetElement(2, 0);
    imageTransform[0][1] = rtimgTransform->GetElement(0, 1);
    imageTransform[1][1] = rtimgTransform->GetElement(1, 1);
    imageTransform[2][1] = rtimgTransform->GetElement(2, 1);
    imageTransform[0][2] = rtimgTransform->GetElement(0, 2);
    imageTransform[1][2] = rtimgTransform->GetElement(1, 2);
    imageTransform[2][2] = rtimgTransform->GetElement(2, 2);
    imageTransform[0][3] = rtimgTransform->GetElement(0, 3);
    imageTransform[1][3] = rtimgTransform->GetElement(1, 3);
    imageTransform[2][3] = rtimgTransform->GetElement(2, 3);
    
    // ZFrame base orientation
    zf::Matrix4x4 ZmatrixBase;
    ZmatrixBase[0][0] = (float) this->ZFrameBaseOrientation->GetElement(0, 0);
    ZmatrixBase[1][0] = (float) this->ZFrameBaseOrientation->GetElement(1, 0);
    ZmatrixBase[2][0] = (float) this->ZFrameBaseOrientation->GetElement(2, 0);
    ZmatrixBase[0][1] = (float) this->ZFrameBaseOrientation->GetElement(0, 1);
    ZmatrixBase[1][1] = (float) this->ZFrameBaseOrientation->GetElement(1, 1);
    ZmatrixBase[2][1] = (float) this->ZFrameBaseOrientation->GetElement(2, 1);
    ZmatrixBase[0][2] = (float) this->ZFrameBaseOrientation->GetElement(0, 2);
    ZmatrixBase[1][2] = (float) this->ZFrameBaseOrientation->GetElement(1, 2);
    ZmatrixBase[2][2] = (float) this->ZFrameBaseOrientation->GetElement(2, 2);
    ZmatrixBase[0][3] = (float) this->ZFrameBaseOrientation->GetElement(0, 3);
    ZmatrixBase[1][3] = (float) this->ZFrameBaseOrientation->GetElement(1, 3);
    ZmatrixBase[2][3] = (float) this->ZFrameBaseOrientation->GetElement(2, 3);

    // Convert Base Matrix to quaternion
    float ZquaternionBase[4];
    zf::MatrixToQuaternion(ZmatrixBase, ZquaternionBase);

    // Set slice range
    int range[2];
    range[0] = this->SliceRangeLow;
    range[1] = this->SliceRangeHigh;

    // Get a pointer to the image array.
    image->SetScalarTypeToShort();
    image->Modified();
    short* inputImage = (short*)image->GetScalarPointer();

    float Zposition[3];
    float Zorientation[4];

    // Call Z-frame registration
    zf::ZFrameCalibration * calibration;
    calibration = new zf::ZFrameCalibration();
    calibration->SetInputImage(inputImage, dimensions, imageTransform);
    calibration->SetOrientationBase(ZquaternionBase);
    int r = calibration->Register(range, Zposition, Zorientation);

    delete calibration;

    if (r)
      {
      // Convert quaternion to matrix
      zf::Matrix4x4 matrix;
      zf::QuaternionToMatrix(Zorientation, matrix);
      matrix[0][3] = Zposition[0];
      matrix[1][3] = Zposition[1];
      matrix[2][3] = Zposition[2];
      
#ifdef DEBUG_ZFRAME_REGISTRATION
      std::cerr << "Result matrix:" << std::endl;
      zf::PrintMatrix(matrix);
#endif // DEBUG_ZFRAME_REGISTRATION
      
      //// For evaluation
      //std::ofstream fout;
      //fout.open("zframe_output.csv", std::ios::out | std::ios::app);
      //
      //fout << volumeNode->GetName() << ", "
      //     << n << ", "
      //     << matrix[0][0] << ", " << matrix[0][1] << ", " << matrix[0][2] << ", " << matrix[0][3] << ", "
      //     << matrix[1][0] << ", " << matrix[1][1] << ", " << matrix[1][2] << ", " << matrix[1][3] << ", "
      //     << matrix[2][0] << ", " << matrix[2][1] << ", " << matrix[2][2] << ", " << matrix[2][3]
      //     << std::endl;
      //fout.close();
      
      vtkMatrix4x4* zMatrix = vtkMatrix4x4::New();
      zMatrix->Identity();
      
      zMatrix->SetElement(0, 0, matrix[0][0]);
      zMatrix->SetElement(1, 0, matrix[1][0]);
      zMatrix->SetElement(2, 0, matrix[2][0]);
      zMatrix->SetElement(0, 1, matrix[0][1]);
      zMatrix->SetElement(1, 1, matrix[1][1]);
      zMatrix->SetElement(2, 1, matrix[2][1]);
      zMatrix->SetElement(0, 2, matrix[0][2]);
      zMatrix->SetElement(1, 2, matrix[1][2]);
      zMatrix->SetElement(2, 2, matrix[2][2]);
      zMatrix->SetElement(0, 3, matrix[0][3]);
      zMatrix->SetElement(1, 3, matrix[1][3]);
      zMatrix->SetElement(2, 3, matrix[2][3]);
      
      if (this->RobotToImageTransform != NULL)
        {
        vtkMatrix4x4* transformToParent = this->RobotToImageTransform->GetMatrixTransformToParent();
        transformToParent->DeepCopy(zMatrix);
        zMatrix->Delete();
        this->RobotToImageTransform->Modified();
        return 1;
        }
      else
        {
        zMatrix->Delete();
        return 0;
        }
      }
    else
      {
      return 0;
      }
    }
  else
    {
    return 0;
    }

}



