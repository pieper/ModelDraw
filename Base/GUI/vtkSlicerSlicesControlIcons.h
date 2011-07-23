#ifndef __vtkSlicerSlicesControlIcons_h
#define __vtkSlicerSlicesControlIcons_h

#include "vtkKWObject.h"
#include "vtkKWResourceUtilities.h"
#include "vtkKWIcon.h"
#include "vtkSlicerIcons.h"
#include "./Resources/vtkSlicerSlicesControl_ImageData.h"

/// Description:
/// This class reads in the png format image data for logos and
/// for icons in Slicer's main toolbar, and elsewhere in
/// slicer's main window. It also creates vtkKWIcons and assigns
/// the image data to them. (Elsewhere in the application,
/// vtkKWLabels and vtkKWPushbuttons can display these
/// vtkKWIcons instead of text.

class VTK_SLICER_BASE_GUI_EXPORT vtkSlicerSlicesControlIcons : public vtkSlicerIcons
{
 public:
    /// 
    /// Usual vtk functions
    static vtkSlicerSlicesControlIcons* New ();
    vtkTypeRevisionMacro ( vtkSlicerSlicesControlIcons, vtkSlicerIcons );
    void PrintSelf ( ostream& os, vtkIndent indent );

    vtkGetObjectMacro ( ManualSliceSpacingIcon, vtkKWIcon );
    vtkGetObjectMacro ( ManualSliceSpacingModeIcon, vtkKWIcon );
    vtkGetObjectMacro ( AutomaticSliceSpacingIcon, vtkKWIcon );
    vtkGetObjectMacro ( AutomaticSliceSpacingModeIcon, vtkKWIcon );
    vtkGetObjectMacro ( FgIcon, vtkKWIcon );
    vtkGetObjectMacro ( BgIcon, vtkKWIcon );
    vtkGetObjectMacro ( ToggleFgBgIcon, vtkKWIcon );
    vtkGetObjectMacro ( LabelOpacityIcon, vtkKWIcon );
    vtkGetObjectMacro ( AllLabelOpacityIcon, vtkKWIcon );
    vtkGetObjectMacro ( LinkControlsIcon, vtkKWIcon );
    vtkGetObjectMacro ( UnlinkControlsIcon, vtkKWIcon );
    vtkGetObjectMacro ( InterpolationOnIcon, vtkKWIcon );
    vtkGetObjectMacro ( InterpolationOffIcon, vtkKWIcon );
    vtkGetObjectMacro ( GridIcon, vtkKWIcon );
    vtkGetObjectMacro ( AnnotationIcon, vtkKWIcon );
    vtkGetObjectMacro ( SpatialUnitsIcon, vtkKWIcon );
    vtkGetObjectMacro ( CrossHairIcon, vtkKWIcon );
    vtkGetObjectMacro (SetFgIcon, vtkKWIcon );
    vtkGetObjectMacro (SetBgIcon, vtkKWIcon );
    vtkGetObjectMacro (SetLbIcon, vtkKWIcon );
    vtkGetObjectMacro (SetOrIcon, vtkKWIcon );
    vtkGetObjectMacro ( FitToWindowIcon, vtkKWIcon );
    vtkGetObjectMacro ( FeaturesVisibleIcon, vtkKWIcon );
    vtkGetObjectMacro (WinLevThreshColIcon, vtkKWIcon);
    vtkGetObjectMacro (FieldOfViewIcon, vtkKWIcon );
    vtkGetObjectMacro (SliceMoreOptionsIcon, vtkKWIcon );
    vtkGetObjectMacro (SliceWidgetOnIcon, vtkKWIcon );
    vtkGetObjectMacro ( SliceWidgetOffIcon, vtkKWIcon );
    vtkGetObjectMacro (SliceLabelOutlineOnIcon, vtkKWIcon );
    vtkGetObjectMacro ( SliceLabelOutlineOffIcon, vtkKWIcon );

    /// 
    /// Method for setting images for widgets in Slicer's SlicesControlFrame
    void AssignImageDataToIcons ( );
    
 protected:
    vtkSlicerSlicesControlIcons ( );
    virtual ~vtkSlicerSlicesControlIcons ( );

    vtkKWIcon *ManualSliceSpacingIcon;
    vtkKWIcon *AutomaticSliceSpacingIcon;
    vtkKWIcon *ManualSliceSpacingModeIcon;
    vtkKWIcon *AutomaticSliceSpacingModeIcon;
    vtkKWIcon *SliceMoreOptionsIcon;
    vtkKWIcon *SliceWidgetOnIcon;
    vtkKWIcon *SliceWidgetOffIcon;
    vtkKWIcon *SliceLabelOutlineOnIcon;
    vtkKWIcon *SliceLabelOutlineOffIcon;   
    vtkKWIcon *FgIcon;
    vtkKWIcon *BgIcon;
    vtkKWIcon *ToggleFgBgIcon;
    vtkKWIcon *AllLabelOpacityIcon;
    vtkKWIcon *LabelOpacityIcon;
    vtkKWIcon *LinkControlsIcon;
    vtkKWIcon *UnlinkControlsIcon;
    vtkKWIcon *InterpolationOnIcon;
    vtkKWIcon *InterpolationOffIcon;
    vtkKWIcon *AnnotationIcon;
    vtkKWIcon *SpatialUnitsIcon;
    vtkKWIcon *CrossHairIcon;
    vtkKWIcon *GridIcon;
    vtkKWIcon *SetFgIcon;
    vtkKWIcon *SetBgIcon;
    vtkKWIcon *SetLbIcon;
    vtkKWIcon *SetOrIcon;
    vtkKWIcon *FitToWindowIcon;
    vtkKWIcon *FeaturesVisibleIcon;
    vtkKWIcon *WinLevThreshColIcon;
    vtkKWIcon *FieldOfViewIcon;

 private:
    vtkSlicerSlicesControlIcons (const vtkSlicerSlicesControlIcons& ); /// Not implemented
    void operator = (const vtkSlicerSlicesControlIcons& ); /// Not implemented
    
};
#endif
