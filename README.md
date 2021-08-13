## Standard I/O addon
This addon is a QProcess wrapper for [ossia score](https://ossia.io).
It enables users to create QML devices in wich external programs can be launched.
Running programs can then communicate with the device through the standard input and output. The standard error output is printed in score's message log window.

# Usage
The provided [ping.qml](ping.qml) example uses the ```write:``` atribute to pass startup  arguments to ```ping```, the default program. Additional startup arguments can also be added after the programs's path in the ```Program``` field. Arguments must be separated by a space. The selected program will launch at the creation of the device. If it finishes, it will be launched again when setting a parameter of the device to a new value.
