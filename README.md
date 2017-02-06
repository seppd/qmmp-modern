# qmmp-modern
### Building and installing: 
`git clone https://github.com/dsmorozov/qmmp-modern.git` 

`qmake` 

`make`  

`sudo make install`

Go to Qmmp settings -> Plugins -> User Interfaces -> Select "Modern User Interface" -> Close -> Restart the program

or

find "current_plugin" config entry in qmmprc and set it to "modern"

The latter is the only option to switch back to "skinned" plugin for now.


### Dependencies:
- C++11
- Qmmp
- Qt5 >= 5.4
- Qt5X11Extras
- unzip

### More or less implemented:
- XML parser
- Basic GUI elements(e.g. button, slider, text)
- Basic functionality (Play, Stop, Pause, Eject/File Open)
- MAKI script loader
- MAKI bytecode interpreter

One can try [this skin](https://www.dropbox.com/s/nl6bd3ufcalo6mf/Tutorial4.wal?dl=0) to see what has been implemented so far.

### Debug
`QT_LOGGING_RULES="modernui.xml.parser.debug=true" qmmp` enables skin xml parser debug
`QT_LOGGING_RULES="modernui.script.loader.debug=true" qmmp` enables script loader debug
`QT_LOGGING_RULES="modernui.script.interpreter.debug=true" qmmp` enables bytecode interpreter debug

### Appendix:
Huge amount of things still remains to be implemented. Don't even try loading any complex skin.
