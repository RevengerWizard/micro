# micro Teascript API

This reference covers the public Teascript API exposed by micro.

## `micro`

- `micro.run() : integer`
  Main framework loop, returns `1` on exit.

- `micro.errorhandler(msg : string, stacktrace : string)`
  Called once on runtime failure.

- `micro.load()`
  Called once after initialization, before the first frame.

- `micro.update(dt : number)`
  Called once per frame before drawing.
  - `dt`: Delta time for the current frame.

- `micro.draw()`
  Called once per frame after `micro.update()`.

- `micro.quit()`
  Called when the application is shutting down.

- `micro.keydown(key : string)`
  Called when a key is pressed.
  - `key`: Normalized key name such as `"a"`, `"space"`, or `"escape"`.

- `micro.keyup(key : string)`
  Called when a key is released.
  - `key`: Normalized key name.

- `micro.mousemove(x : number, y : number)`
  Called when the pointer moves.
  - `x`: Mouse X position.
  - `y`: Mouse Y position.

- `micro.mousedown(button : string)`
  Called when a mouse button is pressed.
  - `button`: Normalized button name such as `"left"` or `"right"`.

- `micro.mouseup(button : string)`
  Called when a mouse button is released.
  - `button`: Normalized button name.

- `micro.textinput(text : string)`
  Called when text is entered through the platform input system.
  - `text`: UTF-8 text payload.

## `micro.event`

- `micro.event.poll() -> list`
  Polls the OS event queue and returns a list of event maps.
  Event maps may contain `type`, `x`, `y`, `button`, `key`, or `text`, depending on the event.

- `micro.event.pump()`
  Processes pending platform events.

- `micro.event.quit()`
  Requests the window to close and routes control through the quit handler path.

## `micro.timer`

- `micro.timer.step()`
  Updates the internal frame timer bookkeeping.

- `micro.timer.getDelta() -> number`
  Returns the time elapsed since the previous frame.

- `micro.timer.getAverage() -> number`
  Returns the moving average frame time.

- `micro.timer.getFps() -> integer`
  Returns the estimated frames per second, rounded to the nearest integer.

## `micro.keyboard`

- `micro.keyboard.reset()`
  Clears the pressed-key state for the current frame.

- `micro.keyboard.isDown(...keys) -> bool`
  Returns `true` if any supplied key is currently held down.
  - `keys`: One or more normalized key names.

- `micro.keyboard.wasPressed(...keys) -> bool`
  Returns `true` if any supplied key was pressed since the last reset.
  - `keys`: One or more normalized key names.

## `micro.mouse`

- `micro.mouse.reset()`
  Clears the pressed-button state for the current frame.

- `micro.mouse.isDown(...buttons) -> bool`
  Returns `true` if any supplied button is currently held down.
  - `buttons`: One or more normalized button names.

- `micro.mouse.wasPressed(...buttons) -> bool`
  Returns `true` if any supplied button was pressed since the last reset.
  - `buttons`: One or more normalized button names.

- `micro.mouse.getPosition() -> list`
  Returns `[x, y]` for the current mouse position.

- `micro.mouse.getX() -> number`
  Returns the current mouse X coordinate.

- `micro.mouse.getY() -> number`
  Returns the current mouse Y coordinate.

## `micro.window`

- `micro.window.setSize(width : integer, height : integer)`
  Sets the native window size in pixels.
  - `width`: New window width.
  - `height`: New window height.

- `micro.window.setTitle(title : string)`
  Sets the native window title.
  - `title`: Window caption text.

- `micro.window.getTitle() -> string`
  Returns the current native window title.

- `micro.window.setFullscreen(enabled : bool)`
  Toggles fullscreen mode.
  - `enabled`: `true` to enable fullscreen, `false` to restore windowed mode.

## `micro.system`

- `micro.system.info(option : string) -> string`
  Returns a platform-specific path or directory string.
  - `option`: Currently supports `"exedir"` and `"appdata"`.

- `micro.system.getClipboard() -> string`
  Returns the current clipboard text.

- `micro.system.setClipboard(text : string)`
  Sets the clipboard text.
  - `text`: UTF-8 text to place on the clipboard.

## `micro.fs`

- `micro.fs.mount(path : string) -> bool`
  Mounts a filesystem directory or package path.
  - `path`: Directory or mount path.

- `micro.fs.unmount(path : string)`
  Unmounts a previously mounted path.

- `micro.fs.setWritePath(path : string)`
  Sets the writable filesystem root.
  - `path`: Directory to use for writes.

- `micro.fs.read(path : string) -> string`
  Reads the full contents of a file.
  - `path`: File path relative to the mounted filesystem.

- `micro.fs.exists(path : string) -> bool`
  Returns `true` when the path exists.

- `micro.fs.write(path : string, data : string)`
  Writes a file, replacing any existing contents.
  - `path`: File path.
  - `data`: Byte string to write.

- `micro.fs.delete(path : string) -> bool`
  Deletes a file or directory entry.
  Returns `true` when the delete succeeds.

- `micro.fs.getSize(path : string) -> number`
  Returns the file size in bytes.

- `micro.fs.getModified(path : string) -> number`
  Returns the last modified timestamp as a numeric value.

- `micro.fs.isDir(path : string) -> bool`
  Returns `true` if the path points to a directory.

- `micro.fs.listDir(path : string) -> list`
  Returns a list of file and directory names contained in the given directory.

- `micro.fs.append(path : string, data : string)`
  Appends data to a file.
  - `path`: File path.
  - `data`: Byte string to append.

- `micro.fs.makeDirs(path : string)`
  Creates a directory tree if it does not already exist.

## `micro.gfx`

- `micro.gfx.init(width : integer, height : integer)`
  Initializes the drawing surface and binds it to `micro.gfx.screen`.
  - `width`: Logical canvas width.
  - `height`: Logical canvas height.

- `micro.gfx.screen : micro.Image`
  The backing screen image used by the renderer.

- `micro.gfx.setAlpha(alpha : integer)`
  Sets the current alpha multiplier used by the renderer.
  - `alpha`: Alpha value from `0` to `255`.

- `micro.gfx.setBlend(mode : string)`
  Selects the blend mode.
  - `mode`: One of `"alpha"`, `"color"`, `"add"`, `"subtract"`, `"multiply"`, `"lighten"`, `"darken"`, `"screen"`, or `"difference"`.

- `micro.gfx.setColor(r, g, b, a)`
  Sets the current draw color.
  - `r`: Red channel.
  - `g`: Green channel.
  - `b`: Blue channel.
  - `a`: Alpha channel.

- `micro.gfx.setMaxFps(fps : number)`
  Sets the target maximum frame rate.
  - `fps`: Target frames per second.

- `micro.gfx.clear(r, g, b, a)`
  Clears the screen to the given color.
  - `r`: Red channel.
  - `g`: Green channel.
  - `b`: Blue channel.
  - `a`: Alpha channel.

- `micro.gfx.pixel(x, y, r, g, b, a)`
  Draws a single pixel.
  - `x`: X coordinate.
  - `y`: Y coordinate.

- `micro.gfx.line(x1, y1, x2, y2, r, g, b, a)`
  Draws a line segment.
  - `x1`, `y1`: Start point.
  - `x2`, `y2`: End point.

- `micro.gfx.rect(mode : string, x, y, w, h, r, g, b, a)`
  Draws a rectangle.
  - `mode`: `"fill"` or `"line"`.
  - `x`, `y`: Top-left corner.
  - `w`, `h`: Size.

- `micro.gfx.circle(mode : string, x, y, radius, r, g, b, a)`
  Draws a circle.
  - `mode`: `"fill"` or `"line"`.
  - `x`, `y`: Center point.
  - `radius`: Circle radius.

- `micro.gfx.draw(image : micro.Image, x, y, rect=nil, r=0, sx=1, sy=sx, ox=0, oy=0)`
  Draws an image buffer to the screen.
  - `image`: `micro.Image` instance.
  - `x`, `y`: Destination position.
  - `rect`: Optional source sub-rectangle map with `x`, `y`, `w`, `h`.
  - `r`: Rotation angle.
  - `sx`, `sy`: Scale factors.
  - `ox`, `oy`: Origin offset.

- `micro.gfx.setFont(font : micro.Font)`
  Selects the font used by `micro.gfx.print()`.
  - `font`: `micro.Font` instance.

- `micro.gfx.print(text : string, x=0, y=0, r=0, sx=nil, sy=nil, ox=0, oy=0)`
  Renders text with the current font.
  - `text`: String to draw.
  - `x`, `y`: Destination position.
  - `r`: Rotation angle.
  - `sx`, `sy`: Scale factors.
  - `ox`, `oy`: Origin offset.

- `micro.gfx.setClearColor(r, g, b)`
  Stores the default clear color used by `micro.gfx.clear()`.
  - `r`: Red channel.
  - `g`: Green channel.
  - `b`: Blue channel.

## `micro.audio`

- `micro.audio.init(rate : integer = 44100, bufferSize : integer = 44100)`
  Initializes the audio system.
  - `rate`: Sample rate in Hz.
  - `bufferSize`: Requested buffer size.

- `micro.audio.master : micro.Source`
  The master audio source. All other sources ultimately route to this source unless rerouted.

## `micro.data`

- `micro.data.compress(data : string) -> string`
  Compresses a byte string with miniz.
  - `data`: Input string.

- `micro.data.decompress(data : string) -> string`
  Decompresses a miniz-compressed byte string.
  - `data`: Compressed input string.

## `micro.imagefx`

- `micro.imagefx.desaturate(image : micro.Image, amount : number)`
  Reduces color intensity in-place.
  - `image`: Destination image.
  - `amount`: Strength of the effect.

- `micro.imagefx.palette(image : micro.Image, palette : list)`
  Remaps image colors using a 256-color palette.
  - `image`: Destination image.
  - `palette`: List of palette colors.

- `micro.imagefx.dissolve(image : micro.Image, amount : integer, seed : integer = 0)`
  Randomly removes pixels by lowering alpha.
  - `image`: Destination image.
  - `amount`: Dissolve amount.
  - `seed`: Optional random seed.

- `micro.imagefx.mask(image : micro.Image, mask : micro.Image, channel : string)`
  Applies an alpha mask from another image.
  - `image`: Destination image.
  - `mask`: Mask image.
  - `channel`: One of `"r"`, `"g"`, `"b"`, or `"a"`.

- `micro.imagefx.wave(dst : micro.Image, src : micro.Image, amountX : number, amountY : number, scaleX : number, scaleY : number, offsetX : integer = 0, offsetY : integer = 0)`
  Copies `src` into `dst` with a sine-wave distortion.
  - `dst`: Destination image.
  - `src`: Source image.
  - `amountX`, `amountY`: Warp amplitudes.
  - `scaleX`, `scaleY`: Wave frequencies.
  - `offsetX`, `offsetY`: Wave phase offsets.

- `micro.imagefx.displace(dst : micro.Image, src : micro.Image, map : micro.Image, channelX : string, channelY : string, scaleX : integer, scaleY : integer)`
  Copies `src` into `dst` using a displacement map.
  - `dst`: Destination image.
  - `src`: Source image.
  - `map`: Displacement image.
  - `channelX`, `channelY`: Channel selectors used for X/Y offset.
  - `scaleX`, `scaleY`: Displacement scale.

- `micro.imagefx.blur(dst : micro.Image, src : micro.Image, radiusX : integer, radiusY : integer)`
  Applies a box blur.
  - `dst`: Destination image.
  - `src`: Source image.
  - `radiusX`, `radiusY`: Blur radius per axis.

## `micro.Image`

- `micro.Image.fromFile(path : string) -> image`
  Loads an image from disk.
  - `path`: File path.

- `micro.Image.fromString(data : string) -> image`
  Loads an image from an in-memory byte string.
  - `data`: Encoded image bytes.

- `micro.Image.fromBlank(width : integer, height : integer) -> image`
  Creates an empty transparent image.
  - `width`: Image width.
  - `height`: Image height.

- `image:clone() -> image`
  Creates a copy of the image.

- `image:reset()`
  Resets the image buffer to its default state.

- `image:clear(r, g, b, a)`
  Fills the whole image with a single color.

- `image:setPixel(x, y, r, g, b, a)`
  Writes one pixel into the image.

- `image:getPixel(x, y) -> list`
  Returns `[r, g, b, a]` for the requested pixel.

- `image:copyPixels(src : micro.Image, x=0, y=0, rect=nil, sx=1, sy=nil)`
  Copies pixels from another image into this one.
  - `src`: Source image.
  - `x`, `y`: Destination offset.
  - `rect`: Optional source sub-rectangle map.
  - `sx`, `sy`: Scaling factors.

- `image:noise(seed=0, low=0, high=0, grey=false)`
  Fills or perturbs the image with procedural noise.
  - `seed`: Noise seed.
  - `low`, `high`: Noise bounds.
  - `grey`: When `true`, generates grayscale noise.

- `image:floodFill(x, y, r, g, b, a)`
  Flood-fills the connected region starting at the given point.

- `image:getWidth() -> integer`
  Returns the image width in pixels.

- `image:getHeight() -> integer`
  Returns the image height in pixels.

- `image:tostring() -> string`
  Returns a string representation of the image userdata.

## `micro.Font`

- `micro.Font.fromFile(path : string, size : integer = 8) -> font`
  Loads a font from a file.
  - `path`: Font file path.
  - `size`: Point size.

- `micro.Font.fromEmbedded(size : integer = 8) -> font`
  Loads the built-in embedded font.
  - `size`: Point size.

- `font:render(text : string) -> image`
  Renders text into a new image.
  - `text`: String to render.

- `font:getWidth(text : string) -> integer`
  Returns the rendered width of the given text.
  - `text`: String to measure.

- `font:getHeight() -> integer`
  Returns the font line height.

- `font:tostring() -> string`
  Returns a string representation of the font userdata.

## `micro.Source`

- `micro.Source.fromData(data : micro.Data) -> source`
  Creates an audio source from encoded sample data.
  - `data`: `micro.Data` instance containing a WAV or OGG stream.

- `micro.Source.fromBlank(length : integer) -> source`
  Creates an empty audio source placeholder.
  - `length`: Sample buffer length.

- `source:getLength() -> number`
  Returns the source length in seconds, normalized to the current audio rate.

- `source:getState() -> string`
  Returns the current playback state as `"playing"`, `"paused"`, or `"stopped"`.

- `source:setDestination(dest : micro.Source | nil)`
  Routes the source output to another source.
  - `dest`: Destination source, or `nil` to route back to `micro.audio.master`.

- `source:setGain(gain : number)`
  Sets the output gain.
  - `gain`: Linear gain, where `1.0` is unity.

- `source:setPan(pan : number)`
  Sets stereo pan.
  - `pan`: Pan value in the range `-1.0` to `1.0`, where `-1.0` is full left and `1.0` is full right.

- `source:setRate(rate : number)`
  Sets playback speed relative to the source's base rate.
  - `rate`: Playback speed multiplier. Values below `0` are rejected.

- `source:setLoop(enabled : bool)`
  Enables or disables looping.
  - `enabled`: `true` to loop playback, `false` to stop at the end.

- `source:play(reset : bool = false)`
  Starts playback.
  - `reset`: When `true`, rewinds before playing.

- `source:pause()`
  Pauses playback without resetting the playhead.

- `source:stop()`
  Stops playback and leaves the source at rest.

## `micro.Gif`

- `micro.Gif.new(path : string, width : integer, height : integer, colors : integer = 63) -> gif`
  Opens a GIF writer.
  - `path`: Output file path.
  - `width`: Frame width.
  - `height`: Frame height.
  - `colors`: Number of palette colors to use.

- `gif:update(image : micro.Image, delay : integer)`
  Writes a frame to the GIF.
  - `image`: Source image.
  - `delay`: Frame delay in milliseconds.

- `gif:close()`
  Finalizes and closes the GIF file.
