; this is the exmaple layout for the interpreter instructions for c-set
; heres the conversion for the following:
;     $(YEAR)         = time->year
;     $(PROJECT)      *? 'Whats the name for this project ('main\'): ' | 'main'
;     $(ENTRY)        = config->entry_point | json->main | '${fileBasenameNoExtension}'
;     if @win32
;           $(NAME) *! 'Whats your name: '
;     elif @linux
;           $(NAME) = 'linux-user'
;     else
;           $(NAME) = 'unknown'
; YEAR is storeed in 0x01FA
; globals defined here
.globals ; const vars cannot be getters like time.year only literals
    mov 'main',                                         0x0001 ; default project name
    mov '${fileBasenameNoExtension}',                   0x0002 ; entry point
    mov 'Whats the name for this project (\'main\'): ', 0x0003 ; string
    mov 'Whats your name: ',                            0x0004 ; string
    mov 'linux-user',                                   0x0005 ; string
    mov 'unknown',                                      0x0006 ; string

.refs ; all the variable names and the adress to it
    set YEAR,       0x071FA
    set PROJECT,    0x071FB
    set ENTRY,      0x071FC
    set NAME,       0x071FD
    
; the code starts here
: 'Z:\devolopment\c-set\c++.c-set-template\.template', 1 ; $(YEAR) = time->year
    mov @time.year, YEAR
: 'Z:\devolopment\c-set\c++.c-set-template\.template', 2 ; $(PROJECT) *? 'Whats the name for this project (\'main\'): '
    in 0x0003, PROJECT
    if PROJECT, nil
        mov 0x0001, PROJECT
    end
    bge PROJECT, nil ; sanatize PROJECT
: 'Z:\devolopment\c-set\c++.c-set-template\.template', 3 ; $(ENTRY) = config->entry_point | json->main | '${fileBasenameNoExtension}'
    mov @config.entry_point, ENTRY
    if ENTRY, nil
        mov @json.main, ENTRY
    end
    if ENTRY, nil
        mov 0x0002, ENTRY
    end
: 'Z:\devolopment\c-set\c++.c-set-template\.template', 4 ; if @win32
    if #win32, 1
: 'Z:\devolopment\c-set\c++.c-set-template\.template', 5 ; $(NAME) *! 'Whats your name: '
        dup 0x0004, NAME ; ask name once and store it in .defaults
        bge NAME, nil ; sanatize NAME
: 'Z:\devolopment\c-set\c++.c-set-template\.template', 6 ; elif @linux
    elif #linux, 1
        mov 0x0005, NAME
: 'Z:\devolopment\c-set\c++.c-set-template\.template', 7 ; else
    else
        mov 0x0006, NAME
    end
end:
    hlt ; halt the program

; addrs that start with 0x0 are literals
; addrs that start with 0x07 are variables
; addrs that start with 0xDF are functions
; addrs that start with 0x0F are labels