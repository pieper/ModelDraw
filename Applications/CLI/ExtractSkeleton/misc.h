/*=========================================================================

  Program:   Extract Skeleton
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/ExtractSkeleton/misc.h $
  Language:  C++
  Date:      $Date: 2008-11-25 15:18:08 -0500 (Tue, 25 Nov 2008) $
  Version:   $Revision: 7978 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
/*
 * misc.h
 * 
 * author:  msturm
 * created: 27 Mar 1997
 * changes: mastyner
 */

#ifndef __IP_MISC_H__
#define __IP_MISC_H__

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>


typedef enum { 
  IP_BYTE = 0,     /* AVS_TYPE_BYTE    = 0 */
  IP_INT,          /* AVS_TYPE_INTEGER = 1 */
  IP_FLOAT,        /* AVS_TYPE_REAL    = 2 */
  IP_DOUBLE,       /* AVS_TYPE_DOUBLE  = 3 */
  IP_SHORT         /* AVS_TYPE_SHORT   = 4 */ 
} ipDataType;


typedef union  {
  void           *_void;
  unsigned char  *_byte;
  short          *_short;
  int            *_int;
  float          *_float;
  double         *_double;
} ipDataUnion;

// memory allocation & handling
size_t ipGetDataSize(const ipDataType type);
void* ipAllocateData(const int size, const size_t elemsize);



// misc functions 

template <class T>
inline void ipSwap(T *a, T *b) {
  T temp = *a; *a = *b; *b = temp;
}

template <class T>
inline T sqr(T x){
  return x * x;
}

// thresholding operators
template <class T>
inline void ipUpperThreshold(T *data, const int size, const T threshold) {
  T *dp = data;
  for (int i = 0; i < size; i++, dp++)
    if (*dp < threshold) *dp = (T) 0.0;
}

template <class T>
inline void ipLowerThreshold(T *data, const int size, const T threshold) {
  T *dp = data;
  for (int i = 0; i < size; i++, dp++)
    if (*dp > threshold) *dp = (T) 0.0;
}

template <class T>
inline void ipUpperBinaryThreshold(T *data, const int size, const T threshold) {
  T *dp = data;
  for (int i = 0; i < size; i++, dp++)
    *dp = (*dp < threshold ? (T) 0.0: (T) 1.0);
}

template <class T>
inline void ipLowerBinaryThreshold(T *data, const int size, const T threshold) {
  T *dp = data;
  for (int i = 0; i < size; i++, dp++)
    *dp = (*dp > threshold ? (T) 0.0: (T) 1.0);
}


#endif
