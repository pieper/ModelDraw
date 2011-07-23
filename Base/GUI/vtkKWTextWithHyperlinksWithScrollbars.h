#ifndef __vtkKWTextWithHyperlinksWithScrollbars_h
#define __vtkKWTextWithHyperlinksWithScrollbars_h


#include "vtkKWTextWithScrollbars.h"
#include "vtkSlicerBaseGUI.h"

class VTK_SLICER_BASE_GUI_EXPORT vtkKWTextWithHyperlinksWithScrollbars : public vtkKWTextWithScrollbars
{
public:

        static vtkKWTextWithHyperlinksWithScrollbars *New();
    vtkTypeRevisionMacro(vtkKWTextWithHyperlinksWithScrollbars, vtkKWTextWithScrollbars);
    virtual void CreateWidget(void);
    virtual void SetText(const char *s);
    virtual void ClickLink(int x, int y);
        void OpenLink(const char *url);

protected:
    vtkKWTextWithHyperlinksWithScrollbars(void);
    ~vtkKWTextWithHyperlinksWithScrollbars(void);



private:
     /// 
    /// Caution: Not implemented
    vtkKWTextWithHyperlinksWithScrollbars(const vtkKWTextWithHyperlinksWithScrollbars&); /// Not implemented
    void operator=(const vtkKWTextWithHyperlinksWithScrollbars&); /// Not implemented
};

#endif
