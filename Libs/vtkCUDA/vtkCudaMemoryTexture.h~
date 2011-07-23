#ifndef __VTKCUDAMEMORYTEXTURE_H__
#define __VTKCUDAMEMORYTEXTURE_H__

#include "vtkObject.h"
#include "vtkCUDA.h"

class vtkImageData;

//BTX
#include "CudappDeviceMemory.h"
#include "CudappHostMemory.h"
//namespace Cudapp {
//    DeviceMemory;
//    HostMemory;}
//ETX
class VTK_CUDA_EXPORT vtkCudaMemoryTexture : public vtkObject
{
    vtkTypeRevisionMacro(vtkCudaMemoryTexture, vtkObject);
public:
    static vtkCudaMemoryTexture* New();

    void SetWidth(unsigned int width) { this->SetSize(width, this->GetHeight()); }
    void SetHeight(unsigned int height) { this->SetSize(this->GetWidth(), height); }
    void SetSize(unsigned int width, unsigned int height);

    unsigned int GetWidth() const { return this->Width; }
    unsigned int GetHeight() const { return this->Height; }

    unsigned int GetTexture() const { return this->TextureID; }

    void BindTexture();
    void BindBuffer();
    //BTX
    unsigned char* GetRenderDestination() const { return this->RenderDestination; }
    //ETX
    void UnbindBuffer();
    void UnbindTexture();

    bool CopyToVtkImageData(vtkImageData* data);

    //BTX
    typedef enum 
    {
        RenderToTexture = 0,
        RenderToMemory
    } RenderMode;
    //ETX
    void SetRenderMode(int mode);
    void SetRenderModeToRenderToTexture(){ return this->SetRenderMode(0); };
    void SetRenderModeToRenderToMemory(){ return this->SetRenderMode(1); };
    int GetCurrentRenderMode() const { return this->CurrentRenderMode; }


protected:
    vtkCudaMemoryTexture();
    ~vtkCudaMemoryTexture();

private:
    void Initialize();
    void RebuildBuffer();

private:
    unsigned char* RenderDestination;

    unsigned int TextureID;
    unsigned int BufferObjectID;

    unsigned int Width;
    unsigned int Height;

    int CurrentRenderMode;

    //BTX
    Cudapp::DeviceMemory    CudaOutputData;
    Cudapp::HostMemory      LocalOutputData;
    //ETX

    static bool  GLBufferObjectsAvailiable;
};
#endif /* __VTKCUDAMEMORYTEXTURE_H__ */
