; i am going insane :DDD

    assume adl=1

NUM_OSCS := 6
_oscasm_NoiseSeedInit := 0x4242

macro evhandler osc
    ;; oscillator arrangement lookup ;;
    ld      hl, _state_oscs + 65 * osc + 1
    ld      de, 0
    ld      e, c
    add     hl, de
    ld      e, (hl) ; de = ptrn idx
    
    ;; pattern & step lookup ;;
    ld      hl, _state_patterns
    ; what no multiply instruction does to a mf
    ex      hl, de
    add     hl, hl ; 2
    add     hl, hl ; 4
    add     hl, hl ; 8
    add     hl, hl ; 16
    add     hl, hl ; 32
    add     hl, hl ; 64
    add     hl, de
    ld      de, 0
    ld      e, b
    add     hl, de
    ld      e, (hl) ; de = note

    ;; note table lookup ;;
    ld      hl, _note_table
    ; what no multiply instruction does to a mf pt 2
    add     hl, de
    add     hl, de
    add     hl, de
    ld      de, (hl) ; de = inc
    ld      (_soundasm_incs + 3 * osc), de
end macro

; the noise oscillator is a macro because we need the oscillator number
macro noiseosc osc
    local b15z, b7z, nonewsample
    
    ; if it's been a certain amount of time then we generate a new sample
    ld      hl, (_oscasm_NoiseLast + 3 * osc)
    ld      de, 16384
    add     hl, de
    push    hl
    pop     de
    or      a, a
    sbc     hl, bc
    ld      hl, (_oscasm_NoiseSeeds + 3 * osc)
    jr      nc, nonewsample
    ld      (_oscasm_NoiseLast + 3 * osc), de

    ; from wikipedia for lfsr
    ; lfsr ^= lfsr >> 7
    ; lfsr ^= lfsr << 9
    ; lfsr ^= lfsr >> 13

    ; >> 7 (idek atp)
    ; bit 0 = bit 7
    ; bit 8 = bit 15
    ; lower byte = upper byte << 1
    ; 3+1+2+3+2 + 2+3+2 + 1+1+1+1+1+1 = 10+7+6 = 23 cycles (worst case)
    ld      de, 0
    ld      e, h
    sla     e
    jr      nc, b15z
    set     0, d
b15z:
    bit     7, l
    jr      z, b7z
    set     0, e
b7z:
    ld      a, d
    xor     a, h
    ld      h, a
    ld      a, e
    xor     a, l
    ld      l, a
    ; << 9 (<< 8 << 1)
    ; 1+2+1+1 = 5 cycles
    ld      a, l ; << 8, a is top byte
    sla     a    ; << 1
    xor     a, h
    ld      h, a
    ; >> 13 (>> 8 >> 5)
    ; 1+2+2+2+2+2+1+1 = 13 cycles
    ld      a, h ; >> 8, a is low byte
    srl     a    ; >> 1
    srl     a    ; >> 1
    srl     a    ; >> 1
    srl     a    ; >> 1
    srl     a    ; >> 1
    xor     a, l
    ld      l, a
    ; ---
    ld      (_oscasm_NoiseSeeds + 3 * osc), hl
nonewsample:
    sra     h
    sra     h
    sra     h
    pop     de
    ld      a, e
    add     a, h
end macro

macro oschandler osc
    local notnoise, noise

    ld      hl, (_soundasm_phases + 3 * osc)
    ld      de, (_soundasm_incs + 3 * osc)
    add     hl, de
    ld      (_soundasm_phases + 3 * osc), hl

    ; the oscs use:
    ; `bc` for their phase and `a` for their output
    ; they are free to modify any other registers
    ld      de, 0
    ld      hl, _state_oscs + 65 * osc
    ld      e, (hl)
    ; if e is the noise oscillator (4)
    ld      bc, 0
    ld      c, a
    push    bc
    ; we load the phase into bc because it's used for all oscs
    ld      bc, (_soundasm_phases + 3 * osc)
    ld      a, 4
    cp      a, e
    jr      nz, notnoise
    noiseosc osc
    jr      noise
notnoise:
    ld      hl, _oscasm_Table
    add     hl, de
    add     hl, de
    add     hl, de
    ld      hl, (hl)
    pop     de
    ld      a, e
    call    __indcallhl
noise:
end macro


    section .text

    public _soundasm_EventHandler
_soundasm_EventHandler:
    evhandler 0
    evhandler 1
    evhandler 2
    evhandler 3
    evhandler 4
    evhandler 5
    ret

    public _soundasm_Setup
_soundasm_Setup:
    ld      hl, _oscasm_NoiseSeedInit
    ld      (_oscasm_NoiseSeeds + 3 * 0), hl
    ld      (_oscasm_NoiseSeeds + 3 * 1), hl
    ld      (_oscasm_NoiseSeeds + 3 * 2), hl
    ld      (_oscasm_NoiseSeeds + 3 * 3), hl
    ld      (_oscasm_NoiseSeeds + 3 * 4), hl
    ld      (_oscasm_NoiseSeeds + 3 * 5), hl
    xor     a, a
    ld      hl, 0
    ld      (_soundasm_time), hl
    ld      (_soundasm_lastrow), hl
    ld      (_soundasm_row), a
    ld      (_soundasm_ptrn), a
    ld      bc, 0
    call    _soundasm_EventHandler
    ; do spr setup
    ret

    public _soundasm_GenerateSample
_soundasm_GenerateSample:
;;; timekeeper ;;;
    ld      de, (_soundasm_time)
    inc     de
    ld      (_soundasm_time), de
    
    ; time == lastrow + spr
    ld      hl, (_soundasm_lastrow)
    ld      bc, (_soundasm_spr)
    add     hl, bc

    ; you have to do some operation to clear the carry flag,
    ; theres no clear carry flag instruction :broken_heart:
    ; oh yeah and theres no subtract w/o carry for 24-bit regs
    ; thank you zilog
    or      a, a
    sbc     hl, de
    jr      nz, nonewrow
    
    ; load row & pattern into b & c, this is useful later
    ld      a, (_soundasm_row)
    ld      b, a
    ld      a, (_soundasm_ptrn)
    ld      c, a

    ld      (_soundasm_lastrow), de
    inc     b
    ld      a, b
    ld      (_soundasm_row), a

    ; row == pattern_length
    ld      a, (_state_pl)
    cp      a, b
    jr      nz, nonewptrn
    
    inc     c
    ld      a, c
    ld      (_soundasm_ptrn), a
    ld      b, 0
    xor     a, a
    ld      (_soundasm_row), a
nonewptrn:
;;; event handler ;;;
    ; at this point:
    ; b = row, c = ptrn
    call    _soundasm_EventHandler
nonewrow:
;;; oscillator section ;;;
    xor a, a
    oschandler 0
    oschandler 1
    oschandler 2
    oschandler 3
    oschandler 4
    oschandler 5
    ret

    private _oscasm_Sine
_oscasm_Sine:
    ld      de, 0
    ld      e, b
    ld      hl, _sine_table
    add     hl, de
    add     a, (hl)
    ret

    private _oscasm_Saw
_oscasm_Saw:
    ld      de, 0
    ld      e, b
    sra     e
    sra     e
    sra     e
    add     a, e
    ret


    private _oscasm_Square
_oscasm_Square:
    or     a, a
    ld     hl, 32768
    sbc.s  hl, bc
    jr     c, sqlow 
    add    a, 127 / NUM_OSCS
    ret
sqlow:
    add    a, -127 / NUM_OSCS
    ret

    private _oscasm_Triangle
_oscasm_Triangle:
    ld      de, 0
    ld      e, b
    ld      hl, _tri_table
    add     hl, de
    add     a, (hl)
    ret

    private _oscasm_Null
_oscasm_Null:
    ret



    section .data
    private _oscasm_NoiseSeeds
_oscasm_NoiseSeeds:
    dl _oscasm_NoiseSeedInit
    dl _oscasm_NoiseSeedInit
    dl _oscasm_NoiseSeedInit
    dl _oscasm_NoiseSeedInit
    dl _oscasm_NoiseSeedInit
    dl _oscasm_NoiseSeedInit

    section .bss
    private _oscasm_NoiseLast
_oscasm_NoiseLast:
    rb 3 * NUM_OSCS


    section .rodata
    private _oscasm_Table
_oscasm_Table:
    dl _oscasm_Sine
    dl _oscasm_Saw
    dl _oscasm_Square
    dl _oscasm_Triangle
    dl _oscasm_Null ; noise doesn't use the table
    dl _oscasm_Null



    section .bss

    private _soundasm_time
_soundasm_time:
    rb 3

    private _soundasm_lastrow
_soundasm_lastrow:
    rb 3

    private _soundasm_row
_soundasm_row:
    rb 1
    
    private _soundasm_ptrn
_soundasm_ptrn:
    rb 1



    section .bss
    public _soundasm_state
_soundasm_state:
    rb 4 ; for the magic
    private _state_tempo
_state_tempo:
    rb 1
    private _state_pl
_state_pl:
    rb 1
    private _state_oscs
_state_oscs:
    rb 65 * NUM_OSCS
    private _state_patterns
_state_patterns:
    rb 2048 ; 64 * 32



    section .data
    private _soundasm_spr
; samples per row
_soundasm_spr:
    ; 120bpm at 4 rows per beat and 8khz sample rate
    dl 1000



    section .bss
    
    private _soundasm_incs
_soundasm_incs:
    rb 3 * NUM_OSCS

    private _soundasm_phases
_soundasm_phases:
    rb 3 * NUM_OSCS



    section .rodata
    private	_note_table
_note_table:
    dl 0, 225, 238, 252, 267, 283, 300, 318, 337, 357, 378, 401, 425, 450, 477, 505, 535, 567, 601, 637, 675, 715, 757, 802, 850, 901, 954, 1011, 1071, 1135, 1202, 1274, 1350, 1430, 1515, 1605, 1701, 1802, 1909, 2022, 2143, 2270, 2405, 2548, 2700, 2860, 3030, 3211, 3402, 3604, 3818, 4045, 4286, 4541, 4811, 5097, 5400, 5721, 6061, 6422, 6804, 7208, 7637, 8091, 8572, 9082, 9622, 10195, 10801, 11443, 12123, 12844, 13608, 14417

    section .rodata
    private _sine_table
_sine_table:
    db 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 21, 21, 21, 21, 21, 21, 21, 21, 20, 20, 20, 20, 19, 19, 19, 19, 18, 18, 18, 18, 17, 17, 17, 16, 16, 16, 15, 15, 14, 14, 14, 13, 13, 12, 12, 11, 11, 11, 10, 10, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, -1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9, -10, -10, -10, -11, -11, -12, -12, -13, -13, -13, -14, -14, -15, -15, -15, -16, -16, -16, -17, -17, -17, -17, -18, -18, -18, -18, -19, -19, -19, -19, -20, -20, -20, -20, -20, -20, -20, -20, -21, -21, -21, -21, -21, -21, -21, -21, -21, -21, -21, -21, -21, -20, -20, -20, -20, -20, -20, -20, -20, -19, -19, -19, -19, -18, -18, -18, -18, -17, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14, -14, -13, -13, -13, -12, -12, -11, -11, -10, -10, -10, -9, -9, -8, -8, -7, -7, -6, -6, -5, -5, -4, -4, -3, -3, -2, -2, -1, -1
    
    section .rodata
    private _tri_table
_tri_table:
    db 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 21, 20, 20, 20, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, -1, -1, -1, -2, -2, -2, -3, -3, -3, -4, -4, -4, -5, -5, -5, -6, -6, -6, -7, -7, -7, -8, -8, -8, -9, -9, -9, -10, -10, -10, -11, -11, -11, -12, -12, -12, -13, -13, -13, -14, -14, -14, -15, -15, -15, -16, -16, -16, -17, -17, -17, -18, -18, -18, -19, -19, -19, -20, -20, -20, -21, -21, -21, -21, -21, -21, -20, -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14, -14, -14, -13, -13, -13, -12, -12, -12, -11, -11, -11, -10, -10, -10, -9, -9, -9, -8, -8, -8, -7, -7, -7, -6, -6, -6, -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, -1, 0

    extern __indcallhl
