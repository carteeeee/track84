# track84

very badly written tracker for the ti-84 plus ce

notes:
- the audio is 8000hz 8-bit
- the usb device is added and removed every time it is played (by the calculator, not the user)

## usage

keys:

- clear: goes to previous menu and exits program
- 2nd: enables edit mode and clicks buttons on the main menu
- arrows: navigates around the ui, if in edit mode these edit the cell
- enter: activates usb interface and plays the song
- math: copies the previous cell and moves down 1 (must not be in edit mode)
- x^-1: clears the current cell (must not be in edit mode)

### edit mode

in edit mode, you are able to edit the current cell's value using the arrow keys.
the up/down arrow keys increment/decrement the value by 1, and the left/right arrow keys increment/decrement the value by a preset jump value (e.g. the jump value for musical notes is 12, for one octave).

### main menu

on the main menu you can choose to save/load songs, edit patterns, and edit your song using the arrows and the 2nd button.
on this menu, the clear key will close the program.

### pattern edit

the top number in the pattern editor is the pattern's number you're currently editing. the pattern length can be changed in the song edit menu

### song edit

the top left number in the song editor is the tempo (jump value is 10), and the number to the right of it is the pattern length (jump value is 8).
the 2nd row allows you to set the oscillator used for each of the 6 voices.
the rest of the rows allow you to program which pattern each voice plays at a certain time.

### playing the song

you can press enter to play the song at any point in the program, in which case it will show a screen that tells you it is playing and it will create a usb microphone device. to exit the playing mode, press any key (sometimes it doesn't want to work and you have to unplug and replug the usb while you're pressing a key. this is a limitation of ??? and i have no clue how to fix it).

## compiling

install the [ce c toolchain](https://ce-programming.github.io/toolchain/) and then run `make`

## todo

- [ ] usb stuff
  - [x] basic functionality and workingness
  - [ ] add a standby mode (using alternates)
- [ ] sound stuff
  - [x] basic oscillator system
  - [x] sine saw square triangle
  - [ ] get a functional noise generator
    - [x] basic noise
    - [ ] pitch change
  - [x] get working event handling
- [ ] gui stuff
  - [x] basic controls and grid layout
  - [x] main menu
  - [x] patterns
  - [x] songs
  - [ ] saving

## high-level overview

there's 3 sections to the code: the gui, the usb driver, and the sound engine.

### the gui

simple-ish gui with a grid layout made in graphx

it's got menu handling and special cases for editing to update the sound engine's state

### the usb driver

this is what actually handles setting up the usb audio device and sending the data.

it calls the sound engine's `GenerateSample` function to generate the audio samples which are then sent over usb.

### the sound engine

the sound engine is now fully written in assembly (for speed!!) and it is used to run the oscillators and generate samples using them.

the `GenerateSample` function has 3 parts, the timekeeper, which keeps track of time in terms of rows and patterns, the event handler, which updates each voice's pitch based on the current time in the state, and the oscillators themselves, which generate the actual audio. 
