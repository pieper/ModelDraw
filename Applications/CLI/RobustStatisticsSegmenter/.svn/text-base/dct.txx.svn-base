/* ================================================================================

   DCT 1D


   20110428 Yi Gao

   ================================================== */

#ifndef dct_txx_
#define dct_txx_


#include "vnl/vnl_math.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_fft_1d.h"

#include <complex>


namespace douher
{
  template< typename T > 
  void dct1(const T* data, std::size_t num_data, T* data_dct)
  {
    typedef std::complex<T> complex_t;

    typedef vnl_vector<T> vnl_vector_t;


    // copy input array to vnl vector
    vnl_vector_t input_data(num_data);
    for (std::size_t i = 0; i < num_data; ++i)
      {
        input_data[i] = data[i];
      }


    // run DCT
    vnl_vector_t dct_data;
    dct1<T>(input_data, dct_data);


    // copy output to the array
    for (std::size_t i = 0; i < num_data; ++i)
      {
        data_dct[i] = dct_data[i];
      }

    return;
  }


  template< typename T > 
  void dct1(const vnl_vector<T>& data, vnl_vector<T>& data_cdt)
  {
    typedef std::complex<T> complex_t;

    typedef vnl_vector<complex_t> vnl_vector_t;

    std::size_t num_data = data.size();


    // compute weights
    complex_t* weights = new complex_t[num_data];
    complex_t i_unit(0, 1);

    weights[0] = 1.0;
    for (std::size_t i = 1; i < num_data; ++i)
      {
        double tmp = -i*(vnl_math::pi)/2.0/num_data;
        complex_t tmp1 = i_unit*tmp;
        weights[i] = 2.0*exp(tmp1);
      }


    vnl_vector_t data_fft_complex(num_data);

    // copy input array to vnl complex vector, and Re-order the
    // elements of the columns of x
    {
      std::size_t j = 0;
      for (std::size_t i = 0; i <= num_data - 1; i += 2)
        {
          data_fft_complex[j++] = data[i];
        }

      for (long i = num_data - 1; i >= 1; i -= 2)
        {
          data_fft_complex[j++] = data[i];
        }
    }


    // do fft
    typedef vnl_fft_1d<T> vnl_fft_1d_t;
    vnl_fft_1d_t ffter(num_data);

    ffter.fwd_transform(data_fft_complex);


    // output
    data_cdt.set_size(num_data);
    for (std::size_t i = 0; i < num_data; ++i)
      {
        data_cdt[i] = (weights[i]*data_fft_complex[i]).real();
      }
    

    delete[] weights;

    return;
  }

  


  /**
   * Inverse DCT
   */
  template< typename T > 
  void idct1(const T* data, std::size_t num_data, T* data_idct)
  {
    typedef std::complex<T> complex_t;

    typedef vnl_vector<T> vnl_vector_t;


    // copy input array to vnl vector
    vnl_vector_t input_data(num_data);
    for (std::size_t i = 0; i < num_data; ++i)
      {
        input_data[i] = data[i];
      }


    // run IDCT
    vnl_vector_t idct_data;
    idct1<T>(input_data, idct_data);


    // copy output to the array
    for (std::size_t i = 0; i < num_data; ++i)
      {
        data_idct[i] = idct_data[i];
      }

    return;
  }


  template< typename T > 
  void idct1(const vnl_vector<T>& data, vnl_vector<T>& data_idct)
  {
    typedef std::complex<T> complex_t;

    typedef vnl_vector<complex_t> vnl_vector_t;

    std::size_t num_data = data.size();


    // compute weights
    complex_t* weights = new complex_t[num_data];
    complex_t i_unit(0, 1);

    weights[0] = 1.0;
    for (std::size_t i = 0; i < num_data; ++i)
      {
        double tmp = i*(vnl_math::pi)/2.0/num_data;
        complex_t tmp1 = i_unit*tmp;
        
        weights[i] = exp(tmp1);
      }

    vnl_vector_t data_fft_complex(num_data);
    for (std::size_t i = 0; i < num_data; ++i)
      {
        data_fft_complex[i] = weights[i]*data[i];
      }

    delete[] weights;


    // do fft
    typedef vnl_fft_1d<T> vnl_fft_1d_t;
    vnl_fft_1d_t ffter(num_data);

    ffter.bwd_transform(data_fft_complex);


    data_idct.set_size(num_data);
    {
      std::size_t j = 0;
      for (std::size_t i = 0; i < num_data; i+=2)
        {
          data_idct[i] = data_fft_complex[j++].real();
        }

      j = num_data - 1;
      for (std::size_t i = 1; i < num_data; i+=2)
        {
          data_idct[i] = data_fft_complex[j--].real();
        }

    }


    return;
  }



} // end namespace 


#endif
