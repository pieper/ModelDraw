/*=========================================================================

  Program:   Extract Skeleton
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/ExtractSkeleton/misc.cxx $
  Language:  C++
  Date:      $Date: 2008-11-25 15:18:08 -0500 (Tue, 25 Nov 2008) $
  Version:   $Revision: 7978 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
/*
 * misc.cc -- miscellaneous utitlity functions
 * 
 * author:  msturm 
 * created: 27 Mar 1997
 * changes: mastyner
 */


#include "misc.h"
#include "string.h"

void* ipAllocateData(const int size, const size_t elemsize)
{
  void *data = NULL;

  if (!(data = malloc(size * elemsize))) {
    fprintf(stderr,"Error: ipAllocateData [%s, line %d]: memory allocation failed:",
            __FILE__, __LINE__);
    perror("");
    exit(errno);
  }
  
  memset(data, 0, size * elemsize);

  return(data);
}


size_t ipGetDataSize(const ipDataType type)
{
  size_t retval;

  switch (type) {
  case IP_BYTE:
    retval = sizeof(char);
    break;
  case IP_SHORT:
    retval = sizeof(short);
    break;
  case IP_INT:
    retval = sizeof(int);
    break;
  case IP_FLOAT:
    retval = sizeof(float);
    break;
  case IP_DOUBLE:
    retval = sizeof(double);
    break;
  default:
#ifdef DEBUG_VSKEL
    fprintf(stderr,"Warning: ipGetDataSize [%s, line %d]: unsuported data type: %d\n",
            __FILE__, __LINE__, type);
#endif

    retval = 0;
    break;
  }

  return retval;
  
}
