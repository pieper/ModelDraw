

[[lindex [vtkSlicerViewerWidget ListInstances] 1] GetViewNode] SetBoxVisible 0
[lindex [vtkSlicerViewerWidget ListInstances] 1] UpdateAxis

foreach v [vtkSlicerViewerWidget ListInstances] {puts [[$v GetViewNode] GetBoxVisible]}

foreach v [vtkSlicerViewerWidget ListInstances] {puts "$v [$v GetViewNode]"}
foreach v [vtkSlicerViewerWidget ListInstances] {puts "$v [$v GetCameraNode]"}

foreach v [vtkMRMLCameraNode ListInstances] {puts "$v [$v GetActiveTag]"}

