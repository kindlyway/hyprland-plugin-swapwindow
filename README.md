# hyprland-plugin-swapwindow
Adds an extra dispatcher to swap 2 windows based on address in hyprland

## Installation

Run ```hyprpm update```, then ```hyprpm add https://github.com/kindlyway/hyprland-plugin-swapwindow```, and lastly ```hyprpm enable hyprland-plugin-swapwindow```

## Using the plugin

To use the plugin, run ```hyprctl dispatch plugin:swapwindowv2:swapwindowv2 address:0x12345678,address:0x12345678```, replacing the addresses with the address you want to target, future support for other regexes will be added, currently only addresses are supported

## Additional notes

This plugin also adds a "swapwindowv2" to the IPC, meaning there will be an extra IPC message if you use this plugin
