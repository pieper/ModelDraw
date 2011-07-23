/* ================================================================================
   histogram

   20110428 Yi Gao

   ================================================== */

#ifndef histc_txx_
#define histc_txx_

#include <cstdlib>

namespace douher
{
  template< typename T >
  void histc(const T* data, std::size_t N,  \
             const T* edges, std::size_t M, \
             long* histogram)
  {
    //long* n = new long[M];
    for (std::size_t im = 0; im < M; ++im)
      histogram[im] = 0;


    for (std::size_t im = 0; im < M-1; ++im)
      {
        for (std::size_t in = 0; in < N; ++in)
          {
            T d = data[in];

            if (edges[im] <= d && d < edges[im+1])
              {
                ++histogram[im];
              }
          }
      }

    // the last bin
    for (std::size_t in = 0; in < N; ++in)
      {
        if (edges[M-1] == data[in])
          {
            ++histogram[M-1];
          }
      }
    

    return;
  }



} // end namespace 


#endif
