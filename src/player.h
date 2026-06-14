#pragma once

#include <gb/gb.h>
#include <gb/metasprites.h>
#include <stdbool.h>

typedef enum  {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_RUN,
    PLAYER_STATE_JUMP,
    PLAYER_STATE_FALL,
    PLAYER_STATE_CROUCH
} PlayerState;

typedef struct {
    const metasprite_t* const* sprite;
    const uint8_t* tiles;
    uint8_t spriteX;
    uint8_t spriteY;
    uint8_t spriteIndex;
    uint8_t spriteTick;
    bool spriteFlip;
    PlayerState oldState;
    PlayerState state;
    int16_t x;
    int16_t y;
    int16_t vx;
    int16_t vy;
    bool onGround;
    uint8_t coyoteTime;
    bool jump;
    uint8_t jumpBuffer;
    uint8_t jumpTick;
} Player;

extern Player player;

void playerTick(void);
void playerSpriteTick(Player* player);