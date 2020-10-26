; ==============================================================================
; =                                                                            =
; = Author: Sylwester Wysocki <sw143@wp.pl>                                    =
; = Created on: 2002                                                           =
; =                                                                            =
; = This is free and unencumbered software released into the public domain.    =
; =                                                                            =
; = Anyone is free to copy, modify, publish, use, compile, sell, or            =
; = distribute this software, either in source code form or as a compiled      =
; = binary, for any purpose, commercial or non-commercial, and by any          =
; = means.                                                                     =
; =                                                                            =
; = In jurisdictions that recognize copyright laws, the author or authors      =
; = of this software dedicate any and all copyright interest in the            =
; = software to the public domain. We make this dedication for the benefit     =
; = of the public at large and to the detriment of our heirs and               =
; = successors. We intend this dedication to be an overt act of                =
; = relinquishment in perpetuity of all present and future rights to this      =
; = software under copyright law.                                              =
; =                                                                            =
; = THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,            =
; = EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF         =
; = MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     =
; = IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR          =
; = OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,      =
; = ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR      =
; = OTHER DEALINGS IN THE SOFTWARE.                                            =
; =                                                                            =
; = For more information, please refer to <https://unlicense.org>              =
; =                                                                            =
; ==============================================================================

ASSUME cs:CODE
CODE SEGMENT
           org    100h
Start:
           include  LOAD.INC

           push     word ptr cs:[SegMIDI]
           pop      ds

           mov      word ptr cs:[PozMIDI], 21   ;poczatek danych MIDI
           call     ResetMIDI

           mov      si, 11                      ;adres tablicy instrumentow
           call     SetChannels
           mov      byte ptr cs:[StatusMIDI], 1 ;Zaznacz ,ze ma grac


           mov      ah, 0bh
@@1:
           int      21h
           cmp      al, 0ffh
           jne      @@1

Koniec:
           mov    ax, word ptr cs:[Old1ch+2]
           mov    ds, ax
           mov    dx, word ptr cs:[Old1ch]
           mov    ax, 251ch
           int    21h

           xor    ah, ah
           int    16h
           mov    ax, 4c01h
           int    21h

          include  MIDIDRV.INC
          PlikMIDI db 'music1.smf',0

CODE ENDS
END Start