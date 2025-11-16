# track84

very badly written tracker for the ti-84 plus ce

notes:
- the code is VERY cursed. you probably do not want to use it in its current state unless you want to contribute
- the audio is 8000hz 8-bit
- the noise oscillator does not exist at the moment
- the usb device is added and removed every time it is played (by the calculator, not the user)

## usage

for now you use the arrow keys to move and the mode key to change the value of something.
enter key plays whatever you've put in.

oh yeah and clear exits the program

## compiling

install the [ce c toolchain](https://ce-programming.github.io/toolchain/) and then uh just run `make`

## todo

- [ ] usb stuff
  - [x] basic functionality and workingness
  - [ ] add a standby mode (using alternates)
- [ ] sound stuff
  - [x] basic oscillator system
  - [x] sine saw square triangle
  - [ ] get a functional noise generator
  - [ ] get working event handling
- [ ] gui stuff
  - [x] basic controls and grid layout
  - [ ] main menu
  - [ ] patterns
  - [ ] sections
  - [ ] songs
  - [ ] probably add saving somewhere in here as well

## high-level overview

there's 3 sections to the code: the gui, the usb driver, and the sound engine.

### the gui

simple-ish gui with a grid layout made in graphx

not much else to say here other than it's still very in beta right now

### the usb driver

this is what actually handles setting up the usb audio device and sending the data.

it calls the sound engine's `sound_GenerateSample` function to generate the audio samples which are then sent over usb.

### the sound engine

this contains the code for all of the oscillators and to generate the samples
