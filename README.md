# d2d
A simple 2D Debugger to visualize 2d primitives.

[![Build Status](https://travis-ci.org/zpgaal/d2d.svg?branch=master)](https://travis-ci.org/zpgaal/d2d)


[![Build status](https://ci.appveyor.com/api/projects/status/4kvy9c05ihxqe9l1?svg=true)](https://ci.appveyor.com/project/zpgaal/d2d)


Programs can attach to the visualizer through multiple channels and send 2d primitives.

## Commands
Commands are provided as a JSon message usually

### Control Commands

#### reset
 ```json
 { "command":"reset" }
 ```
 Reset the visualizer. This command removes all the layers and graphics items and resets the tool the initial state
#### clear
 ```json
 { "command":"clear"
   "layer":layer }
 ```
Clears the given __layer__. If no __layer __ is given or if it set to "*", all the layers are cleared. Note, only the items of the 
selected layers are cleared, but the layers are kept preserving the current visibility settings.
#### cache
 ```json
 { "command":"cache",
   "id": register }
 ```
 Store the current scene in the __register__. Registers are stored as persistent files in the user folder.

### Primitives
All primitives have some common, optional properties:
- color - argb components given as a large (32bit) hexadecimal value. White is assumed if not given
- layer - string identifying the layer. "default" is assumed if not given

#### Point
 ```json
 { "command":"point",
   "color": color,
   "layer": layer,
   "x": coord_x,
   "y": coord_y }
 ```
 Adds a point (a small disc) going at (__coord_x__,__coord_y__) with __color__ color to the __layer__ layer.

#### Line
 ```json
 { "command":"line",
   "color": color,
   "layer": layer,
   "x0": coord_start_x,
   "y0": coord_start_y,
   "x1": coord_end_x,
   "y1": coord_end_y }
 ```
 Adds a line going from (__coord_start_x__,__coord_start_y__) to (__coord_end_x__,__coord_end_y__) with __color__ color to the __layer__ layer.
 
#### Rectangle
 ```json
 { "command":"rect",
   "color": color,
   "layer": layer,
   "x0": coord_tl_x,
   "y0": coord_tl_y,
   "x1": coord_br_x,
   "y1": coord_br_y }
 ```
 Adds a (non-filled) rectangle connecting the two diagonal vertex  (__coord_tl_x__,__coord_tl_y__), (__coord_br_x__,__coord_br_y__) with __color__ color to the __layer__ layer.
 
#### Polygon
 ```json
 { "command":"poly",
   "color": color,
   "layer": layer,
   "x": coords_x,
   "y": coords_y }
 ```
 Adds a (filled) polygon. The x(y) coordinates of the contour vertices of the polygon are given in the __coords_x__ (__coords_y__) array. The size of x,y shall be identical and shall contain at least 3 vertex.

#### Poly-line
 ```json
 { "command":"polyline",
   "color": color,
   "layer": layer,
   "x": coords_x,
   "y": coords_y }
 ```
 Adds a line strip. The x(y) coordinates of the vertices of the line lisr are given in the __coords_x__ (__coords_y__) array. The size of x,y shall be identical and shall contain at least 3 vertex.
 
#### Text
 ```json
 { "command":"text",
   "color": color,
   "layer": layer,
   "x": coord_tl_x,
   "y": coord_tl_y,
   "text": text }
 ```
 Adds a text to the (__coord_tl_x__,__coord_tl_y__) position. A point (a small disk) marks the location and the __text__ text is placed to the right. When multiple texts are placed to the same location they are not drawn over each other, but written one under the other aligned to the given position.
 
## Channels

### Tcp channels
D2D is also a tcp server. Multiple program may send data the server. Each packet is parsed as a string and may contain any number of json messages and thus a packet is not a well-formed JSon message. There is no requirement to enclose the whole string (actually it is not supported) in a top-level block. It is enough to concatenate the commands into a large packet and send it at once. (Or one may send the commands one-by-one).

For now, it cannot be configured and accepts connections on the 1234 port (set in the constructor of Application).

### Window native events
Messages are acquired by the WM_COPYDATA native event. COPYDATASTRUCT::dwData shall be set to 8 and lpData to a zero terminated string containing the json message. Note there are some "immediate" commands through which JSon parsing can be eliminated, but less general. For more information check the source.

## Extensibility
The tool is meant to be simplistic without too much concern about "general" factory infrastructures. 
To add new commands, the source shall be modified:
 - extend the Cmd* enums.
 - derive a class from SceneManager::Command 
 - write a static parse function for your class to create the command from JSon object
 - add the parser function to the SceneManager::addCommand
 - write a static save function for your class to support the save/load into register
 - add the save function to SceneManager::save

For input handling there is no factory framework either, to add a new Input class:
 - name it Input* and let it have a SceneManager reference
 - add it the the Application as a member
 - initialize it in the Application::initInputs function
 - to add new commands call SceneManager::addCommand

