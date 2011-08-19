#include "DownsampleHeuristics.h"
#include "ConvertSlicerROIToRegion.h"
#include "itkSlicerBoxSpatialObject.h"

#include <itkImage.h>

#include <iostream>
#include <cstdlib>

namespace
{

typedef itk::Image<short, 3> ImageType;
typedef ImageType::RegionType  RegionType;
typedef RegionType::SizeType   SizeType;
typedef RegionType::IndexType   IndexType;
typedef ImageType::SpacingType SpacingType;

ImageType::Pointer
createTestImage(SizeType size,
                SpacingType spacing)
{
  ImageType::Pointer image = ImageType::New();

  IndexType ind = {{0,0,0}};

  RegionType region;
  region.SetIndex(ind);
  region.SetSize(size);

  image->SetRegions(region);
  image->SetSpacing(spacing);
  //image->Allocate();
  
  return image;
}

template <class T, class J>
void assign3(T& vec, J x, J y, J z)
{
  vec[0] = x;
  vec[1] = y;
  vec[2] = z;
}

}

int CountSamplesTest(int, char* [])
{
  const unsigned int NTESTS = 7;
  unsigned int sizes[NTESTS][3] = {{32,32,63},
                                   {32,32,64},
                                   {32,32,65},
                                   {32,32,96},
                                   {64,64,64},
                                   {128,128,33},
                                   {128,128,64}
                                   };

  double spacings[NTESTS][3] = {{8.0,8.0,2.0},
                                {8.0,8.0,2.0},
                                {8.0,8.0,2.0},
                                {8.0,8.0,2.0},
                                {4.0,4.0,2.0},
                                {1.0936,1.0936,3.00001},
                                {1.0936,1.0936,3.00001}
                                };
  
  for(unsigned int i = 0; i < NTESTS; ++i)
    {
    std::cout << "===== TEST " << i  << " =====" << std::endl;
    SizeType size;
    size[0] = sizes[i][0];
    size[1] = sizes[i][1];
    size[2] = sizes[i][2];

    SpacingType spacing;
    spacing[0] = spacings[i][0];
    spacing[1] = spacings[i][1];
    spacing[2] = spacings[i][2];

    ImageType::Pointer image = createTestImage(size, spacing);

    std::cout << "spacing: " << image->GetSpacing() << std::endl;
    std::cout << "size: " << image->GetLargestPossibleRegion().GetSize() 
      << std::endl;

    itk::Point<double, 3> p1;
    assign3(p1, 0.0, 0.0, 0.0);
    itk::Point<double, 3> p2;
    assign3(p2, 1.0, 5.0, 2.0);

    itk::SlicerBoxSpatialObject<3>::Pointer box1 =
      convertPointsToBoxSpatialObject(p1, p2);

    unsigned long count = countInsideVoxels<ImageType>(image, box1);
    std::cout << "count: " << count << std::endl;
    }

  return EXIT_SUCCESS;
}
