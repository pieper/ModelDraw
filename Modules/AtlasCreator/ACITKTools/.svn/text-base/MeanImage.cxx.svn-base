/*=========================================================================

 Program:   Insight Segmentation & Registration Toolkit
 Module:    $RCSfile: Combiner.cxx,v $
 Language:  C++
 Date:      $Date: 2009-03-16 20:36:50 $
 Version:   $Revision: 1.00 $

 Copyright (c) Insight Software Consortium. All rights reserved.
 See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageIOBase.h"
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkConstrainedValueAdditionImageFilter.h"
#include "itkDivideByConstantImageFilter.h"
#include "itkCastImageFilter.h"

namespace
{

  template<class T> int ReadSumWrite(std::vector<std::string> filePathsList, std::string outputFilePath, T)
  {

    typedef T InputPixelType;
    typedef T OutputPixelType;

    typedef itk::OrientedImage<float, 3> FloatImageType;
    typedef itk::OrientedImage<InputPixelType, 3> InputImageType;
    typedef itk::OrientedImage<OutputPixelType, 3> OutputImageType;

    typedef itk::ImageFileReader<InputImageType> ReaderType;

    typedef itk::CastImageFilter<InputImageType, FloatImageType>
        CastToFloatFilterType;
    typedef itk::DivideByConstantImageFilter<FloatImageType, int, FloatImageType>
        DivideFilterType;

    typedef itk::BSplineInterpolateImageFunction<FloatImageType>
        InterpolatorType;
    typedef itk::ResampleImageFilter<FloatImageType, FloatImageType>
        ResampleType;
    typedef itk::ConstrainedValueAdditionImageFilter<FloatImageType,
        FloatImageType, FloatImageType> AddFilterType;

    typedef itk::CastImageFilter<FloatImageType, OutputImageType>
        CastToOutputFilterType;

    typedef itk::ImageFileWriter<OutputImageType> WriterType;

    typename FloatImageType::Pointer imageSum = FloatImageType::New();
    typename FloatImageType::RegionType region =
        imageSum->GetLargestPossibleRegion();
    typename FloatImageType::SizeType size = region.GetSize();
    imageSum->SetRegions(size);
    imageSum->Allocate();
    imageSum->FillBuffer(0);

    int numberOfFiles = filePathsList.size();

    bool firstRun = true;

    // loop through all files
    for (int i = 0; i < numberOfFiles; i++)
      {

      const char *filename = filePathsList[i].c_str();

      std::cout << "Loading: " << filename << std::endl;

      typename ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(filename);
      reader->Update();

      // we need to cast to float to always have float here
      typename CastToFloatFilterType::Pointer cast =
          CastToFloatFilterType::New();
      cast->SetInput(reader->GetOutput());
      cast->Update();

      // divide
      typename DivideFilterType::Pointer div = DivideFilterType::New();
      div->SetInput(cast->GetOutput());
      div->SetConstant(numberOfFiles);
      div->Update();

      // buffer as image
      FloatImageType::Pointer image = div->GetOutput();

      if (!firstRun)
        {

        // start adding with the 2nd run
        typename InterpolatorType::Pointer interp = InterpolatorType::New();
        interp->SetInputImage(imageSum);

        std::cout << "Resampling: " << filename << std::endl;
        typename ResampleType::Pointer resample = ResampleType::New();
        resample->SetInput(imageSum);
        resample->SetOutputParametersFromImage(image);
        resample->SetInterpolator(interp);
        resample->SetDefaultPixelValue(0);
        resample->ReleaseDataFlagOn();

        std::cout << "Adding: " << filename << std::endl;
        // now add it to the imageSum
        typename AddFilterType::Pointer add = AddFilterType::New();
        add->SetInput1(resample->GetOutput());
        add->SetInput2(image);
        add->Update();

        // replace imageSum with the sum of the two images
        imageSum = add->GetOutput();

        }
      else
        {
        // this is the first run

        // replace imageSum with the loaded image
        imageSum = image;

        firstRun = false;
        }

      } // end for

    // now reCast to output type
    typename CastToOutputFilterType::Pointer outputCast =
        CastToOutputFilterType::New();
    outputCast->SetInput(imageSum);
    outputCast->Update();

    // write mean image
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilePath.c_str());
    writer->SetInput(outputCast->GetOutput());
    writer->SetUseCompression(1);
    writer->Update();

    return EXIT_SUCCESS;

  }

  //To check the image voxel type
  void GetImageType(std::string fileName, itk::ImageIOBase::IOPixelType &pixelType, itk::ImageIOBase::IOComponentType &componentType)
  {
    typedef itk::Image<unsigned char, 3> ImageType;
    itk::ImageFileReader<ImageType>::Pointer imageReader;
    imageReader = itk::ImageFileReader<ImageType>::New();
    imageReader->SetFileName(fileName.c_str());
    imageReader->UpdateOutputInformation();
    pixelType = imageReader->GetImageIO()->GetPixelType();
    componentType = imageReader->GetImageIO()->GetComponentType();
  }

} // end of anonymous namespace


int main(int argc, char * argv[])
{
  // Verify the number of parameters in the command line
  if (argc < 3)
    {
    std::cerr << "AtlasCreator MeanImage v0.1" << std::endl;
    std::cerr << "Create a mean image of a bunch of images." << std::endl;
    std::cerr << "Note: The output type will be the same as the input type."
        << std::endl;
    std::cerr << " " << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " filePathsList outputFilePath " << std::endl;
    std::cerr << "Example: " << std::endl;
    std::cerr << argv[0]
        << " \"case01.nrrd case02.nrrd case03.nrrd\" /tmp/mean.nrrd"
        << std::endl;
    return EXIT_FAILURE;
    }

  itk::ImageIOBase::IOPixelType pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  std::vector < std::string > filePathsList;
  std::string outputFilePath;

  // outputFilePath
  outputFilePath = argv[2];
  std::cout << "OutputFilePath: " << outputFilePath << std::endl;

  // filePathsList
  std::istringstream iss(argv[1]);
  do
    {
    std::string sub;
    iss >> sub;
    if (sub.length() != 0)
      {
      filePathsList.push_back(sub);
      }
    }
  while (iss);

  if (filePathsList.size() == 0)
    {
    std::cerr << "No input files." << std::endl;
    return EXIT_FAILURE;
    }

  try
    {
    // we get the pixeltype of the first image
    GetImageType(filePathsList[0], pixelType, componentType);

    switch (componentType)
      {
      case itk::ImageIOBase::UCHAR:
        return ReadSumWrite(filePathsList, outputFilePath,
            static_cast<unsigned char> (0));
        break;
      case itk::ImageIOBase::CHAR:
        return ReadSumWrite(filePathsList, outputFilePath,
            static_cast<char> (0));
        break;
      case itk::ImageIOBase::USHORT:
        return ReadSumWrite(filePathsList, outputFilePath,
            static_cast<unsigned short> (0));
        break;
      case itk::ImageIOBase::SHORT:
        return ReadSumWrite(filePathsList, outputFilePath,
            static_cast<short> (0));
        break;
      case itk::ImageIOBase::UINT:
        return ReadSumWrite(filePathsList, outputFilePath,
            static_cast<unsigned int> (0));
        break;
      case itk::ImageIOBase::INT:
        return ReadSumWrite(filePathsList, outputFilePath, static_cast<int> (0));
        break;
      case itk::ImageIOBase::ULONG:
        return ReadSumWrite(filePathsList, outputFilePath,
            static_cast<unsigned long> (0));
        break;
      case itk::ImageIOBase::LONG:
        return ReadSumWrite(filePathsList, outputFilePath,
            static_cast<long> (0));
        break;
      case itk::ImageIOBase::FLOAT:
        return ReadSumWrite(filePathsList, outputFilePath,
            static_cast<float> (0));
        break;
      case itk::ImageIOBase::DOUBLE:
        return ReadSumWrite(filePathsList, outputFilePath,
            static_cast<double> (0));
        break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cerr << "Unknown component type.. Aborting!" << std::cerr;
        return EXIT_FAILURE;
      }

    } catch (itk::ExceptionObject &excep)
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
