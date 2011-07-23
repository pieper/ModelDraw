/* ================================================================================

   Kernel density estimation via diffusion, 1D


   20110428 Yi Gao

   ================================================== */

#ifndef kde_via_diffusion_h_
#define kde_via_diffusion_h_

#include <cstdlib>


namespace douher
{
  /**
   * KDE via diffusion. T should be float or double
   */
  template<typename T>
  void kde_via_diffusion(const double* data, std::size_t numData, double xMin, double xMax, std::size_t numPDFSamples, \
                         double* pdfx, double* pdf);

  template<typename Tnull>
  double kde_via_diffusion_foo(double t, std::size_t num_data, long* I, double* a2, std::size_t num_pdf_samples);
}

#include "kde_via_diffusion.txx"

#endif
