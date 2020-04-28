# Chip8 Emulator

This is my first baby step in exploring the fascinating world of emulator-dev. I followed [this](https://austinmorlan.com/posts/chip8_emulator/) wonderful blog-post by Austin Morlan. This code is more or less a direct C port of the code given in his post.

## Building and Running

The code depends on SDL2. Assuming you've got it installed, simply run `make`. It will build and place the binary in `bin`.

The program expects 3 command line arguments:
1. scale: The canonical Chip8 display is 64x32. You obviously don't want to run an emulator that tiny. This is used to scale the viewer's size. If you're not sure, try using a scale of 10
2. delay: This controls the FPS of the emulator. It is the number of milliseconds b/w any 2 frames. Different ROMs work best with different delays.
3. rom: This is the path of the ROM

As an example, download [this](https://github.com/corax89/chip8-test-rom/blob/master/test_opcode.ch8) test ROM and place it in this project's directory. Then do `./bin/chip8 10 1 test_opcode.ch8`. A window should open up showing something which should look a bit similar to [this](https://github.com/corax89/chip8-test-rom/blob/master/test_rom.png) image.

Check out [this](https://github.com/dmatlack/chip8/tree/master/roms/games) link to see an extensive collection of game ROMs.

### Keypad to Keyboard input mapping

By design, Chip8 takes user input via 16 keys(0-F) on a keypad. They are mapped to your keyboard as shown in this diagram. All the game ROMs specify input controls in terms of keypad keys.

<pre>
Keypad       Keyboard
+-+-+-+-+    +-+-+-+-+
|1|2|3|C|    |1|2|3|4|
+-+-+-+-+    +-+-+-+-+
|4|5|6|D|    |Q|W|E|R|
+-+-+-+-+ => +-+-+-+-+
|7|8|9|E|    |A|S|D|F|
+-+-+-+-+    +-+-+-+-+
|A|0|B|F|    |Z|X|C|V|
+-+-+-+-+    +-+-+-+-+
</pre>

## Why code this thing in C?

A couple of reaons:
* this project seemed small enough that I felt doing it in C would not cause me any significant distress.
* why not?
