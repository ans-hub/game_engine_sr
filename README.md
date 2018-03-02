# Software renderer and game console

This repository contains implementation of `game console` suggested by Andre Lamothe in the book [Advanced 3D Graphics and Rasterization](http://a.co/6XNNYrD). The main idea of suggested game console was to study different algorithms (graphics, games, etc), using vurtual linear addressed video buffer. This means that all the manipulations with the graphics (math, transformation, rasterisation, etc.) you must perform manually. And only then you will send data to the video memory.

But in this great book we have a game console written for Windows. Thus for those people who likes this book and want to start to write games and who haven't Windows this may became a problem.

`Note: `This console is under development as I read the book

## Linear video buffer implementation

One of the problems that I faced was an implementation of abstract linear addressed video buffer.

In my implementation the linear video buffer is an abstraction consisting of an array in the system memory with a step of 4 bytes to store the pixel color in the format `ARGB (byte-ordered)`. Usually buffer has size is evaluated as *window_width x widnow_height x bytes_per_pixel*.

The linear buffer has the following interface:
- `operator[]` - allows you to access and write to the buffer data
- `Clear()` - Clears the buffer with the specified color
- `SendDataToFB()` - sends data to video memory (used at the end of rasterization)

`SendDataToFB()` implemented throught OpenGL function `glDrawPixels`, which is now deprecated. But this fact is shouldn't be a problem since this function is present anywhere in compatibility opengl profile (or in the versions <= than 3.0).

For systems which has only OpenGL ES implementations we have another way to substitute `glDrawPixels` in core opengl profile (see comments in the end of `/lib/draw/gl_buffer.cc`).

This buffer can be easily reduced to 16 bit or 8 bit color space manually.

## Game console examples

Directory `examples` contains examples based on the game console. Currently there are placed game called `Raiders 3D` which is my implementation of the game originally written by Andre Lamothe.

The main actions is placed in two classes `Logic` and `Scene`. It should be noted that this is an educational implementation, and thus most of the functions which is responsible for the behavior of objects are located in these two classes (`Logic` and `Scene`). Although in a real game it would be better to arrange some of the functions which is responsible for the behavior of objects in the implementation of these objects themselves.

<div style="text-align: center;" markdown="1" />
<img src="screenshot.png" style="width: 600px;" />
</div>
<div style="text-align: center;" markdown="1" />
<img src="screenshot2.png" style="width: 600px;" />
</div>

## Libraries used in game console

`math` - mathematical library, which I start to implement just now

`window` - performs to create window, toggle fullscreen, grab keybord events and mouse events

`draw` - draw functions, which may be used with drawing surface, given by `window` library

`audio` - is a wrapper to BASS audio library

`system` - useful system stuffs, such as timer

`data` - data manipulation classes

## Important notes

When I started writing `math` library, I did not supposed that I would be use it not only for 2d but for 3d calculations too. In this case I should warn those who will clone this repository - interface of `math` library will be changed.

## Install

### Check dependicies:

Following packages should be installed (example given under Ubuntu 17.10):

```bash
 $ sudo apt install mesa-common-dev
 $ sudo apt install mesa-utils
 $ sudo apt install libxrandr-dev
 $ sudo apt install libglu1-mesa-dev
```

### Install `libbass.so` audio library (for audio module):

```bash
  $ wget http://us.un4seen.com/files/bass24-linux.zip
  $ unzip bass24-linux.zip -d tmp/
  $ sudo cp tmp/x64/libbass.so /usr/local/lib/  # for 32-bit - tmp/libbass.so
  $ sudo chmod a+rx /usr/local/lib/libbass.so
  $ sudo ldconfig
  $ rm -rf tmp/ bass24-linux.zip
```

P.S.: if something goes wrong while downloading bass audio library, then the best way is to use [direct link](https://www.un4seen.com/bass.html) for downloading library

### Finish the installation:

```bash
  $ git clone https://github.com/ans-hub/game_console
```

## Notes about installation under cygwin:

This packages should be installed to work with video modes:

* cygutils-x11
* libGLU1
* xrandr
* randrproto
* libxcb-randr0

Also is necessary to add path to environment variable PATH:
```bash
  $ export PATH=/usr/local/lib:$PATH
```

Installation of bass audio library is similar, but in the case of cygwin we should download windows library (`bass.dll`) and place it in `/usr/local/lib/`
