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

#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConstrainedValueAdditionImageFilter.h"
#include "itkMultiplyByConstantImageFilter.h"
#include "itkDivideByConstantImageFilter.h"
#include "itkCastImageFilter.h"

// image types
typedef itk::OrientedImage<short, 3> ImageType;
typedef itk::OrientedImage<float, 3> FloatImageType;

namespace {

// templated method for writing out any type
template<class T> int ReCastAndWriteOut(FloatImageType *image, std::string outputFile, T, std::string type)
{

  typedef    T       OutputPixelType;

  typedef itk::OrientedImage< OutputPixelType, 3 >   OutputImageType;

  typedef itk::CastImageFilter<FloatImageType, OutputImageType> CastFilterType;
  typedef itk::ImageFileWriter<OutputImageType> WriterType;

  // now we will re-cast to the output type
  std::cout << "Re-Casting Atlas to " << type << std::endl;

  typename CastFilterType::Pointer cast = CastFilterType::New();
  cast->SetInput(image);
  cast->Update();

  // write out the new image
  typename WriterType::Pointer writer = WriterType::New();

  const char *filename = outputFile.c_str();

  writer->SetFileName(filename);
  writer->SetInput(cast->GetOutput());
  writer->SetUseCompression(1);

  //writer->Print(std::cout);
  std::cout << "Writing Atlas to: " << filename << std::endl;
  writer->Update();

  return 1;

}
}


int main(int argc, char ** argv)
{
  // Verify the number of parameters in the command line
  if (argc < 6)
    {
    std::cerr << "AtlasCreator Combiner v0.1" << std::endl;
    std::cerr << "Add a bunch of images to a sum image, reCast or normalize if requested." << std::endl;
    std::cerr << " " << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0]
        << " labelValue filePathsList outputAtlasDirectory reCastString normalizeTo "
        << std::endl;
    std::cerr << "Example: " << std::endl;
    std::cerr << argv[0]
        << " 6 \"case01.nrrd case02.nrrd case03.nrrd\" /tmp/acout/ \"double\" 100"
        << std::endl;
    return EXIT_FAILURE;
    }

  // let's define some hardcoded cast strings
  std::string charString = "char";
  std::string unsignedCharString = "unsigned char";
  std::string doubleString = "double";
  std::string floatString = "float";
  std::string intString = "int";
  std::string unsignedIntString = "unsigned int";
  std::string longString = "long";
  std::string unsignedLongString = "unsigned long";
  std::string shortString = "short";
  std::string unsignedShortString = "unsigned short";

  // now we parse the arguments
  int label;
  std::vector < std::string > filePathsList;
  std::string outputDirectory;
  std::string reCastString;
  int normalizeTo;

  // label
  std::stringstream ss;
  ss << argv[1];

  ss >> label;

  std::cout << "Label: " << label << std::endl;

  // filePathsList
  std::istringstream iss(argv[2]);
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

  // outputDirectory
  outputDirectory = argv[3];
  std::cout << "OutputDirectory: " << outputDirectory << std::endl;

  // reCastString
  reCastString = argv[4];
  std::cout << "ReCast String: " << reCastString << std::endl;

  // normalizeTo
  std::stringstream ss2;
  ss2 << argv[5];

  ss2 >> normalizeTo;

  std::cout << "NormalizeTo: " << normalizeTo << std::endl;

  typedef itk::ImageFileReader<ImageType> ReaderType;
  typedef itk::NearestNeighborInterpolateImageFunction<ImageType>
      InterpolatorType;
  typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleType;
  typedef itk::BinaryThresholdImageFilter<ImageType, ImageType>
      ThresholdFilterType;
  typedef itk::ConstrainedValueAdditionImageFilter<ImageType, ImageType,
      ImageType> AddFilterType;
  typedef itk::CastImageFilter<ImageType, FloatImageType> CastToFloatFilterType;
  typedef itk::MultiplyByConstantImageFilter<FloatImageType, int,
      FloatImageType> MultiplyFilterType;
  typedef itk::DivideByConstantImageFilter<FloatImageType, int, FloatImageType>
      DivideFilterType;

  bool firstRun = true;

  ImageType::Pointer imageSum = ImageType::New();
  ImageType::RegionType region = imageSum->GetLargestPossibleRegion();
  ImageType::SizeType size = region.GetSize();
  imageSum->SetRegions(size);
  imageSum->Allocate();
  imageSum->FillBuffer(0);

  FloatImageType::Pointer imageFloat = FloatImageType::New();

  int numberOfFiles = filePathsList.size();

  try
    {

    // loop through all files
    for (int i = 0; i < numberOfFiles; i++)
      {

      const char *filename = filePathsList[i].c_str();

      std::cout << "Loading: " << filename << std::endl;

      ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(filename);
      reader->Update();

      // binarize the loaded image according to the label
      ThresholdFilterType::Pointer threshold = ThresholdFilterType::New();
      threshold->SetInput(reader->GetOutput());
      threshold->SetLowerThreshold(label);
      threshold->SetUpperThreshold(label);
      threshold->SetInsideValue(1);
      threshold->SetOutsideValue(0);
      threshold->Update();

      // get the thresholded image
      ImageType::Pointer image = threshold->GetOutput();

      if (!firstRun)
        {

        // start adding with the 2nd run
        InterpolatorType::Pointer interp = InterpolatorType::New();
        interp->SetInputImage(imageSum);

        std::cout << "Resampling: " << filename << std::endl;
        ResampleType::Pointer resample = ResampleType::New();
        resample->SetInput(imageSum);
        resample->SetOutputParametersFromImage(image);
        resample->SetInterpolator(interp);
        resample->SetDefaultPixelValue(0);
        resample->ReleaseDataFlagOn();

        std::cout << "Adding: " << filename << std::endl;
        // now add it to the imageSum
        AddFilterType::Pointer add = AddFilterType::New();
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

      }

    // we need to cast to float to always have float here
    CastToFloatFilterType::Pointer cast = CastToFloatFilterType::New();
    cast->SetInput(imageSum);
    cast->Update();

    imageFloat = cast->GetOutput();

    // now normalize if requested
    if (normalizeTo != -1)
      {

      std::cout << "Dividing Atlas by: " << numberOfFiles << std::endl;

      DivideFilterType::Pointer div = DivideFilterType::New();
      div->SetInput(imageFloat);
      div->SetConstant(numberOfFiles);
      div->Update();

      imageFloat = div->GetOutput();

      // now normalize to a different value than 1, if requested
      if (normalizeTo != 1)
        {

        std::cout << "Multiplying Atlas with: " << normalizeTo << std::endl;

        MultiplyFilterType::Pointer mul = MultiplyFilterType::New();
        mul->SetInput(imageFloat);
        mul->SetConstant(normalizeTo);
        mul->Update();

        imageFloat = mul->GetOutput();
        }

      }

    // we have a float image now

    std::string outputFile = outputDirectory;
    outputFile += "atlas";
    outputFile += argv[1]; // label
    outputFile += ".nrrd";

    if (normalizeTo == 1)
      {
      // we need a double here
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<double>(0),"Double");
      }
    else if (reCastString.compare(shortString) == 0)
      {
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<short>(0),reCastString);
      }
    else if (reCastString.compare(unsignedShortString) == 0)
      {
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<unsigned short>(0),reCastString);
      }
    else if (reCastString.compare(charString) == 0)
      {
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<char>(0),reCastString);
      }
    else if (reCastString.compare(unsignedCharString) == 0)
      {
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<unsigned char>(0),reCastString);
      }
    else if (reCastString.compare(doubleString) == 0)
      {
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<double>(0),reCastString);
      }
    else if (reCastString.compare(floatString) == 0)
      {
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<float>(0),reCastString);
      }
    else if (reCastString.compare(intString) == 0)
      {
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<int>(0),reCastString);
      }
    else if (reCastString.compare(unsignedIntString) == 0)
      {
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<unsigned int>(0),reCastString);
      }
    else if (reCastString.compare(longString) == 0)
      {
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<long>(0),reCastString);
      }
    else if (reCastString.compare(unsignedLongString) == 0)
      {
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<unsigned long>(0),reCastString);
      }
    else
      {
      // assume short
      ReCastAndWriteOut(imageFloat,outputFile,static_cast<short>(0),reCastString);
      }


    } catch (itk::ExceptionObject & err)
    {

    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;

    }

  return EXIT_SUCCESS;
}
