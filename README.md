<br />
<p align="center">
  <a href="https://github.com/DanielSant0s/Enceladus/">
    <img src="https://user-images.githubusercontent.com/47725160/128257833-e5dba4f3-2c90-4b92-b512-6245e51580f2.png" alt="Logo" width="100%" height="auto">
  </a>

  <p align="center">
    Enhanced Lua environment for PlayStation 2™
    <br />
  </p>
</p>  


<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-enceladus">About Enceladus</a>
      <ul>
        <li><a href="#function-types">Function types</a></li>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#coding">Coding</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#features">Features</a></li>
      </ul>
    </li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#thanks">Thanks</a></li>
  </ol>
</details>

## About Enceladus

Enceladus is a project that seeks to facilitate and at the same time brings a complete kit for users to create homebrew software for PlayStation 2 using the Lua language. It has dozens of built-in functions, both for creating games and apps. The main advantage over using Enceladus project instead of the pure PS2SDK is above all the practicality, you will use one of the simplest possible languages to create what you have in mind, besides not having to compile, just script and test, fast and simple.

### Function types:
* System: Everything that involves files, folders and system stuff.
* Graphics: You can control the entire 2D part of your project, that is, draw images, shapes, lines, change their properties, etc.
* Screen: The entire screen of your project (2D and 3D), being able to change the resolution, enable or disable parameters.
* Font: Functions that control the texts that appear on the screen, loading texts, drawing and unloading from memory.
* Pads: Above being able to draw and everything else, A human interface is important. Supports rumble and pressure sensitivity.
* Timer: Control the time precisely in your code, it contains several timing functions.
* Sound: Basic sound functions.

New types are always being added and this list can grow a lot over time, so stay tuned.

### Built With

* [PS2DEV](https://github.com/ps2dev/ps2dev)
* [Lua](https://github.com/ps2dev/lua)

## Coding

In this section you will have some information about how to code using Enceladus, from prerequisites to useful functions and information about the language.

### Prerequisites

Using Enceladus you only need one way to code and one way to test your code, that is, if you want, you can even create your code on PS2, but I'll leave some recommendations below.

* PC: [Visual Studio Code](https://code.visualstudio.com)(with Lua extension) and [PCSX2](https://pcsx2.net/download/development/dev-windows.html)(1.7.0 or above, enabled HostFS is required) or PS2Client for test.
* How to enable HostFS on PCSX2 1.7.0:  
![image](https://user-images.githubusercontent.com/47725160/145600021-b07dd873-137d-4364-91ec-7ace0b1936e2.png)

* Android: [QuickEdit](https://play.google.com/store/apps/details?id=com.rhmsoft.edit&hl=pt_BR&gl=US) and a PS2 with uLE for test.

Oh, and I also have to mention that an essential prerequisite for using Enceladus is knowing how to code in Lua.

### Features

Enceladus uses the latest version of Lua language (currently 5.4.3), which means that it brings all language's available resources so far. Below is the list of usable functions of Enceladus project currently, this list is constantly being updated.

**Graphics functions:**

Primitive shapes:
* Graphics.drawPixel(x, y, color)
* Graphics.drawRect(x, y, width, height, color)
* Graphics.drawLine(x, y, x2, y2, color)
* Graphics.drawCircle(x, y, radius, color, filled) *filled isn't mandatory
* Graphics.drawTriangle(x, y, x2, y2, x3, y3, color, color2, color3) *color2 and color3 are not mandatory
* Graphics.drawQuad(x, y, x2, y2, x3, y3, x4, y4 color, color2, color3, color4) *color2, color3 and color4 are not mandatory

Image functions:
* image = Graphics.loadImage(path) *Supports BMP, JPG and PNG
* Graphics.drawImage(image, x, y, color)
* Graphics.drawRotateImage(image, x, y, angle, color)
* Graphics.drawScaleImage(image, x, y, scale_x, scale_y, color)
* Graphics.drawPartialImage(image, x, y, start_x, start_y, width, height, color)
* Graphics.drawImageExtended(image, x, y, start_x, start_y, width, height, scale_x, scale_y, angle, color)
* Graphics.setImageFilters(image, filter) *Choose between NEAREST and LINEAR filters
* width = Graphics.getImageWidth(image)
* height = Graphics.getImageHeight(image)
* Graphics.freeImage(image)

**Screen functions:**

* Screen.clear(color) *color isn't mandatory
* Screen.flip()
* Screen.waitVblankStart()
* Screen.setMode(mode, width, height, colormode, interlace, field)  
  • Default NTSC mode: Screen.setMode(NTSC, 640, 448, CT24, INTERLACED, FIELD)  
  • Default PAL mode: Screen.setMode(PAL, 640, 512, CT24, INTERLACED, FIELD)  
  • Available modes: NTSC, _480p, PAL, _576p, _720p, _1080i  
  • Available colormodes: CT16, CT16S, CT24, CT32  
  • Available interlaces: INTERLACED, NONINTERLACED  
  • Available fields: FIELD, FRAME  
* modetable = Screen.getMode()  
  • modetable.mode  
  • modetable.interlace  
  • modetable.field  
  • modetable.dithering  
  • modetable.doubleBuffering  
  • modetable.zBuffering  
  • modetable.width  
  • modetable.height  
  • modetable.aspect  
  • modetable.colormode  

**Font functions:**

Freetype functions(TTF, OTF):
* Font.ftInit()
* font = Font.ftLoad("font.ttf")
* Font.ftPrint(font, x, y, align, width, height, text, color)
* Font.ftSetPixelSize()
* Font.ftUnload(font)
* Font.ftEnd()

Image functions(FNT, PNG, BMP):
* font = Font.load("font.fnt/png/bmp")
* Font.print(font, x, y, scale, text, color)
* Font.unload(font)

ROM font functions:
* Font.fmLoad()
* Font.fmPrint(x, y, scale, text, color) *color isn't mandatory
* Font.fmUnload()

**Pads functions:**

* pad = Pads.get(port) *port isn't mandatory
* Pads.getLeftStick(port) *port isn't mandatory
* Pads.getRightStick(port) *port isn't mandatory
* type = Pads.getType(port) *port isn't mandatory  
  • PAD_DIGITAL  
  • PAD_ANALOG  
  • PAD_DUALSHOCK  
* press = Pads.getPressure(port, button) *port isn't mandatory
* Pads.rumble(port, big, small) *port isn't mandatory
* Pads.check(pad, button)
* Buttons list:  
  • PAD_SELECT  
  • PAD_START  
  • PAD_UP  
  • PAD_RIGHT  
  • PAD_DOWN  
  • PAD_LEFT  
  • PAD_TRIANGLE  
  • PAD_CIRCLE  
  • PAD_CROSS  
  • PAD_SQUARE  
  • PAD_L1  
  • PAD_R1  
  • PAD_L2  
  • PAD_R2  
  • PAD_L3  
  • PAD_R3  

**System functions:**

* fd = System.openFile(path, type)
* buffer = System.readFile(file, size)
* System.writeFile(fd, data, size)
* System.closeFile(fd)
* System.seekFile(fd, pos, type)
* size = System.sizeFile(fd)
* System.doesFileExist(path)
* System.CurrentDirectory(path) *if path given, it sets the current dir, else it gets the current dir
* listdir = System.listDirectory(path) *path isn't mandatory  
  • listdir[index].name - return file name on indicated index(string)  
  • listdir[index].size - return file size on indicated index(integer)  
  • listdir[index].directory - return if indicated index is a file or a directory(bool)  
* System.createDirectory(path)
* System.removeDirectory(path)
* System.removeFile(path)
* System.copyFile(source, dest)
* System.moveFile(source, dest)
* System.rename(source, dest)
* System.md5sum(string)
* System.sleep(sec)
* freemem = System.getFreeMemory()
* freevram = System.getFreeVRAM()
* fps = System.getFPS(prev, cur)
* System.exitToBrowser()
* type, freespace, format = System.getMCInfo(slot)

**Timer functions:**

* timer = Timer.new()
* Timer.getTime(timer)
* Timer.setTime(src, value)
* Timer.destroy(timer)
* Timer.pause(timer)
* Timer.resume(timer)
* Timer.reset(timer)
* Timer.isPlaying(timer)

**Sound functions:**

* Sound.setFormat(bitrate, freq, channels)
* Sound.setVolume(volume)
* Sound.initADPCM()
* Sound.setADPCMVolume(channel, volume)
* audio = Sound.loadADPCM(path)
* Sound.playADPCM(channel, audio)

## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AwesomeFeature`)
3. Commit your Changes (`git commit -m 'Add some AwesomeFeature'`)
4. Push to the Branch (`git push origin feature/AwesomeFeature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See `LICENSE` for more information.

<!-- CONTACT -->
## Contact

Daniel Santos - [@danadsees](https://twitter.com/your_username) - danielsantos346@gmail.com

Project Link: [https://github.com/DanielSant0s/Enceladus](https://github.com/DanielSant0s/Enceladus)

## Thanks

Here are some direct and indirect thanks to the people who made the project viable!

* HowlingWolf&Chelsea - Tests, tips and many other things
* fjtrujy - Help on porting current Lua language versions and general code help
* Rinnegatamante - Having created lpp, which served as a great initial guide
* Whole PS2DEV team





