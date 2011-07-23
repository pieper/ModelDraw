/*=========================================================================

  Program:   SparseFieldLevelSetContour
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/SparseFieldLevelSetContour/MeanCurvatureEnergy.h $
  Language:  C++
  Date:      $Date: 2010-07-29 19:10:30 -0400 (Thu, 29 Jul 2010) $
  Version:   $Revision: 14395 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef MEANCURVATUREENERGY
#define MEANCURVATUREENERGY

#include "MeshEnergy.h"

class MeanCurvatureEnergy : public MeshEnergy
{
public:
MeanCurvatureEnergy( MeshData* data ) {meshdata = data;}
virtual ~MeanCurvatureEnergy( ) {}

double eval_energy( const std::vector<int>& C );
std::valarray<double> getforce( const std::list<int>& C);
std::valarray<double> getforce( const std::list<int>& C,
const std::list<int>& L_p1, const std::list<int>& L_n1,
const std::vector<double>& phi);

};



#endif
