# micrö

A tiny framework for making 2D games with chunky pixels in Teascript.

This is a rework of [rxi's Juno](https://github.com/rxi/juno) framework, adapted to work with Teascript and with some overall changes in the library.

## Building

Micro's main dependencies are [GLFW](https://github.com/glfw/glfw) for OS window and input and [Glew](https://github.com/nigels-com/glew) for OpenGL extension loading. You'll also need make and gcc.

Make sure to have the latest commit of [Teascript](https://github.com/RevengerWizard/teascript) compiled.

You may need to modify the Makefile to point to the correct library paths used by your system.

```bash
git clone https://github.com/RevengerWizard/micro && cd micro
make
```

## License

Licenced under MIT License. [Copy of the license can be found here](https://github.com/RevengerWizard/micro/blob/master/LICENSE)