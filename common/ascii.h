#pragma once


#define ASCII_TO_UPPER 0x20

//hard coded US Standard Keyboard 
const static char shifted_numbers[10] = {"!@#$%^&*()"};

static inline char ascii_to_upper(char c){
    if('a' <= c && c <= 'z'){
        return c - ASCII_TO_UPPER ; // sub 32
    }
    return c;
}

static inline char ascii_to_lower(char c){
    if('A' <= c && c <= 'Z'){
        return c + ASCII_TO_UPPER ; // add 32
    }
    return c;
}

static inline char ascii_normalize_alphanum(char c){
    //this is only for use with a bad reverse ascii  -> scancode thing im doing for TESTING ONLY

    if(c == ' ') return c; //pretty much the only one we wanna handle right now
    
    if(c < '!' ) return 0; //other wise its an escape or something 
    if('0' <= c && c <= '9')  return c;
    c = ascii_to_lower(c); //scancode 2 ascii chart is lowercase
    if('a' <= c && c <= 'z') return c;

    return 0;
    
}


static inline char apply_shift_modifier(char c){
    if(c < '!') return c; //escape sequence so no mod to apply

    if('a' <= c && c <= 'z'){
        return c - ASCII_TO_UPPER ; // sub 32
    }
    if('0' <= c && c <= '9'){
        return (c == '0') ? ')' : shifted_numbers[c - '1'];
    }
    switch(c)
    {
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case '\\': return '|';
        case ';': return ':';
        case '\'': return '"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        case '`': return '~';
        default:
            return c;
    } 
}