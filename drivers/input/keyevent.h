#pragma once
#include "../../kernel/kernel.h"

#define KEYEVENT_BUF_SIZE 64
typedef struct {
    uint8_t events[KEYEVENT_BUF_SIZE];  // Array to hold key events
    int front;                          
    int rear;                                                
} KeyEventStream;


static inline void init_keystream(KeyEventStream* stream)
{
    memset((void*)stream->events, 0, KEYEVENT_BUF_SIZE);
    stream->front = 0;
    stream->rear = -1;
    
}
static inline uint8_t keystream_isempty(KeyEventStream* stream){
    return (stream->rear < 0) ;
}

static inline uint8_t keystream_isfull(KeyEventStream* stream){
    return ((stream->rear + 1) % KEYEVENT_BUF_SIZE == stream->front);
}


static inline uint8_t keystream_latest(KeyEventStream* stream){ //top
    if(keystream_isempty(stream)) return 0;

    return stream->events[stream->rear];
} static inline uint8_t keystream_top(KeyEventStream* stream){ return keystream_latest(stream); }

static inline uint8_t keystream_oldest(KeyEventStream* stream){ //bottom
    if(keystream_isempty(stream)) return 0;

    uint8_t sc = stream->events[stream->front];
} static inline uint8_t keystream_bottom(KeyEventStream* stream){ return keystream_oldest(stream); }


static inline uint8_t keystream_add(KeyEventStream* stream, uint8_t sc){
    
    
    stream->rear = (stream->rear + 1) % KEYEVENT_BUF_SIZE;
    stream->events[stream->rear] = sc;
    if(keystream_isfull(stream)){
        stream->front = (stream->front + 1) % KEYEVENT_BUF_SIZE;
    }
    return sc;
}