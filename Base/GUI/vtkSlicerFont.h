#ifndef __vtkSlicerFont_h
#define __vtkSlicerFont_h

#include <vector>
#include <string>

#include "vtkObject.h"
#include "vtkSlicerBaseGUIWin32Header.h" 

class VTK_SLICER_BASE_GUI_EXPORT vtkSlicerFont : public vtkObject
{
  public:
  static vtkSlicerFont* New();
  vtkTypeRevisionMacro(vtkSlicerFont, vtkObject);

  /// 
  /// Get methods for Slicer's Font options
  vtkGetMacro(FontSizeSmall0, int);
  vtkGetMacro(FontSizeSmall1, int);
  vtkGetMacro(FontSizeSmall2, int);
  vtkGetMacro(FontSizeMedium0, int);
  vtkGetMacro(FontSizeMedium1, int);
  vtkGetMacro(FontSizeMedium2, int);
  vtkGetMacro(FontSizeLarge0, int);
  vtkGetMacro(FontSizeLarge1, int);
  vtkGetMacro(FontSizeLarge2, int);
  vtkGetMacro(FontSizeLargest0, int);
  vtkGetMacro(FontSizeLargest1, int);
  vtkGetMacro(FontSizeLargest2, int);

  virtual int GetFontSize0(const char *size);
  virtual int GetFontSize1(const char *size);
  virtual int GetFontSize2(const char *size);

  virtual int IsValidFontFamily(const char *str);
  virtual int GetNumberOfFontFamilies();
  const char *GetFontAsFamily(const char *fontstring);
  const char *GetFontFamily(int i)
  {
    return (FontFamilies[i].c_str());
  }

  virtual int IsValidFontSize(const char *str);
  virtual int GetNumberOfFontSizes();
  const char *GetFontSize(int i)
  {
    return (FontSizes[i].c_str());
  }

  protected:
    
  vtkSlicerFont();
  ~vtkSlicerFont();

  /// try this...
  //BTX
  std::vector <std::string> FontFamilies;
  std::vector <std::string> FontSizes;
  //ETX
    
  int FontSizeSmall0;
  int FontSizeSmall1;
  int FontSizeSmall2;
  int FontSizeMedium0;
  int FontSizeMedium1;
  int FontSizeMedium2;
  int FontSizeLarge0;
  int FontSizeLarge1;
  int FontSizeLarge2;
  int FontSizeLargest0;
  int FontSizeLargest1;
  int FontSizeLargest2;
        
  private:
  vtkSlicerFont(const vtkSlicerFont&); /// Not implemented
  void operator =(const vtkSlicerFont&); /// Not implemented
};

#endif
