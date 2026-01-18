Tweaks for the third person camera in TLOZ: The Wind Waker HD. 
For [Cemu](https://cemu.info/) and for [Aroma](https://aroma.foryour.cafe/).

[GBAtemp thread](https://gbatemp.net/threads/cemu-aroma-tloz-the-wind-waker-hd-invert-3rd-person-camera-y-axis-patch.678754/#post-10799346), and [showcase video](https://youtu.be/bAeP5r48RNA) 

# Patch list
- Invert Y axis (everywhere)
- Consistent Y axis: prevents the game from inverting the Y axis in certain locations

# Download and instructions
Downloads are available in the [releases](https://github.com/derivativeoflog7/WWHD_invert3pYaxis/releases/latest) page, with usage instructions being present for both platforms in the downloads, or in this repo in their respective folder.  

# Credits 
The Aroma plugin uses code from [Inkay](https://github.com/PretendoNetwork/Inkay) (specifically, for finding the address and patching the instruction) and from the [WUPS example plugin](https://github.com/wiiu-env/WiiUPluginSystem/tree/main/plugins/example_plugin) (settings handling, logging, makefile).    
Many thanks to the Cemu team, as the debugging tools were key for me to figure this out; Maschell and techmuse for pointing me in the right direction for creating the plugin; Maschell again for helping me figure out how to use libfunctionpatcher for this; and ComradeWave for helping me out.
