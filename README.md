# GBoySim

## <ins>Explanation</ins>

I was looking for a project to help me better understand the low-level / EE environment for embedded systems as well as provide proof of my understanding of C++ 11, 14, 17, 20 concepts.
I'm starting with implementing a Game Boy simulator and hopefully moving onto more modern systems. The gameboy architecture has a number of interesting design choices such as:

* memory-mapping
* a CRT emulated screen
* a temporarily accessible boot rom
* multiple registers of varying size and purpose
* an instruction set with multiple addressing modes
  * including a "zero-page" memory access
* A memory block controller to allow larger games and save files
    * up to 2MB of ROM
* A Pixel Processing Unit utilizing a 8x8 bit tile structure for a reduced memory footprint
* A 3 layer system for rendering graphics (Background, Window, and Sprite) which the screen is then a viewport into.
* Several components utilizing "banks" reducing the memory map but still allowing access to relatively large amounts of memory.

## <ins>Build and Run</ins>

cmake -B ./build  
cmake --build .

## <ins>References</ins>

### Optables:
https://gbdev.io/gb-opcodes//optables/
https://meganesulli.com/generate-gb-opcodes/

### General Guides:
https://gekkio.fi/files/gb-docs/gbctr.pdf
http://gameboy.mongenel.com/dmg/asmmemmap.html
https://gbdev.io/pandocs/About.html
https://problemkaputt.de/pandocs.htm
https://archive.org/details/GameBoyProgManVer1.1/page/n47/mode/2up
http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-GPU-Timings
https://www.youtube.com/watch?v=HyzD8pNlpwI&ab_channel=media.ccc.de

### DAA Instruction:
https://forums.nesdev.org/viewtopic.php?t=15944#:~:text=The%20DAA%20instruction%20adjusts%20the,%2C%20lower%20nybble%2C%20or%20both.

### Half-Carry flag:
https://robdor.com/2016/08/10/gameboy-emulator-half-carry-flag