// pk_solver_exec.cxx

#include <vcl_cstdio.h>
#include "PkSolver.h"

int main (void)
{
  int signalSz = 32;
  float* timeAxis = new float [signalSz];
  float* pixelIntensity = new float [signalSz];
  float* AIF = new float [signalSz];
  float Ktrans;
  float Ve;
  float Fpv;

  //Initialize values of timeAxis, pixelIntensity, AIF.

  //Run PK solver.
  bool result = PkSolver::pk_solver (signalSz, timeAxis, pixelIntensity, AIF, Ktrans, Ve, Fpv);
  delete timeAxis;
  delete pixelIntensity;
  delete AIF;
  vcl_printf ("  Ktrans %f\n", Ktrans);
  vcl_printf ("  Ve %f\n", Ve);
  vcl_printf ("  Fpv %f\n", Fpv);

  return result;

}


