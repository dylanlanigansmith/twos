#pragma once


#define ASCII_TO_UPPER 0x20

//hard coded US Standard Keyboard 
const static char shifted_numbers[10] = {"!@#$%^&*()"};

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