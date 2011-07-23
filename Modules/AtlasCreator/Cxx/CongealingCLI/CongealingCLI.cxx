#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>

#include <stdio.h>
#ifdef WIN32
#include <process.h>
#endif
#include "CongealingCLICLP.h"


// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace {

//-------------------------------------------------------------------------------------------------
std::string ParseExperimentalParameters(int congeal_optimize_bestpoints, std::string test)
{

  std::stringstream out;
  std::string output;

  out << "# experimental" << std::endl;
  out << "congeal.optimize.bestpoints " << congeal_optimize_bestpoints << std::endl;
  out << "test " << test << std::endl;

  output = out.str();

  return output;

}

//-------------------------------------------------------------------------------------------------
std::string ParseInputParameters(int congeal_inputfiles, std::string congeal_inputfile_format, std::string congeal_inputfiles_list)
{

  std::stringstream out;
  std::string output;

  out << "# input" << std::endl;
  out << "congeal.inputfiles " << congeal_inputfiles << std::endl;
  out << "congeal.inputfile.format " << congeal_inputfile_format << std::endl;
  out << "congeal.inputfiles.list " << congeal_inputfiles_list << std::endl;

  output = out.str();

  return output;

}

//-------------------------------------------------------------------------------------------------
std::string ParseOptimizationParameters(std::string congeal_optimize_algorithm)
{

  std::stringstream out;
  std::string output;

  out << "# optimization" << std::endl;
  out << "congeal.optimize.algorithm " << congeal_optimize_algorithm << std::endl;

  output = out.str();

  return output;

}

//-------------------------------------------------------------------------------------------------
std::string ParseRandomWalkParameters(float congeal_optimize__randomwalk__kernel, int congeal_optimize__randomwalk__steps, int congeal_optimize__randomwalk__directions)
{

  std::stringstream out;
  std::string output;

  out << "# randomwalk" << std::endl;
  out << "congeal.optimize[randomwalk].kernel " << congeal_optimize__randomwalk__kernel << std::endl;
  out << "congeal.optimize[randomwalk].steps " << congeal_optimize__randomwalk__steps << std::endl;
  out << "congeal.optimize[randomwalk].directions " << congeal_optimize__randomwalk__directions << std::endl;

  output = out.str();

  return output;

}

//-------------------------------------------------------------------------------------------------
std::string ParseErrorFunctionParameters(std::string congeal_optimize_error)
{

  std::stringstream out;
  std::string output;

  out << "# error function" << std::endl;
  out << "congeal.optimize.error " << congeal_optimize_error << std::endl;

  output = out.str();

  return output;

}

//-------------------------------------------------------------------------------------------------
std::string ParseErrorFunctionParzenParameters(float congeal_error__parzen__sigma, float congeal_error__parzen__apriori)
{

  std::stringstream out;
  std::string output;

  out << "# parzen error function" << std::endl;
  out << "congeal.error[parzen].sigma " << congeal_error__parzen__sigma << std::endl;
  out << "congeal.error[parzen].apriori " << congeal_error__parzen__apriori << std::endl;

  output = out.str();

  return output;

}

//-------------------------------------------------------------------------------------------------
std::string ParseOutputParameters(std::string congeal_output_prefix,
                                  int congeal_output_colors_mid,
                                  int congeal_output_colors_range,
                                  int congeal_output_sourcegrid,
                                  int congeal_optimize_progresspoints,
                                  int congeal_output_average_width,
                                  int congeal_output_average_height)
{

  std::stringstream out;
  std::string output;

  out << "# output" << std::endl;
  out << "congeal.output.prefix " << congeal_output_prefix << std::endl;
  out << "congeal.output.colors.mid " << congeal_output_colors_mid << std::endl;
  out << "congeal.output.colors.range " << congeal_output_colors_range << std::endl;
  out << "congeal.output.sourcegrid " << congeal_output_sourcegrid << std::endl;
  out << "congeal.optimize.progresspoints " << congeal_optimize_progresspoints << std::endl;
  out << "congeal.output.average.width " << congeal_output_average_width << std::endl;
  out << "congeal.output.average.height " << congeal_output_average_height << std::endl;

  output = out.str();

  return output;

}

//-------------------------------------------------------------------------------------------------
std::string ParseInitialStepsParameters(float congeal_initialsteps_translate, float congeal_initialsteps_rotate, float congeal_initialsteps_scale, float congeal_initialsteps_warp)
{

  std::stringstream out;
  std::string output;

  out << "# initial steps" << std::endl;
  out << "congeal.initialsteps.translate " << congeal_initialsteps_translate << std::endl;
  out << "congeal.initialsteps.rotate " << congeal_initialsteps_rotate << std::endl;
  out << "congeal.initialsteps.scale " << congeal_initialsteps_scale << std::endl;
  out << "congeal.initialsteps.warp " << congeal_initialsteps_warp << std::endl;

  output = out.str();

  return output;

}

//-------------------------------------------------------------------------------------------------
std::string ParseScheduleParameters(std::vector<std::string> congeal_schedule__n__cache,
                                    std::vector<int> congeal_schedule__n__downsample,
                                    std::vector<std::string> congeal_schedule__n__optimize_affine,
                                    std::vector<int> congeal_schedule__n__warpfield__0__size,
                                    std::vector<int> congeal_schedule__n__warpfield__1__size,
                                    std::vector<int> congeal_schedule__n__warpfield__2__size,
                                    std::vector<int> congeal_schedule__n__warpfield__3__size,
                                    std::vector<std::string> congeal_schedule__n__optimize_warp__0__,
                                    std::vector<std::string> congeal_schedule__n__optimize_warp__1__,
                                    std::vector<std::string> congeal_schedule__n__optimize_warp__2__,
                                    std::vector<std::string> congeal_schedule__n__optimize_warp__3__,
                                    std::vector<int> congeal_schedule__n__optimize_iterations,
                                    std::vector<int> congeal_schedule__n__optimize_samples)
{

  std::stringstream out;
  std::string output;

  out << "# schedules" << std::endl;

  // now define "n" in the config file
  out << std::endl;
  out << "n -1";
  out << std::endl;

  // we already checked if all vectors have the same size, so we just loop through all of them with one loop
  for (unsigned int i=0;i<congeal_schedule__n__cache.size();i++)
    {

    out << "congeal.schedule[{++n}].cache " << congeal_schedule__n__cache[i] << std::endl;
    out << "congeal.schedule[{$n}].downsample " << congeal_schedule__n__downsample[i] << std::endl;
    out << "congeal.schedule[{$n}].optimize.affine " << congeal_schedule__n__optimize_affine[i] << std::endl;
    out << "congeal.schedule[{$n}].warpfield[0].size " << congeal_schedule__n__warpfield__0__size[i] << std::endl;
    out << "congeal.schedule[{$n}].warpfield[1].size " << congeal_schedule__n__warpfield__1__size[i] << std::endl;
    out << "congeal.schedule[{$n}].warpfield[2].size " << congeal_schedule__n__warpfield__2__size[i] << std::endl;
    out << "congeal.schedule[{$n}].warpfield[3].size " << congeal_schedule__n__warpfield__3__size[i] << std::endl;
    out << "congeal.schedule[{$n}].optimize.warp[0] " << congeal_schedule__n__optimize_warp__0__[i] << std::endl;
    out << "congeal.schedule[{$n}].optimize.warp[1] " << congeal_schedule__n__optimize_warp__1__[i] << std::endl;
    out << "congeal.schedule[{$n}].optimize.warp[2] " << congeal_schedule__n__optimize_warp__2__[i] << std::endl;
    out << "congeal.schedule[{$n}].optimize.warp[3] " << congeal_schedule__n__optimize_warp__3__[i] << std::endl;
    out << "congeal.schedule[{$n}].optimize.iterations " << congeal_schedule__n__optimize_iterations[i] << std::endl;
    out << "congeal.schedule[{$n}].optimize.samples " << congeal_schedule__n__optimize_samples[i] << std::endl;
    out << std::endl;

    }

  // now define "congeal.schedules" automatically
  out << std::endl;
  out << "congeal.schedules {++n}";
  out << std::endl;

  output = out.str();

  return output;

}

} // end of anonymous namespace



//-------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  PARSE_ARGS;

  std::stringstream out;
  std::string output("");

  // step 1: generate config string

  out << ParseExperimentalParameters(congeal_optimize_bestpoints, test);
  out << std::endl;
  out << ParseInputParameters(congeal_inputfiles, congeal_inputfile_format, congeal_inputfiles_list);
  out << std::endl;
  out << ParseOptimizationParameters(congeal_optimize_algorithm);
  out << std::endl;

  if (!strcmp(congeal_optimize_algorithm.c_str(),"randomwalk"))
    {
    // randomwalk was selected
    out << ParseRandomWalkParameters(congeal_optimize__randomwalk__kernel, congeal_optimize__randomwalk__steps, congeal_optimize__randomwalk__directions);
    out << std::endl;
    }

  out << ParseErrorFunctionParameters(congeal_optimize_error);
  out << std::endl;

  if (!strcmp(congeal_optimize_error.c_str(),"parzen"))
    {
    // parzen was selected
    out << ParseErrorFunctionParzenParameters(congeal_error__parzen__sigma, congeal_error__parzen__apriori);
    out << std::endl;
    }

  out << ParseOutputParameters(congeal_output_prefix,
                               congeal_output_colors_mid,
                               congeal_output_colors_range,
                               congeal_output_sourcegrid,
                               congeal_optimize_progresspoints,
                               congeal_output_average_width,
                               congeal_output_average_height);
  out << std::endl;

  out << ParseInitialStepsParameters(congeal_initialsteps_translate, congeal_initialsteps_rotate, congeal_initialsteps_scale, congeal_initialsteps_warp);
  out << std::endl;


  // now parse the schedule options, which is a little bit more complicated
  // since we deal with vectors, let's do some sanity checks
  unsigned int generalSize = congeal_schedule__n__cache.size();
  if (congeal_schedule__n__cache.size() != generalSize ||
      congeal_schedule__n__downsample.size() != generalSize ||
      congeal_schedule__n__optimize_affine.size() != generalSize ||
      congeal_schedule__n__warpfield__0__size.size() != generalSize ||
      congeal_schedule__n__warpfield__1__size.size() != generalSize ||
      congeal_schedule__n__warpfield__2__size.size() != generalSize ||
      congeal_schedule__n__warpfield__3__size.size() != generalSize ||
      congeal_schedule__n__optimize_warp__0__.size() != generalSize ||
      congeal_schedule__n__optimize_warp__1__.size() != generalSize ||
      congeal_schedule__n__optimize_warp__2__.size() != generalSize ||
      congeal_schedule__n__optimize_warp__3__.size() != generalSize ||
      congeal_schedule__n__optimize_iterations.size() != generalSize ||
      congeal_schedule__n__optimize_samples.size() != generalSize)
  {
    std::cerr << "The scheduler options have different sizes! Please make sure that all comma separated values have the same number of elements.." << std::endl;
    return EXIT_FAILURE;
  }

  // all vectors are valid, let's parse..
  out << ParseScheduleParameters(congeal_schedule__n__cache,
                                 congeal_schedule__n__downsample,
                                 congeal_schedule__n__optimize_affine,
                                 congeal_schedule__n__warpfield__0__size,
                                 congeal_schedule__n__warpfield__1__size,
                                 congeal_schedule__n__warpfield__2__size,
                                 congeal_schedule__n__warpfield__3__size,
                                 congeal_schedule__n__optimize_warp__0__,
                                 congeal_schedule__n__optimize_warp__1__,
                                 congeal_schedule__n__optimize_warp__2__,
                                 congeal_schedule__n__optimize_warp__3__,
                                 congeal_schedule__n__optimize_iterations,
                                 congeal_schedule__n__optimize_samples);
  out << std::endl;

  // empty line at the end
  out << std::endl;
  output = out.str();

  //
  // std::cout << output << std::endl;

  // step 2: write config string to output Path

  if (!strcmp(outputPath.c_str(),""))
    {
    std::cerr << "Couldn't create output file.\n";
    return EXIT_FAILURE;
    }

  std::ofstream of(outputPath.c_str());

  if (of)
    {
    of << output;
    of.close( );
    }

  // step 3: launch congeal or just print the filePath to the config file

  if (!strcmp(launch.c_str(),""))
    {
    std::cout << "Configuration file written to " << outputPath.c_str() << std::endl;
    }
  else
    {
#ifdef WIN32
    _execlp(launch.c_str(),launch.c_str(),outputPath.c_str(),(char *) 0);
#else
    execlp(launch.c_str(),launch.c_str(),outputPath.c_str(),(char *) 0);
#endif
    }


  return EXIT_SUCCESS;
}

