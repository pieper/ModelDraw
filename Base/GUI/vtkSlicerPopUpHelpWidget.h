#ifndef __vtkSlicerPopUpHelpWidget_h
#define __vtkSlicerPopUpHelpWidget_h

#include "vtkSlicerWidget.h"
#include "vtkSlicerHelpIcons.h"

#include "vtkKWPushButton.h"
#include "vtkKWTopLevel.h"
#include "vtkKWTextWithScrollbars.h"
#include "vtkKWLabel.h"


class VTK_SLICER_BASE_GUI_EXPORT vtkSlicerPopUpHelpWidget : public vtkSlicerWidget
{
  
public:
  static vtkSlicerPopUpHelpWidget* New();
  vtkTypeRevisionMacro(vtkSlicerPopUpHelpWidget,vtkSlicerWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// 
  /// Getting the button to open  the help window
  vtkGetObjectMacro ( HelpButton, vtkKWPushButton );

  /// 
  /// Getting the button to close the help window
  vtkGetObjectMacro ( CloseButton, vtkKWPushButton );

  /// 
  /// Getting the window in which to display help text.
  vtkGetObjectMacro ( HelpWindow, vtkKWTopLevel );

  /// 
  /// Getting the icons for help widgets
  vtkGetObjectMacro ( HelpIcons, vtkSlicerHelpIcons );
  
  /// 
  /// Getting the text widget that displays
  /// the helpstring.
  vtkGetObjectMacro (HelpText, vtkKWTextWithScrollbars );

  /// Descriptoin:
  /// Getting the label that contains the title
  vtkGetObjectMacro (HelpTitle, vtkKWLabel );
  
  /// 
  /// Use this method to set the help or information text.
  /// Only simple formatting is now possible, for instance:
  /// **bold** and
  //__underline__ and
  //~~italic~~
  /// and that's about it.
  void SetHelpText ( const char *text );

  /// 
  /// Use this method to set the text to appear in the title
  /// of the help window. Best to use something brief, less
  /// than 30 chars, for instance:
  /// "Choosing a cutoff frequency"
  void SetHelpTitle ( const char *title );


  /// 
  /// alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessWidgetEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  /// 
  /// removes observers on widgets in the class
  virtual void RemoveWidgetObservers ( );

  /// 
  /// adds observers on widgets in the class
  virtual void AddWidgetObservers ( );

  /// 
  /// Internal Callbacks. do not use.
  void DisplayHelpWindow ( );
  void WithdrawHelpWindow ( );
  
 protected:
  vtkSlicerPopUpHelpWidget();
  virtual ~vtkSlicerPopUpHelpWidget();

  /// 
  /// Create the widget.
  virtual void CreateWidget();


  void Bind ( );
  void UnBind ( );

  vtkKWTopLevel *HelpWindow;
  vtkKWPushButton *HelpButton;
  vtkKWPushButton *CloseButton;
  vtkSlicerHelpIcons *HelpIcons;
  vtkKWTextWithScrollbars *HelpText;
  vtkKWLabel *HelpTitle;
  
private:

  vtkSlicerPopUpHelpWidget(const vtkSlicerPopUpHelpWidget&); /// Not implemented
  void operator=(const vtkSlicerPopUpHelpWidget&); /// Not Implemented
};

#endif

