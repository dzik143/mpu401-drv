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

          OffsetMIDI dw (offset PlikMIDI1)
                     dw (offset PlikMIDI2)

          PlikMIDI1 db 'MUSIC1.SMF',0
          PlikMIDI2 db 'MENU.SMF',0

MIDI MACRO bajt
           mov    al, bajt
           call   WriteMIDI
ENDM

;ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ    Wysyla bajt do ukladu MIDI                                         บ
;บ       Wejscie: AL = bajt do wyslania                                  บ
;ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
WriteMIDI PROC
               push    dx
               push    ax

               mov     dx, 331h    ;Status Port
MidiWaitW:
               in      al, dx
               test    al, 40h     ;czekaj az bit 7=0 (uklad gotowy do zapisu)
               jne     MidiWaitW

               pop     ax
               dec     dx          ;DX= 330h - port danych
               out     dx, al

               pop     dx
               RET
ENDP

;ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ    Odbiera bajt z ukladu MIDI                                         บ
;บ       Wyjscie: AL = bajt zwrocony przez karte                         บ
;ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
ReadMIDI PROC
               push    dx

               mov     dx, 331h    ;Status Port
MidiWaitR:
               in      al, dx
               test    al, 80h     ;czekaj az bit 6 = 0 (uklad gotowy)
               jne     MidiWaitR

               dec     dx          ;DX= 330h - port danych
               in      al, dx

               pop     dx
               RET
ENDP

;ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ    Wysyla komende do ukladu MIDI                                      บ
;บ       Wejscie: AL = bajt do wyslania                                  บ
;ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
WriteCommandMIDI PROC
               push    dx
               push    ax

               mov     dx, 331h    ;Status Port
MidiWaitWC:
               in      al, dx
               test    al, 40h     ;czekaj az bit 7=0 (uklad gotowy do zapisu)
               jne     MidiWaitWC

               pop     ax
               out     dx, al      ;331h = Command Port

               pop     dx
               RET
ENDP

;ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ    Resetuje uklad MIDI i instaluje sterownik na IRQ 08h               บ
;ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
ResetMIDI PROC
                mov     al, 0ffh
                call    WriteCommandMIDI   ;komenda 0ffh = RESET KARTY

WaitForReady:   call    ReadMIDI
                cmp     al, 0feh           ;0feh = reset O.K
                jne     WaitForReady

                mov     al, 3fh            ;kom. 3fh = tryb Uart (??!!)
                call    WriteCommandMIDI
                RET
ENDP

;ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ    Wlacza podana nute na okreslonym instrumencie                      บ
;บ       Wejscie: AL = nuta                                              บ
;บ                BX = Nr. kanalu (programowego)                         บ
;บ                DS = segment danych MIDI                               บ
;ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
NoteOn PROC
            push   ax

            mov    al, 80h                 ;wylacz stara nute
            add    al, byte ptr ds:[bx+1]
            call   WriteMIDI
            mov    al, byte ptr cs:[OldNote+bx]
            call   WriteMIDI

            mov    al, 90h                 ;9x = graj
            add    al, byte ptr ds:[bx+1]  ;DS:1 = tablica kanalow
            call   WriteMIDI

            pop    ax
            mov    byte ptr cs:[OldNote+bx], al
            call   WriteMIDI               ;ustaw nute
            MIDI   7fh                     ;i glosnosc na 7fh (max.)
            RET
ENDP

;ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ    Programowanie kanalow                                              บ
;บ       Wejscie: DS:SI = tablica z kolejnymi instrumentami (dla 10 kan) บ
;ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
SetChannels PROC
            push   ax

            xor    ah, ah      ;nr. zmienianego kanalu
MainSC:     mov    al, ah
            add    al, 0c0h    ;Cx = zmiana instrumentu na kanale X
            call   WriteMIDI
            lodsb              ;AL = zadany instrument
            call   WriteMIDI

            inc    ah
            cmp    ah, 0ah
            jb     MainSC

            pop    ax
            RET
ENDP

     Old1ch   dd ?
    SegMIDI   dw ?
    PozMIDI   dw 21
    LiczMIDI  db 0
      Tempo   db 3
 StatusMIDI   db 0         ;ustwiane na 1 gdy ma grac
    OldNote   db 10 dup (35)

MidiTsr PROC
                cmp    byte ptr cs:[StatusMIDI], 1      ;sprawdz czy ma grac
                jne    KoniecIRQ3
                cli
                push   ds
                push   si
                push   ax
                push   bx

                mov    ax, word ptr cs:[SegMIDI]
                mov    ds, ax
                mov    si, word ptr cs:[PozMIDI]

                cmp    byte ptr cs:[LiczMIDI], 0
                je     Play
                dec    byte ptr cs:[LiczMIDI]         ;zmniejsz LiczMIDI
                jne    KoniecIRQ2

Play:           mov    al, byte ptr cs:[Tempo]
                mov    byte ptr cs:[LiczMIDI], al
                xor    bx, bx                         ;licznik kanalow
PlayChan:       lodsb                                 ;czytaj komende
                cmp    al, 0
                je     NextChan                       ;jezeli 0 to nie graj
                cmp    al, 0ffh                       ;0ffh = koniec
                jne    NextChan-3
                mov    word ptr cs:[PozMIDI], 21      ;poczatek danych MIDI
                jmp    KoniecIRQ2

                call   NoteOn
NextChan:       inc    bl                             ;Nastepny kanal
                cmp    bl, byte ptr ds:0              ;BL=IleKanalow
                jna    PlayChan

KoniecIRQ:      mov    word ptr cs:[PozMIDI], si
KoniecIRQ2:
                pop    bx
                pop    ax
                pop    si
                pop    ds
                sti
KoniecIRQ3:     IRET
ENDP

;ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
;ณ   ODTWARZA PLIK SMF             ณ
;ณ     Wejscie: BX = nr. pliku     ณ
;ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
PlayMIDI PROC
              push    ds
              push    si

              push    cs
              pop     ds
              shl     bx, 1
              mov     dx, word ptr cs:[OffsetMIDI+BX]
              mov     ax, 3d00h
              int     21h
              mov     bx, ax

              push    word ptr cs:[SegMIDI]
              pop     ds
              xor     dx, dx
              mov     cx, 2000
              mov     ah, 3fh
              int     21h

              mov     ah, 3eh
              int     21h

              mov     word ptr cs:[PozMIDI], 21  ;poczatek danych MIDI
              mov     si, 11                     ;adres tablicy instrumentow
              call    SetChannels
              mov     byte ptr cs:[StatusMIDI], 1 ;Zaznacz ,ze ma grac

              pop     si
              pop     ds
              RET
ENDP