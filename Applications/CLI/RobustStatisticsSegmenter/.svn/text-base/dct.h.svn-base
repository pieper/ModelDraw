/* ================================================================================

   DCT 1D


   20110428 Yi Gao

   ================================================== */

#ifndef dct_h_
#define dct_h_


#include "vnl/vnl_vector.h"

namespace douher
{
  /**
   * T should be float or double
   */
  template< typename T > 
  void dct1(const T* data, std::size_t num_data, T* data_dct);
  
  template< typename T > 
  void dct1(const vnl_vector<T>& data, vnl_vector<T>& data_fft);



  template< typename T > 
  void idct1(const T* data, std::size_t num_data, T* data_idct);
  
  template< typename T > 
  void idct1(const vnl_vector<T>& data, vnl_vector<T>& data_fft);



} // end namespace 

#include "dct.txx"

#endif
