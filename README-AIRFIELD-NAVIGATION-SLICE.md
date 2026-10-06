# Airfield navigation slice

Adds the Airfield to the existing VM-first shell navigation:

```text
Airfield button
    -> NavigateToAirfieldCommand
    -> MainViewModel.SelectedViewModel
    -> AirfieldViewModel
    -> AirfieldAwareViewTemplateSelector
    -> AirfieldView
```

Important boundaries:

- no clickable taxi nodes yet
- no route execution yet
- no aircraft movement yet
- no Rx stream yet
- existing Tower/Trackings selector source is not modified
- the existing 54 native/domain tests should remain unchanged

The AirfieldViewModel is now a projected C++/WinRT runtimeclass so it can
participate in the same `IInspectable` / DataTemplateSelector pipeline as
TowerViewModel and TrackingsViewModel.
