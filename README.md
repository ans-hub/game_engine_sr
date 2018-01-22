# Game console

This repository contains implementation of `game console` suggested by Andre Lamothe in the book [Advanced 3D Graphics and Rasterization](http://a.co/6XNNYrD). The main idea of suggested game console was to study different algorithms (graphics, games, etc), using vurtual linear addressed video buffer. This means that all the manipulations with the graphics (math, transformation, rasterisation, etc.) you must perform manually. And only then you will send data to the video memory.

But in this great book we have a game console written for Windows. Thus for those people who likes this book and want to start to write games and who haven't Windows this may became a problem.

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

Directory `games` contains games (games) based on the game console. Currently there are placed game called `Raiders 3D` which is my implementation of the game originally written by Andre Lamothe.

The main actions is placed in two classes `Logic` and `Scene`. It should be noted that this is an educational implementation, and thus most of the functions which is responsible for the behavior of objects are located in these two classes (`Logic` and `Scene`). Although in a real game it would be better to arrange some of the functions which is responsible for the behavior of objects in the implementation of these objects themselves.

## Libraries used in game console

`math` - mathematical library, which I start to implement just now

`window` - performs to create window, toggle fullscreen, grab keybord events and mouse events

`draw` - draw functions, which may be used with drawing surface, given by `window` library

`audio` - is a wrapper to BASS audio library

`system` - useful system stuffs, such as timer

## Important notes

When I started writing `math` library, I did not supposed that I would be use it not only for 2d but for 3d calculations too. In this case I should warn those who will clone this repository - interface of `math` library will be changed.

## Install

```bash
  $ git clone https://github.com/ans-hub/game_console
  $ cd games/raiders3d/
  $ make
```