#ifndef vtkCudaVolumeInformationHandler_H_
#define vtkCudaVolumeInformationHandler_H_

#include "vtkObject.h"
#include "vtkCUDA.h"


#include "cudaVolumeInformation.h"
#include "CudappHostMemory.h"
#include "CudappDeviceMemory.h"

class vtkVolume;
class vtkVolumeProperty;
class vtkImageData;
class vtkMatrix4x4;

class VTK_CUDA_EXPORT vtkCudaVolumeInformationHandler : public vtkObject
{
public:
    static vtkCudaVolumeInformationHandler* New();
    vtkTypeRevisionMacro(vtkCudaVolumeInformationHandler, vtkObject);
    virtual void PrintSelf(ostream& os, vtkIndent indent);

    vtkGetObjectMacro(Volume, vtkVolume);
    void SetVolume(vtkVolume* Volume);
    void SetInputData(vtkImageData* inputData);
    
    //BTX
    //const cudaVolumeInformation& GetVolumeInfo() const { return VolumeInfo; }
    cudaVolumeInformation& GetVolumeInfo() { return VolumeInfo; }
    //ETX

    void SetThreshold(float min, float max);
    void SetSampleDistance(float sampleDistance);
    void SetOrientationMatrix(vtkMatrix4x4* matrix);
    void SetTransformationMatrix(vtkMatrix4x4* matrix);

    float GetSampleDistance();

    void ResizeTransferFunction(unsigned int size);
    virtual void Update();

protected:
    vtkCudaVolumeInformationHandler();
    ~vtkCudaVolumeInformationHandler();

    void UpdateVolumeProperties(vtkVolumeProperty *property);
    void UpdateVolume();
    void UpdateImageData();
    void UpdateMatrix();
    void UpdateGradient();


private:
    vtkCudaVolumeInformationHandler& operator=(const vtkCudaVolumeInformationHandler&); // not implemented
    vtkCudaVolumeInformationHandler(const vtkCudaVolumeInformationHandler&); // not implemented


private:
    vtkImageData*           InputData;
    vtkVolume*              Volume;
    vtkMatrix4x4* TransformationMatrix;
    vtkMatrix4x4* OrientationMatrix;

    //BTX
    cudaVolumeInformation  VolumeInfo;

    Cudapp::DeviceMemory    CudaInputBuffer;
    Cudapp::DeviceMemory    GradientBuffer;

    Cudapp::HostMemory      LocalColorTransferFunction;
    Cudapp::DeviceMemory    CudaColorTransferFunction;
    Cudapp::HostMemory      LocalAlphaTransferFunction;
    Cudapp::DeviceMemory    CudaAlphaTransferFunction;
    Cudapp::HostMemory      LocalGradientTransferFunction;
    Cudapp::DeviceMemory    CudaGradientTransferFunction;

    //ETX
};

#endif /* vtkCudaVolumeInformationHandler_H_ */
