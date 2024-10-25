# Pong-Homebrew-GBA
A pong game for the GBA

# Requirements
[The devkitpro toolchain](https://devkitpro.org/)
[The mGBA emulator](https://mgba.io/)
[Visual Studio Code](https://code.visualstudio.com/)

Some knowledge about C.

Some knowledge about GBA dev:
[this is a good starting point](https://zerodayarcade.com/tutorials/building-pong-for-the-gba)
[this is a good follow through](https://gbadev.net/tonc/foreword.html)

# Image to .h & .s generation
Generate your .h and .s file with [grit](https://www.coranac.com/man/grit/html/grit.htm) via the following command at the root of the project:
`grit image/* -gb`

Be careful, the generated image tends to be twice the size it should be, I don't know why.