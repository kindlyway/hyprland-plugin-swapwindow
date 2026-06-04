# hyprland-plugin-swapwindow
Adds an extra dispatcher to swap 2 windows based on address in hyprland

## Installation

Run ```hyprpm update```, then ```hyprpm add https://github.com/kindlyway/hyprland-plugin-swapwindow```, and lastly ```hyprpm enable swapwindowv2```

## Using the plugin

# Hyprlang

To use the plugin, run ```hyprctl dispatch plugin:swapwindowv2:swapwindowv2 address:0x12345678,address:0x12345678```, replacing the addresses with the address you want to target, future support for other regexes will be added, currently only addresses are supported and process ids are supported, using any other regex will produce unintended results

# Lua

To use the plugin on lua, run ```hyprctl dispatch "hl.plugin.swapwindow.swap('address:0x12345678','address:0x12345678')"```

## Additional notes

This plugin also adds a "swapwindowv2" to the IPC, meaning there will be an extra IPC message if you use this plugin

## Contributing

If you find an error with this plugin, or want to help with some other features, please open up a issue or pull request :)
