/* ================================================================================

   Gaussian kernel density estimation of 1D data


   20100805 Yi Gao

   ================================================== */

#include "kde_via_diffusion.h"
#include "histc.h"
#include "dct.h"

// std
#include <vector>
#include <cmath>
//tst
#include <iostream>
//tst//
//#include <omp.h>


#ifndef kde_via_diffusion_txx_
#define kde_via_diffusion_txx_

namespace douher
{
  template<typename T>
  void kde_via_diffusion(const double* data, std::size_t num_data, double xMin, double xMax, std::size_t num_pdf_samples, \
                         double* pdfx, double* pdf)
  {
    // set up x
    pdfx[0] = xMin;
    pdfx[num_pdf_samples-1] =  xMax;
    double interval = (xMax - xMin)/(num_pdf_samples-1);
    for (std::size_t i = 1; i < num_pdf_samples-1; ++i)
      {
        pdfx[i] = xMin + i*interval;
      }

    long* initial_histogram = new long[num_pdf_samples];
    histc<double>(data, num_data, pdfx, num_pdf_samples, initial_histogram);

    double* initial_pdf = new double[num_pdf_samples];
    
    {
      double sum = 0.0;
      for (std::size_t i = 0; i < num_pdf_samples; ++i)
        {
          initial_pdf[i] = initial_histogram[i]/static_cast<double>(num_pdf_samples);
          sum += initial_pdf[i];
        }

      for (std::size_t i = 0; i < num_pdf_samples; ++i)
        {
          initial_pdf[i] /= sum;
        }
    }

    delete[] initial_histogram;
    initial_histogram = 0;


    double* a = new double[num_pdf_samples];
    dct1(initial_pdf, num_pdf_samples, a);

    delete[] initial_pdf; initial_pdf = 0;

//     a[0] /= 2.0; // hack, the fftw's dct (type II)'s first element is
//     // twice that of the one computed in the kde matlab dct
//     // code.


    // now compute the optimal bandwidth^2 using the referenced method
    long* I = new long[num_pdf_samples-1];
    double* a2 = new double[num_pdf_samples-1];
    for (std::size_t i = 1; i <= num_pdf_samples-1; ++i)
      {
        I[i-1] = i*i;
        a2[i-1] = a[i]*a[i]/4;
      }



    //Use bisection method to solve the equation 0=t-zeta*gamma^[5](t)
    //in [0, 0.1]
    double solution = 0.0;
    {
      double lo = 0;
      double hi = 0.1;
      if (kde_via_diffusion_foo<char>(0.0, num_data, I, a2, num_pdf_samples) <= 0)
        {
          lo = 0.0;
          hi = 0.1;
        }
      else
        {
          lo = 0.1;
          hi = 0.0;
        }

      solution = lo + (hi - lo)/2.0;

      //while (solution != lo && solution != hi)
      while (fabs(solution - lo) >= 1e-10 && fabs(hi - solution) >= 1e-10)
        {
          double tmp = kde_via_diffusion_foo<char>(solution, num_data, I, a2, num_pdf_samples);
        
//           //tst
//           std::cout<<"\t function = "<<tmp<<std::endl;
//           //tst//

          if (tmp <= 0)
            {
              lo = solution;
            }
          else
            {
              hi = solution;
            }
          solution = lo + (hi - lo)/2.0;

//           //tst
//           std::cout<<"\t\t solution = "<<solution<<std::endl;
//           //tst//
        }
    }


    //solution = 1.8217e-05;

    //tst
    std::cout<<"solution = "<<solution<<std::endl;
    //tst//

    // smooth the discrete cosine transform of initial data using t_star
    double* a_t = new double[num_pdf_samples];

    {
      double pi2 = vnl_math::pi;
      pi2 *= pi2;
      for (std::size_t i = 0; i < num_pdf_samples; ++i)
        {
          a_t[i] = a[i]*exp(-static_cast<double>(i)*static_cast<double>(i)*pi2*solution/2.0);
        }
    }

    // now apply the inverse discrete cosine transform
    double* density = new double[num_pdf_samples];
    idct1(a_t, num_pdf_samples, density);

    double R = xMax - xMin;



    // normalize to get final results
    for (std::size_t i = 0; i < num_pdf_samples; ++i)
      {
        pdf[i] = density[i] / R + vnl_math::eps;
      }    

    // double pdf_area = 0.0;
    // for (std::size_t i = 0; i < num_pdf_samples - 1; ++i)
    //   {
    //     pdf_area += (pdfx[i+1] - pdfx[i])*(pdf[i] + pdf[i+1])/2.0;
    //   }    


    // //tst
    // std::cout<<pdf_area<<std::endl;
    // //tst//

    // for (std::size_t i = 0; i < num_pdf_samples; ++i)
    //   {
    //     pdf[i] /= pdf_area;
    //   }    



    delete[] density; density = 0;
    delete[] a_t; a_t = 0;
    delete[] I; I = 0;
    delete[] a2; a2 = 0;
    delete[] a; a = 0;

    return;
  }

  template<typename Tnull>
  double kde_via_diffusion_foo(double t, std::size_t num_data, long* I, double* a2, std::size_t num_pdf_samples)
  {
    // Note that by the flow of this program, a2 and I are both of
    // length num_pdf_samples-1

    // this implements the function t-zeta*gamma^[l](t)
    double l = 7;
    double f = 0.0;

    double pi2 = vnl_math::pi;
    pi2 *= pi2;

    for (std::size_t i = 0; i < num_pdf_samples-1; ++i)
      {
        f += (pow(static_cast<double>(I[i]), l)*a2[i]*exp(-static_cast<double>(I[i])*pi2*t));
      }
    f *= 2.0*pow(vnl_math::pi, 2.0*l);

    for (int s = l-1; s >= 2; --s)
      {
        double p = 1.0;
        for (int i = 1; i <= 2*(s-1); i += 2)
          {
            p *= static_cast<double>(i);
          }
        
        double K0 = p*sqrt(2.0*vnl_math::pi);
        double c = (1.0 + pow(0.5, static_cast<double>(s)+0.5))/3.0;
        double time = pow(2.0*c*K0/static_cast<double>(num_data)/f, 2.0/(3.0 + 2.0*static_cast<double>(s)));

        f = 0.0;
        for (std::size_t i = 0; i < num_pdf_samples-1; ++i)
          {
            f += (pow(static_cast<double>(I[i]), static_cast<double>(s))*a2[i]*exp(-static_cast<double>(I[i])*pi2*time));
          }

        f *= 2.0*pow(vnl_math::pi, 2.0*static_cast<double>(s));
      }


    return t - pow(2.0*static_cast<double>(num_data)*sqrt(vnl_math::pi)*f, -0.4);
  }


}

#endif //KDEViaDiffusion_txx_
