# GBoySim

## Explanation

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

## Build and Run



## References

https://robdor.com/2016/08/10/gameboy-emulator-half-carry-flag
https://gekkio.fi/files/gb-docs/gbctr.pdf
http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-GPU-Timings
https://gbdev.io/gb-opcodes//optables/
http://gameboy.mongenel.com/dmg/asmmemmap.html
https://www.youtube.com/watch?v=HyzD8pNlpwI&ab_channel=media.ccc.de
https://archive.org/details/GameBoyProgManVer1.1/page/n47/mode/2up
https://meganesulli.com/generate-gb-opcodes/