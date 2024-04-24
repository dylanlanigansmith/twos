#pragma once

#ifndef __SCANCODE_H__
#define __SCANCODE_H__


#define SCAN_LSHIFT_DOWN 0x2a
#define SCAN_LSHIFT_UP 0xaa

#define SCAN_BACKSPACE_DOWN 0xe
#define SCAN_ENTER_DOWN 0x1c



//this guys table has been a life saver!
//https://blog.igorw.org/2015/03/04/scancode-to-ascii/
#define SCANCODE2ASCII_TABLE /*
       0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
*/  0x00, 0x1B, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0x08, 0x09, /*0
*/  0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69, 0x6F, 0x70, 0x5B, 0x5D, 0x0D, 0x00, 0x61, 0x73, /*1
*/  0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B, 0x27, 0x60, 0x00, 0x5C, 0x7A, 0x78, 0x63, 0x76, /*2
*/  0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2f, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*3
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*4
*/  0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*5
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*6
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*7
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*8
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*9
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*A
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*B
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*C
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*D
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*E
*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /*F
*/
static const unsigned char sc2ascii[] = {SCANCODE2ASCII_TABLE};



#define PS2_BREAK_ALL 0xff //all keys up / error
#define PS2_PAUSE_BREAK 0x9d


#define PS2_MAKEORBREAK 0x80 //0x10000000, mask, msb 1 = break 

#define PS2_KEY_ESCAPE        0x01
#define PS2_KEY_1             0x02
#define PS2_KEY_2             0x03
#define PS2_KEY_3             0x04
#define PS2_KEY_4             0x05
#define PS2_KEY_5             0x06
#define PS2_KEY_6             0x07
#define PS2_KEY_7             0x08
#define PS2_KEY_8             0x09
#define PS2_KEY_9             0x0A
#define PS2_KEY_0             0x0B
#define PS2_KEY_MINUS         0x0C    /* - on main keyboard */
#define PS2_KEY_EQUALS        0x0D
#define PS2_KEY_BACKSPACE     0x0E    /* backspace */
#define PS2_KEY_TAB           0x0F
#define PS2_KEY_Q             0x10
#define PS2_KEY_W             0x11
#define PS2_KEY_E             0x12
#define PS2_KEY_R             0x13
#define PS2_KEY_T             0x14
#define PS2_KEY_Y             0x15
#define PS2_KEY_U             0x16
#define PS2_KEY_I             0x17
#define PS2_KEY_O             0x18
#define PS2_KEY_P             0x19
#define PS2_KEY_LBRACKET      0x1A
#define PS2_KEY_RBRACKET      0x1B
#define PS2_KEY_ENTER         0x1C    /* Enter on main keyboard */
#define PS2_KEY_LEFT_CTRL     0x1D
#define PS2_KEY_A             0x1E
#define PS2_KEY_S             0x1F
#define PS2_KEY_D             0x20
#define PS2_KEY_F             0x21
#define PS2_KEY_G             0x22
#define PS2_KEY_H             0x23
#define PS2_KEY_J             0x24
#define PS2_KEY_K             0x25
#define PS2_KEY_L             0x26
#define PS2_KEY_SEMICOLON     0x27
#define PS2_KEY_APOSTROPHE    0x28
#define PS2_KEY_GRAVE         0x29    /* accent grave */
#define PS2_KEY_LEFT_SHIFT    0x2A
#define PS2_KEY_BACKSLASH     0x2B
#define PS2_KEY_Z             0x2C
#define PS2_KEY_X             0x2D
#define PS2_KEY_C             0x2E
#define PS2_KEY_V             0x2F
#define PS2_KEY_B             0x30
#define PS2_KEY_N             0x31
#define PS2_KEY_M             0x32
#define PS2_KEY_COMMA         0x33
#define PS2_KEY_PERIOD        0x34    /* . on main keyboard */
#define PS2_KEY_SLASH         0x35    /* / on main keyboard */
#define PS2_KEY_RIGHT_SHIFT   0x36
#define PS2_KEY_PRINTSCREEN   0x37
#define PS2_KEY_LEFT_ALT      0x38
#define PS2_KEY_SPACE         0x39
#define PS2_KEY_CAPSLOCK      0x3A
#define PS2_KEY_F1            0x3B
#define PS2_KEY_F2            0x3C
#define PS2_KEY_F3            0x3D
#define PS2_KEY_F4            0x3E
#define PS2_KEY_F5            0x3F
#define PS2_KEY_F6            0x40
#define PS2_KEY_F7            0x41
#define PS2_KEY_F8            0x42
#define PS2_KEY_F9            0x43
#define PS2_KEY_F10           0x44
#define PS2_KEY_NUMLOCK       0x45
#define PS2_KEY_SCROLLLOCK    0x46    /* Scroll Lock */
#define PS2_KEY_HOME          0x47    /* Home on arrow keypad */
#define PS2_KEY_UP            0x48    /* UpArrow on arrow keypad */
#define PS2_KEY_PAGEUP        0x49    /* PgUp on arrow keypad */
#define PS2_KEY_LEFT          0x4B    /* LeftArrow on arrow keypad */
#define PS2_KEY_CENTER        0x4C    /* 5 on arrow keypad */
#define PS2_KEY_RIGHT         0x4D    /* RightArrow on arrow keypad */
#define PS2_KEY_END           0x4F    /* End on arrow keypad */
#define PS2_KEY_DOWN          0x50    /* DownArrow on arrow keypad */
#define PS2_KEY_PAGEDOWN      0x51    /* PgDn on arrow keypad */
#define PS2_KEY_INSERT        0x52    /* Insert on arrow keypad */
#define PS2_KEY_DELETE        0x53    /* Delete on arrow keypad */
#define PS2_KEY_F11           0x57
#define PS2_KEY_F12           0x58
#define PS2_KEY_PAUSE         0x59    /* Pause/Break */
#define PS2_KEY_LEFT_WIN      0x5B    /* Left Windows key */
#define PS2_KEY_RIGHT_WIN     0x5C    /* Right Windows key */
#define PS2_KEY_MENU          0x5D    /* Application Menu key */
#define PS2_KEY_POWER         0x5E    /* Power Management key */
#define PS2_KEY_SLEEP         0x5F    /* System Sleep key */
#define PS2_KEY_WAKE          0x63    /* System Wake key */
#define PS2_KEY_WEB_SEARCH    0x65    /* Web Search key */
#define PS2_KEY_WEB_FAVORITES 0x66    /* Web Favorites key */
#define PS2_KEY_WEB_REFRESH   0x67    /* Web Refresh key */
#define PS2_KEY_WEB_STOP      0x68    /* Web Stop key */
#define PS2_KEY_WEB_FORWARD   0x69    /* Web Forward key */
#define PS2_KEY_WEB_BACK      0x6A    /* Web Back key */
#define PS2_KEY_MY_COMPUTER   0x6B    /* My Computer key */
#define PS2_KEY_MAIL          0x6C    /* Mail key */
#define PS2_KEY_MEDIA_SELECT  0x6D    /* Media Select key */
#define PS2_KEY_LEFT_CTRLE     0xE0    /* Left Ctrl key (extended) */
#define PS2_KEY_LEFT_SHIFTE    0xE1    /* Left Shift key (extended) */
#define PS2_KEY_RIGHT_CTRLE   0xE2    /* Right Ctrl key (extended) */
#define PS2_KEY_RIGHT_SHIFTE   0xE5    /* Right Shift key (extended) */


#define PS2_MAX_NAME_IDX 87
static const char* ps2_key_names[] = {
    "NONE",               // 0x00 (Reserved)
    "ESCAPE",             // 0x01
    "1",                  // 0x02
    "2",                  // 0x03
    "3",                  // 0x04
    "4",                  // 0x05
    "5",                  // 0x06
    "6",                  // 0x07
    "7",                  // 0x08
    "8",                  // 0x09
    "9",                  // 0x0A
    "0",                  // 0x0B
    "MINUS",              // 0x0C
    "EQUALS",             // 0x0D
    "BACKSPACE",          // 0x0E
    "TAB",                // 0x0F
    "Q",                  // 0x10
    "W",                  // 0x11
    "E",                  // 0x12
    "R",                  // 0x13
    "T",                  // 0x14
    "Y",                  // 0x15
    "U",                  // 0x16
    "I",                  // 0x17
    "O",                  // 0x18
    "P",                  // 0x19
    "LBRACKET",           // 0x1A
    "RBRACKET",           // 0x1B
    "ENTER",              // 0x1C
    "LEFT_CTRL",          // 0x1D
    "A",                  // 0x1E
    "S",                  // 0x1F
    "D",                  // 0x20
    "F",                  // 0x21
    "G",                  // 0x22
    "H",                  // 0x23
    "J",                  // 0x24
    "K",                  // 0x25
    "L",                  // 0x26
    "SEMICOLON",          // 0x27
    "APOSTROPHE",         // 0x28
    "GRAVE",              // 0x29
    "LEFT_SHIFT",         // 0x2A
    "BACKSLASH",          // 0x2B
    "Z",                  // 0x2C
    "X",                  // 0x2D
    "C",                  // 0x2E
    "V",                  // 0x2F
    "B",                  // 0x30
    "N",                  // 0x31
    "M",                  // 0x32
    "COMMA",              // 0x33
    "PERIOD",             // 0x34
    "SLASH",              // 0x35
    "RIGHT_SHIFT",        // 0x36
    "PRINTSCREEN",        // 0x37
    "LEFT_ALT",           // 0x38
    "SPACE",              // 0x39
    "CAPSLOCK",           // 0x3A
    "F1",                 // 0x3B
    "F2",                 // 0x3C
    "F3",                 // 0x3D
    "F4",                 // 0x3E
    "F5",                 // 0x3F
    "F6",                 // 0x40
    "F7",                 // 0x41
    "F8",                 // 0x42
    "F9",                 // 0x43
    "F10",                // 0x44
    "NUMLOCK",            // 0x45
    "SCROLLLOCK",         // 0x46
    "HOME",               // 0x47
    "UP",                 // 0x48
    "PAGEUP",             // 0x49
    "LEFT",               // 0x4B
    "CENTER",             // 0x4C
    "RIGHT",              // 0x4D
    "END",                // 0x4F
    "DOWN",               // 0x50
    "PAGEDOWN",           // 0x51
    "INSERT",             // 0x52
    "DELETE",             // 0x53
    "F11",                // 0x57
    "F12",                // 0x58
    "PAUSE",              // 0x59
    "LEFT_WIN",           // 0x5B
    "RIGHT_WIN"           // 0x5C
};


inline static const char* get_scancode_name(uint8_t sc){
    static const char* unknown = "UNKNOWN"; //should ITOA it if unknown
    if (sc < PS2_MAX_NAME_IDX){
        return ps2_key_names[sc];
    }

    return unknown;
}

inline static uint8_t is_scan_up(uint8_t sc){
    return (sc & PS2_MAKEORBREAK);
}

inline static uint8_t scan_extract_key(uint8_t sc){
    return (sc & 0x7f);
}
#endif // __SCANCODE_H__