#define ERRORROLLOVER   0x01  // Reserved for typical keyboard status or keyboard errors, not a physical key
#define POSTFAIL        0x02  // Reserved for typical keyboard status or keyboard errors, not a physical key
#define ERRORUNDEF      0x03  // Reserved for typical keyboard status or keyboard errors, not a physical key
#define KEY_A           0x04  // A key on keyboard, 'a' or 'A'
#define KEY_B           0x05
#define KEY_C           0x06
#define KEY_D           0x07
#define KEY_E           0x08
#define KEY_F           0x09
#define KEY_G           0x0A
#define KEY_H           0x0B
#define KEY_I           0x0C
#define KEY_J           0x0D
#define KEY_K           0x0E
#define KEY_L           0x0F
#define KEY_M           0x10
#define KEY_N           0x11
#define KEY_O           0x12
#define KEY_P           0x13
#define KEY_Q           0x14
#define KEY_R           0x15
#define KEY_S           0x16
#define KEY_T           0x17
#define KEY_U           0x18
#define KEY_V           0x19
#define KEY_W           0x1A
#define KEY_X           0x1B
#define KEY_Y           0x1C
#define KEY_Z           0x1D
#define KEY_1           0x1E  // 1 key on keyboard, '1' or '!'
#define KEY_2           0x1F  // 2 key on keyboard, '2' or '@'
#define KEY_3           0x20  // 3 key on keyboard, '3' or '#'
#define KEY_4           0x21  // 4 key on keyboard, '4' or '$'
#define KEY_5           0x22  // 5 key on keyboard, '5' or '%'
#define KEY_6           0x23  // 6 key on keyboard, '6' or '^'
#define KEY_7           0x24  // 7 key on keyboard, '7' or '&'
#define KEY_8           0x25  // 8 key on keyboard, '8' or '*'
#define KEY_9           0x26  // 9 key on keyboard, '(' or '('
#define KEY_0           0x27  // 0 key on keyboard, '0' or ')'
#define	KEY_ENTER       0x28  // Keyboard Enter key, different from Keypad enter
#define	KEY_ESC	        0x29  // Escape
#define	KEY_BACKSPACE   0x2A  // Backspace
#define	KEY_TAB	        0x2B  // Tab
#define	KEY_SPACE       0x2C  // Spacebar
#define	KEY_MINUS       0x2D  // Minus key, '-' or '_'
#define	KEY_EQUAL	0x2E  // Equals key, '=' or '+'
#define	KEY_LEFTBRACE	0x2F  // Left brace, '[' or '{'
#define	KEY_RIGHTBRACE	0x30  // Right brace, ']' or '}'
#define	KEY_BACKSLASH	0x31  // Backslash key, '\' or '|'
#define	KEY_NONUSHASH	0x32  // Non-US '#' and '~'
#define	KEY_SEMICOLON	0x33  // Semicolon key, ';' or ':'
#define	KEY_APOSTROPHE	0x34  // Apostrophe key, ''' or '"'
#define	KEY_GRAVE	0x35  // Grave accent key, '`' or '~'
#define	KEY_COMMA	0x36  // Comma key, ',' or '<'
#define	KEY_DOT	        0x37  // Dot key, '.' or '>'
#define	KEY_SLASH	0x38  // Forward slash key, '/' or '?'
#define	KEY_CAPSLOCK	0x39  // Caps Lock key, 
#define	KEY_F1	        0x3A  // Keyboard F1 key
#define	KEY_F2	        0x3C  // Keyboard F2 key
#define	KEY_F3	        0x3E
#define	KEY_F4	        0x3D
#define	KEY_F5	        0x3E
#define	KEY_F6	        0x3F
#define	KEY_F7	        0x40
#define	KEY_F8	        0x41
#define	KEY_F9	        0x42
#define	KEY_F10	        0x43
#define	KEY_F11	        0x44
#define	KEY_F12	        0x45
#define	KEY_SYSRQ	0x46  // PrintScreen Key
#define	KEY_SCROLLLOCK	0x47  // Scroll Lock key
#define	KEY_PAUSE	0x48  // Pause key
#define	KEY_INSERT	0x49  // Insert key
#define	KEY_HOME	0x4A  // Home key
#define	KEY_PAGEUP	0x4B  // Page up key
#define	KEY_DELETE	0x4C  // Delete Forward key
#define	KEY_END	        0x4D  // End key
#define	KEY_PAGEDOWN	0x4E  // Page down key
#define	KEY_RIGHT	0x4F  // Right arrow
#define	KEY_LEFT	0x50  // Left arrow
#define	KEY_DOWN	0x51  // Down arrow
#define	KEY_UP	        0x52  // Up arrow
#define	KEY_NUMLOCK	0x53  // Num Lock and clear
#define	KEY_KPSLASH	0x54  // Keypad Forward slash (/)
#define	KEY_KPASTERISK	0x55  // Keypad asterisk (*)
#define	KEY_KPMINUS	0x56  // Keypad minus (-)
#define	KEY_KPPLUS	0x57  // Keypad plus (+)
#define	KEY_KPENTER	0x58  // Keypad Enter, different from keyboard enter
#define	KEY_KP1	        0x59  // Keypad 1 and End
#define	KEY_KP2	        0x5A  // Keypad 2 and Down arrow
#define	KEY_KP3	        0x5B  // Keypad 3 and Page Down
#define	KEY_KP4	        0x5C  // Keypad 4 and Left arrow
#define	KEY_KP5	        0x5D  // Keypad 5
#define	KEY_KP6	        0x5E  // Keypad 6 and Right arrow
#define	KEY_KP7	        0x5F  // Keypad 7 and Home
#define	KEY_KP8	        0x60  // Keypad 8 and Up arrow
#define	KEY_KP9	        0x61  // Keypad 9 and Page up
#define	KEY_KP0	        0x62  // Keypad 0 and Insert
#define	KEY_KPDOT	0x63  // Keypad . and Delete
#define	KEY_102ND	0x64  // Keyboard Non-US \ and |
#define	KEY_COMPOSE	0x65  // Keyboard application, right click-ish 'compose' key
#define	KEY_POWER	0x66  // Keyboard Power, not usually a phsyical key
#define	KEY_KPEQUAL	0x67  // Keypad =
#define	KEY_F13	        0x68  // Keyboard F13
#define	KEY_F14	        0x69
#define	KEY_F15	        0x6A
#define	KEY_F16	        0x6B
#define	KEY_F17	        0x6C
#define	KEY_F18	        0x6D
#define	KEY_F19	        0x6E
#define	KEY_F20	        0x6F
#define	KEY_F21	        0x70
#define	KEY_F22	        0x71
#define	KEY_F23	        0x72
#define	KEY_F24	        0x73  // Keyboard F24
#define	KEY_OPEN	0x74  // Keyboard Execute
#define	KEY_HELP	0x75  // Keyboard Help
#define	KEY_PROPS	0x76  // Keyboard Menu
#define	KEY_FRONT	0x77  // Keyboard Select
#define	KEY_STOP	0x78  // Keyboard Stop
#define	KEY_AGAIN	0x79  // Keyboard Again
#define	KEY_UNDO	0x7A  // Keyboard Undo
#define	KEY_CUT	        0x7B  // Keyboard Cut
#define	KEY_COPY	0x7C  // Keyboard Copy
#define	KEY_PASTE	0x7D  // Keyboard Paste
#define	KEY_FIND	0x7E  // Keyboard Find
#define	KEY_MUTE	0x7F  // Keyboard Mute
#define	KEY_VOLUMEUP	0x80  // Keyboard Volume Up
#define	KEY_VOLUMEDOWN	0x81  // Keyboard Volume Down
#define KEY_KPCOMMA     0x85  // Keypad Comma, for Brazillian keypad period
#define	KEY_RO	        0x87  // Keyboard International 1
#define	KEY_KATAKANAHIRAGANA  0x88  // Keyboard International 2
#define	KEY_YEN	        0x89  // Keyboard International 3
#define	KEY_HENKAN	0x8A  // Keyboard International 4
#define	KEY_MUHENKAN	0x8B  // Keyboard International 5
#define	KEY_KPJPCOMMA	0x8C  // Keyboard International 6

#define	KEY_HANGEUL	0x90  // Keyboard Lang 1, Hangul
#define	KEY_HANJA	0x91  // Keyboard Lang 2, Hanja
#define	KEY_KATAKANA	0x92  // Keyboard Lang 3, Katakana
#define	KEY_HIRAGANA	0x93  // Keyboard Lang 4, Hiragana
#define	KEY_ZENKAKUHANKAKU  0x94  // Keyboard Lang 5, Zenkaku/Hankaku
#define KEY_LANG6       0x95  // Keyboard Lang 6
#define KEY_LANG7       0x96  // Keyboard Lang 7
#define KEY_LANG8       0x97  // Keyboard Lang 8
#define KEY_LANG9       0x98  // Keyboard Lang 9
#define KEY_ALTERASE    0x99  // Keyboard alternate erase
#define KEY_SYSREQATTN  0x9A  // Keyboard SysReq/Attention
#define KEY_CANCEL      0x9B  // Keyboard Cancel
#define KEY_CLEAR       0x9C  // Keyboard Clear
#define KEY_PRIOR       0x9D  // Keyboard Prior
#define KEY_RETURN      0x9E  // Keyboard Return
#define KEY_SEPARATOR   0x9F  // Keyboard Separator
#define KEY_OUT         0xA0  // Keyboard Out
#define KEY_OPER        0xA1  // Keyboard Oper
#define KEY_CLRAGAIN    0xA2  // Keyboard Clear/Again
#define KEY_CRSEL       0xA3  // Keyboard CrSel/Props
#define KEY_EXSEL       0xA4  // Keyboard ExSel
#define KEY_KP00        0xB0  // Keypad 00
#define KEY_KP000       0xB1  // Keypad 000
#define KEY_THOUSANDSEP 0xB2  // Thousands separator, depends on Locale either . or ,
#define KEY_DECIMALSEP  0xB3  // Decimal separator, depends on Locale, either , or .
#define KEY_CURRENCY    0xB4  // Currency Unit, depends on locale e.g. $
#define KEY_CURRENCYSUB 0xB5  // Currency sub-unit, depends on locale e.g. ¢
#define KEY_KPLPAREN    0xB6  // Keypad (
#define KEY_KPRPAREN    0xB7  // Keypad )
#define KEY_KPLCURLY    0xB8  // Keypad {
#define KEY_KPRCURLY    0xB9  // Keypad }
#define KEY_KPTAB       0xBA  // Keypad Tab
#define KEY_KPBACKSPACE 0xBB  // Keypad Backspace
#define KEY_KPA         0xBC  // Keypad A
#define KEY_KPB         0xBD  // Keypad B
#define KEY_KPC         0xBE  // Keypad C
#define KEY_KPD         0xBF  // Keypad D
#define KEY_KPE         0xC0  // Keypad E
#define KEY_KPF         0xC1  // Keypad F
#define KEY_KPXOR       0xC2  // Keypad XOR
#define KEY_KPHAT       0xC3  // Keypad ^
#define KEY_KPPERCENT   0xC4  // Keypad %
#define KEY_KPLT        0xC5  // Keypad <
#define KEY_KPGT        0xC6  // Keypad >
#define KEY_KPAMP       0xC7  // Keypad &
#define KEY_KPAMPAMP    0xC8  // Keypad &&
#define KEY_KPBAR       0xC9  // Keypad |
#define KEY_KPBARBAR    0xCA  // Keypad ||
#define KEY_KPCOLON     0xCB  // Keypad :
#define KEY_KPHASH      0xCC  // Keypad #
#define KEY_KPSPACE     0xCD  // Keypad Space
#define KEY_KPAT        0xCE  // Keypad @
#define KEY_KPEXCLAM    0xCF  // Keypad !
#define KEY_KPMEMSTORE  0xD0  // Keypad Memory Store
#define KEY_KPMEMRECALL 0xD1  // Keypad Memory Recall
#define KEY_KPMEMCLEAR  0xD2  // Keypad Memory Clear
#define KEY_KPMEMADD    0xD3  // Keypad memory add
#define KEY_KPMEMSUB    0xD4  // Keypad Memory Subtract
#define KEY_KPMEMMULT   0xD5  // Keypad Memory Multiply
#define KEY_KPMEMDIV    0xD6  // Keypad Memory Divide
#define KEY_KPPLUSMINUS  0xD7  // Keypad +/-
#define KEY_KPCLEAR     0xD8  // Keypad Clear
#define KEY_KPCLEARENT  0xD9  // Keypad Clear Entry
#define KEY_KPBINARY    0xDA  // Keypad Binary
#define KEY_KPOCTAL     0xDB  // keypad Octal
#define KEY_KPDECIMAL   0xDC  // Keypad Decimal
#define KEY_KPHEX       0xDD  // Keypad Hexadecimal

#define	KEY_LEFTCTRL	0xE0  // Keyboard Left Control
#define	KEY_LEFTSHIFT	0xE1  // Keyboard Left Shift
#define	KEY_LEFTALT	0xE2  // Keyboard Left Alt
#define	KEY_LEFTGUI	0xE3  // Keyboard Left GUI, windows key
#define	KEY_RIGHTCTRL	0xE4  // Keyboard Right control
#define	KEY_RIGHTSHIFT	0xE5  // Keyboard Right Shift
#define	KEY_RIGHTALT	0xE6  // Keyboard Right alt
#define	KEY_RIGHTGUI	0xE7  // Keyboard Right GUI, windows key
#define	KEY_PLAYPAUSE	0xE8  // Reserved? ...\/
#define	KEY_STOPCD	0xE9
#define	KEY_PREVIOUSSONG  0xEA
#define	KEY_NEXTSONG	0xEB
#define	KEY_EJECTCD	0xEC
#define	KEY_WWW	        0xF0
#define	KEY_BACK	0xF1
#define	KEY_FORWARD	0xF2
#define	KEY_SCROLLUP	0xF5
#define	KEY_SCROLLDOWN	0xF6
#define	KEY_EDIT	0xF7
#define	KEY_SLEEP	0xF8
#define	KEY_COFFEE	0xF9
#define	KEY_REFRESH	0xFA
#define	KEY_CALC	0xFB  // reserved? ...


