#include "vtkCTHandBoneClass.h"
#include "vtkImageEMGeneral.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"

//------------------------------------------------------------------------
vtkCTHandBoneClass* vtkCTHandBoneClass::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCTHandBoneClass");
  if(ret)
    {
      return (vtkCTHandBoneClass*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkCTHandBoneClass;

}


//----------------------------------------------------------------------------
vtkCTHandBoneClass::vtkCTHandBoneClass() {
}

void vtkCTHandBoneClass::xxx() {
  std::cout << "xxx" << std::endl;
}

int vtkCTHandBoneClass::blur(const char* inputImage, const char* outputImage, const char* gaussianVariance, const char* maxKernelWidth)
{
  std::cerr << "Blur Image: " << std::endl;

  // Setup pixel type and input image type
  typedef itk::Image< signed short,  3 >   ImageType;
  //To save as an 8bit image
  typedef itk::Image< unsigned char,  3 >   OutputImageType;

  //Read in Image File
  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputImage );
  reader->Update();

  typedef  signed short  PixelType;

  typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleFilterType;
  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  rescaler->SetOutputMinimum(0);
  rescaler->SetOutputMaximum(255);
  rescaler->SetInput(reader->GetOutput());
  rescaler->Update();

  typedef itk::DiscreteGaussianImageFilter<ImageType,ImageType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(rescaler->GetOutput());
  filter->SetVariance( atoi(gaussianVariance) );
  filter->SetMaximumKernelWidth( atoi(maxKernelWidth) );
  filter->Update();

  typedef itk::RescaleIntensityImageFilter<ImageType, OutputImageType> RescaleFilterType2;
  RescaleFilterType2::Pointer rescaler2 = RescaleFilterType2::New();
  rescaler2->SetOutputMinimum(0);
  rescaler2->SetOutputMaximum(255);
  rescaler2->SetInput(filter->GetOutput());
  rescaler2->Update();

  //Write blurred file out
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImage );
  writer->SetInput(rescaler2->GetOutput());
  writer->Update();

  return EXIT_SUCCESS;

}




int vtkCTHandBoneClass::binary_threshold(const char* inputDirectory, const char* outputThresholdImage, const char* _lowerThreshold, const char* _upperThreshold)
{
  std::cerr << "Binary Threshold Image: " << std::endl;

  // Setup pixel type and input image type
  typedef itk::Image< signed short,  3 >   ImageType;

  ////Read in dicom series
  //itk::GDCMSeriesFileNames::Pointer FileNameGenerator = itk::GDCMSeriesFileNames::New();
  //FileNameGenerator->SetUseSeriesDetails(false);
  //FileNameGenerator->SetDirectory( inputDirectory );

  //std::vector<std::string> seriesUIDs = FileNameGenerator->GetSeriesUIDs();

  //typedef itk::ImageSeriesReader< ImageType > ImageSeriesReaderType;
  //ImageSeriesReaderType::Pointer imageSeriesReader = ImageSeriesReaderType::New();
  //itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

  //imageSeriesReader->SetFileNames( FileNameGenerator->GetFileNames( seriesUIDs[0] ) );
  //imageSeriesReader->SetImageIO( dicomIO );
  //imageSeriesReader->Update();

  typedef itk::ImageFileReader<ImageType> InputImageType;
  InputImageType::Pointer input = InputImageType::New();
  input->SetFileName( inputDirectory );
  input->Update();

  // TODO signed vs unsigned
  typedef  signed short  InputPixelType;
  const InputPixelType lowerThreshold = atoi( _lowerThreshold );
  const InputPixelType upperThreshold = atoi( _upperThreshold );

  typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(input->GetOutput());
  filter->SetOutsideValue(255);
  filter->SetInsideValue(0);
  filter->SetLowerThreshold(lowerThreshold);
  filter->SetUpperThreshold(upperThreshold);
  filter->Update();

  typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleFilterType;
  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  rescaler->SetOutputMinimum(0);
  rescaler->SetOutputMaximum(255);
  rescaler->SetInput(filter->GetOutput());

  ImageType::Pointer DicomImage = ImageType::New();
  DicomImage = rescaler->GetOutput();

  //Write flipped file out
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputThresholdImage );
  writer->SetInput(DicomImage);
  writer->Update();

  return EXIT_SUCCESS;
}




int vtkCTHandBoneClass::flip( const char* inputFile, const char* outputFlippedImageFile, const char* flipAxisX, const char* flipAxisY, const char* flipAxisZ )
{
  std::cerr << "Flip Image: " << std::endl;

  // Setup pixel type and input image type
  typedef itk::Image< signed short,  3 >   ImageType;

  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFile );
  reader->Update();

  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFlippedImageFile );

  typedef itk::FlipImageFilter< ImageType >  FilterType;
  FilterType::Pointer filter = FilterType::New();

  //Setup input to flip image filter
  typedef FilterType::FlipAxesArrayType     FlipAxesArrayType;
  FlipAxesArrayType flipArray;
  flipArray[0] = flipAxisX;
  flipArray[1] = flipAxisY;
  flipArray[2] = flipAxisZ;

  //Perform filtering
  filter->SetFlipAxes( flipArray );
  filter->SetInput( reader->GetOutput());

  ImageType::Pointer DicomImage = ImageType::New();
  DicomImage = filter->GetOutput();

  //Write flipped file out
  writer->SetInput(DicomImage);
  writer->Update();

  return EXIT_SUCCESS;
}




int vtkCTHandBoneClass::fiducial_threshold(const char* inputImage, const char* outputImage, const char* fiducialfile, const char* logfile )
{
  std::cerr << "Fiducial Threshold: " << std::endl;

  //Read in file
  // Setup pixel type and input image type
  typedef itk::Image< float,  3 >   ImageType;

  typedef itk::ImageFileReader<ImageType> InputImageType;
  InputImageType::Pointer input = InputImageType::New();
  input->SetFileName( inputImage );
  input->Update();

  ImageType::Pointer image = ImageType::New();
  image = input->GetOutput();

  //Convert image to Slicer3 space
  ImageType::DirectionType imageDir = image->GetDirection( );
  ImageType::PointType origin = image->GetOrigin( );
  ImageType::SpacingType spacing = image->GetSpacing( );

  int dominantAxisRL = itk::Function::Max3(imageDir[0][0],imageDir[1][0],imageDir[2][0]);
  int signRL = itk::Function::Sign(imageDir[dominantAxisRL][0]);
  int dominantAxisAP = itk::Function::Max3(imageDir[0][1],imageDir[1][1],imageDir[2][1]);
  int signAP = itk::Function::Sign(imageDir[dominantAxisAP][1]);
  int dominantAxisSI = itk::Function::Max3(imageDir[0][2],imageDir[1][2],imageDir[2][2]);
  int signSI = itk::Function::Sign(imageDir[dominantAxisSI][2]);

  ImageType::DirectionType DirectionToRAS;
  DirectionToRAS.SetIdentity( );
  //TODO
  /*
    if (signRL == 1)
    {
    DirectionToRAS[dominantAxisRL][dominantAxisRL] = -1.0;
    origin[dominantAxisRL] *= -1.0;
    }
    if (signAP == -1)
    {
    DirectionToRAS[dominantAxisAP][dominantAxisAP] = -1.0;
    origin[dominantAxisAP] *= -1.0;
    }
    if (signSI == 1)
    {
    DirectionToRAS[dominantAxisSI][dominantAxisSI] = -1.0;
    origin[dominantAxisSI] *= -1.0;
    }

    imageDir *= DirectionToRAS;

    image->SetDirection( imageDir  );
    image->SetOrigin( origin );
    image->Update();

    ////Write flipped file out
    //typedef itk::ImageFileWriter< ImageType >  WriterType2;
    //WriterType2::Pointer writer2 = WriterType2::New();
    //writer2->SetFileName( outputImage );
    //writer2->SetInput(image);
    //writer2->Update();

    //VTK Storage
    vtkDoubleArray *xvalues = vtkDoubleArray::New();
    vtkDoubleArray *yvalues = vtkDoubleArray::New();
    vtkDoubleArray *zvalues = vtkDoubleArray::New();
    vtkDoubleArray *intensity = vtkDoubleArray::New();

    //Fiducial File Read in and manipulate
    std::ifstream trialFile;
    std::string trialString;
    trialFile.open( fiducialfile );

    double newvalue = 0;
    int totalfiducials = 0;

    int file_start = 0;

    while(!trialFile.eof())
    {
    std::getline(trialFile, trialString,'#');
    std::getline(trialFile, trialString,'#');
    std::getline(trialFile, trialString,'#');
    std::getline(trialFile, trialString,'#');
    std::getline(trialFile, trialString,'=');
    std::getline(trialFile, trialString,'#');
    totalfiducials = atoi(trialString.c_str());
    trialString.clear();
    std::getline(trialFile, trialString,'?');
    }
    trialFile.close();
    cout<< "totalfiducials: "<<totalfiducials<<endl;

    std::ifstream inputFile;
    std::string inputString;
    inputFile.open( fiducialfile );

    int num_fiducial = 0;

    while(num_fiducial < totalfiducials)
    {
    if(file_start == 0)
    {
    //Junk
    std::getline(inputFile, inputString,',');
    std::getline(inputFile, inputString,',');
    std::getline(inputFile, inputString,',');
    std::getline(inputFile, inputString,',');
    std::getline(inputFile, inputString,',');
    std::getline(inputFile, inputString,',');
    std::getline(inputFile, inputString,',');
    std::getline(inputFile, inputString,',');
    std::getline(inputFile, inputString,',');
    std::getline(inputFile, inputString,',');
    inputString.clear();
    file_start = 1;
    }
    else
    {
    //Junk
    std::getline(inputFile, inputString,',');
    inputString.clear();
    }

    //X-coord
    std::getline(inputFile, inputString,',');
    newvalue = atof(inputString.c_str());
    xvalues->InsertNextValue(newvalue);
    cout<<"x: "<<newvalue<<endl;
    inputString.clear();

    //Y-coord
    std::getline(inputFile, inputString,',');
    newvalue = atof(inputString.c_str());
    yvalues->InsertNextValue(newvalue);
    cout<<"y: "<<newvalue<<endl;
    inputString.clear();

    //Z-coord
    std::getline(inputFile, inputString,',');
    newvalue = atof(inputString.c_str());
    zvalues->InsertNextValue(newvalue);
    cout<<"z: "<<newvalue<<endl;
    inputString.clear();

    std::getline(inputFile, inputString,',');
    std::getline(inputFile, inputString,'\n');
    inputString.clear();

    num_fiducial++;

    }
    inputFile.close();

    int counter = xvalues->GetNumberOfTuples();

    cout<<"spacing: "<<spacing[0]<<" "<<spacing[1]<<" "<<spacing[2]<<endl;
    cout<<"origin: "<<origin[0]<<" "<<origin[1]<<" "<<origin[2]<<endl;

    //itk for pixel intensity
    ImageType::IndexType pixelIndex;
    ImageType::PixelType pixelValue;
    for(int i = 0; i<counter; i++)
    {
    pixelIndex[0] = ((xvalues->GetValue(i))-origin[0])/spacing[0];
    pixelIndex[1] = ((zvalues->GetValue(i))-origin[2])/spacing[2];
    pixelIndex[2] = ((yvalues->GetValue(i))+origin[1])/spacing[1];

    if(pixelIndex[0] < 0)
    {
    pixelIndex[0] = -pixelIndex[0];
    }
    if(pixelIndex[1] < 0)
    {
    pixelIndex[1] = -pixelIndex[1];
    }
    if(pixelIndex[2] < 0)
    {
    pixelIndex[2] = -pixelIndex[2];
    }

    cout<<"pixelindex: "<<pixelIndex[0]<<" "<<pixelIndex[1]<<" "<<pixelIndex[2]<<endl;

    pixelValue = image->GetPixel(pixelIndex);
    cout<<"pixelvalue: "<<pixelValue<<endl;
    intensity->InsertNextValue(pixelValue);
    }

    //Calculate mean
    double intensity_mean = 0;
    for(int i = 0; i<counter; i++)
    {
    intensity_mean += intensity->GetValue(i);
    }
    intensity_mean = intensity_mean/counter;

    cout<<"intensitymean: "<<intensity_mean<<endl;

    //Calculate standard deviation
    double stdev = 0;
    for(int i=0; i<counter; i++)
    {
    stdev = stdev + ((intensity_mean - (intensity->GetValue(i)))*(intensity_mean - (intensity->GetValue(i))));
    }
    stdev = sqrt(stdev/counter);

    int threshvalue = intensity_mean - stdev;

    cout<<"threshvalue: "<<threshvalue<<endl;

    ofstream outfile;
    outfile.open ( logfile, ios::app);
    outfile << "Theshold Value "<< inputImage <<" "<<threshvalue<<" mean "<<intensity_mean<<" stdev "<<stdev<<endl;
    outfile.close();
  */

  typedef itk::ImageFileReader<ImageType> InputImageType2;
  InputImageType2::Pointer input2 = InputImageType2::New();
  input2->SetFileName( inputImage );
  input2->Update();

  typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(input2->GetOutput());
  filter->SetOutsideValue(0);
  filter->SetInsideValue(1);
  //    filter->SetLowerThreshold(threshvalue);
  filter->SetLowerThreshold(150);
  filter->SetUpperThreshold(3000);
  filter->Update();

  //typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleFilterType;
  //RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  //rescaler->SetOutputMinimum(0);
  //rescaler->SetOutputMaximum(1);
  //rescaler->SetInput(filter->GetOutput());

  ////Thresholding code
  //typedef  float  InputPixelType;

  //typedef itk::ThresholdImageFilter<ImageType> FilterType;
  //FilterType::Pointer filter = FilterType::New();
  //filter->SetInput(input2->GetOutput());
  //filter->SetOutsideValue(-1000);
  //filter->ThresholdBelow(threshvalue);
  //filter->Update();

  //Write flipped file out
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImage );
  writer->SetInput(filter->GetOutput());
  writer->Update();

  return EXIT_SUCCESS;
}



int vtkCTHandBoneClass::largest_component( const char* inputImage, const char* outputImage )
{
  std::cerr << "Island Removal and Fill: " << std::endl;

  typedef unsigned char PixelType;
  const int dimension=3;
  typedef itk::Image<PixelType, dimension> ImageType;

  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer fileReader = ReaderType::New();
  fileReader->SetFileName( inputImage );
  fileReader->Update();

  /* Extract Label of Interest */
  typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> BinaryFilterType;
  BinaryFilterType::Pointer extractLabelFilter = BinaryFilterType::New();
  extractLabelFilter->SetInput(fileReader->GetOutput());
  extractLabelFilter->SetLowerThreshold(1);
  extractLabelFilter->SetUpperThreshold(255);
  extractLabelFilter->SetInsideValue(1);
  extractLabelFilter->SetOutsideValue(0);
  extractLabelFilter->Update();

  std::cout << "Extract Label" << std::endl;

  typedef itk::ConnectedComponentImageFilter<ImageType,ImageType,ImageType> ConnectedFilterType;
  ConnectedFilterType::Pointer connectedFilter = ConnectedFilterType::New();
  connectedFilter->SetInput( extractLabelFilter->GetOutput() );
  connectedFilter->Update();

  std::cout << "Connected Component" << std::endl;

  typedef itk::RelabelComponentImageFilter<ImageType,ImageType> RelabelFilterType;
  RelabelFilterType::Pointer relabelFilter = RelabelFilterType::New();
  relabelFilter->SetInput(connectedFilter->GetOutput());
  relabelFilter->Update();

  std::cout << "Relabel Image" << std::endl;

  BinaryFilterType::Pointer binaryFilter = BinaryFilterType::New();
  binaryFilter->SetInput(relabelFilter->GetOutput());
  binaryFilter->SetLowerThreshold(1);
  binaryFilter->SetUpperThreshold(1);
  binaryFilter->SetInsideValue(1);
  binaryFilter->SetOutsideValue(0);
  binaryFilter->Update();

  std::cout << "Binary Threshold" << std::endl;

  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer fileWriter= WriterType::New();
  fileWriter->SetFileName( outputImage );
  fileWriter->SetInput( binaryFilter->GetOutput() );
  fileWriter->Update();

  return EXIT_SUCCESS;
}


//----------------------------------------------------------------------------
void vtkCTHandBoneClass::PrintSelf(ostream& os,vtkIndent indent) {
  /*
    int x,y;
    os << indent << "------------------------------------------ CLASS ----------------------------------------------" << endl;
    this->vtkImageEMGenericClass::PrintSelf(os,indent);  
    os << indent << "ProbDataPtr:             " << this->ProbDataPtr << endl;
    os << indent << "ProbDataIncY:            " << this->ProbDataIncY << endl;
    os << indent << "ProbDataIncZ:            " << this->ProbDataIncZ << endl;

    os << indent << "LogMu:                   ";
    for (x= 0 ; x < this->NumInputImages; x ++) os << this->LogMu[x] << " ";
    os<< endl;

    os << indent << "LogCovariance:           ";
    for (y= 0 ; y < this->NumInputImages; y ++) {
    for (x= 0; x < this->NumInputImages; x++)  os << this->LogCovariance[y][x] << " " ;
    if ( y < (this->NumInputImages-1)) os<< "| ";
    }
    os<< endl;

    os << indent << "ReferenceStandardPtr:    ";
    if (this->ReferenceStandardPtr) {
    os << this->ReferenceStandardPtr << endl;
    } else {os << "(None)" << endl;}

    os << indent << "PrintQuality:            " << this->PrintQuality << endl;
  */
}
