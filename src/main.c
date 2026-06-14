#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdbool.h>

#include "../obj/foxgirl_idle.h"
#include "../obj/foxgirl_run.h"
#include "../obj/foxgirl_jump.h"
#include "../obj/foxgirl_fall.h"
#include "../obj/foxgirl_crouch.h"
#include "../obj/test.h"

#include "player.h"

const uint8_t tiles[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

bool getTileCollision(int16_t x, int16_t y) {
    int16_t tileX = x >> 3;
    int16_t tileY = y >> 3;
    if (tileX < 0 || tileX >= 20 || tileY < 0 || tileY >= 18) return false; 
    uint16_t index = tileX + (tileY * 20);
    return (tiles[index] > 0);
}

void main(void) {
    uint8_t keys = 0;
    uint8_t oldKeys = 0;
    uint8_t pressedKeys = 0;
    uint8_t releasedKeys = 0;

    player.spriteX = 0; 
    player.spriteY = 0;
    player.spriteIndex = 0;
    player.spriteTick = 0;
    player.spriteFlip = false;
    player.oldState = PLAYER_STATE_IDLE;
    player.state = PLAYER_STATE_IDLE;
    player.x = (16) << 5; 
    player.y = (16) << 5;
    player.vx = 0; 
    player.vy = 0;
    player.onGround = false;
    player.coyoteTime = 0;
    player.jump = false;
    player.jumpBuffer = 0;
    player.jumpTick = 0;

    SPRITES_8x16; 

    set_sprite_palette(OAMF_CGB_PAL0, 1, foxgirl_idle_palettes);
    set_sprite_data(0, 4, foxgirl_idle_tiles);

    set_bkg_palette(OAMF_CGB_PAL0, 1, test_palettes);
    set_bkg_data(0, 2, test_tiles);

    set_bkg_tiles(0, 0, 20, 18, tiles);

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    while(1) {
        oldKeys = keys;
        keys = joypad();
        pressedKeys = keys & ~oldKeys;
        releasedKeys = oldKeys & ~keys;

        player.oldState = player.state;

        bool spriteChange = false;

        bool coll = false;
        int16_t x = (player.x >> 5);
        int16_t y = (player.y >> 5);

        for (uint8_t i = 0; i < 8; i++) {
            if (getTileCollision(x - 4 + i, y + 1)) {
                coll = true;
                break;
            }
        }

        if (coll) {
            player.onGround = true;
            player.coyoteTime = 8;
            if (player.state != PLAYER_STATE_IDLE || player.state != PLAYER_STATE_RUN) player.state = PLAYER_STATE_IDLE;
        } else {
            player.onGround = false;
            if (player.state != PLAYER_STATE_JUMP) player.state = PLAYER_STATE_FALL;
        }

        if (!player.onGround && player.coyoteTime > 0) player.coyoteTime--;

        if (keys & J_DOWN) {
            if (player.onGround) {
                player.state = PLAYER_STATE_CROUCH;
            }
        }

        bool friction = true;
        bool moved = false;
        if (keys & J_LEFT) {
            if (player.state != PLAYER_STATE_CROUCH) {
                player.vx -= player.onGround ? 5 : 2;
                if (player.vx < -50) player.vx = -50;
                player.spriteFlip = true;
                if (player.onGround) player.state = PLAYER_STATE_RUN;
                moved = true;
                friction = false;
            }
        }
        if (keys & J_RIGHT) {
            if (player.state != PLAYER_STATE_CROUCH) {
                player.vx += player.onGround ? 5 : 2;
                if (player.vx > 50) player.vx = 50;
                player.spriteFlip = false;
                if (player.onGround) player.state = PLAYER_STATE_RUN;
                moved = true;
                friction = false;
            }
        }
        if (!moved && player.state == PLAYER_STATE_RUN) {
            player.state = PLAYER_STATE_IDLE;
        }

        if (pressedKeys & J_A) {
            player.jumpBuffer = 10;
        }
        if (player.jumpBuffer > 0) {
            if (player.onGround || player.coyoteTime > 0) {
                player.vy = -140;
                player.jump = true;
                player.jumpBuffer = 0;
                player.jumpTick = (keys & J_A) ? 20 : 0;
                player.state = PLAYER_STATE_JUMP;
            }
        }
        if (releasedKeys & J_A) {
            if (player.state == PLAYER_STATE_JUMP) {
                if (player.jumpTick > 0) {
                    player.jumpTick = 0;
                }
            }
        }

        if (friction) {
            uint8_t fr = 1;
            if (player.onGround) fr = 4;
            if (player.state == PLAYER_STATE_CROUCH) fr = 6;
            if (player.vx > 0) {
                player.vx -= fr;
                if (player.vx < 0) player.vx = 0; 
            } else if (player.vx < 0) {
                player.vx += fr;
                if (player.vx > 0) player.vx = 0; 
            }
        }

        if (player.jumpBuffer > 0) player.jumpBuffer--;

        if (!player.onGround) {
            player.vy += 7;
            if (player.jumpTick > 0) player.jumpTick--;
            if (player.jump && player.jumpTick == 0 && player.vy < 0) {
                player.vy = (player.vy * 16) >> 5; //*50%
                player.jump = false;
            }
        }

        x = ((player.x + player.vx) >> 5);
        y = (player.y >> 5);

        for (uint8_t i = 0; i < 11; i++) {
            if (getTileCollision(x - 4, y - 11 + i)) {
                int16_t t = ((x) >> 3) << 3;
                player.x = (t + 4 << 5);
                player.vx = 0;
                break;
            }
        }

        for (uint8_t i = 0; i < 11; i++) {
            if (getTileCollision(x + 4, y - 11 + i)) {
                int16_t t = ((x) >> 3) << 3;
                player.x = (t + 4 << 5);
                player.vx = 0;
                break;
            }
        }

        x = (player.x >> 5);
        y = ((player.y + player.vy) >> 5);

        for (uint8_t i = 0; i < 8; i++) {
            if (getTileCollision(x - 4 + i, y)) {
                int16_t t = ((y) >> 3) << 3;
                player.y = (t << 5);
                player.vy = 0;
                break;
            }
        }

        for (uint8_t i = 0; i < 8; i++) {
            if (getTileCollision(x - 4 + i, y - 11)) {
                int16_t t = ((y) >> 3) << 3;
                player.y = (t + 4 << 5);
                player.vy = 0;
                break;
            }
        }

        player.x += player.vx;
        player.y += player.vy;

        if (player.x < ((4) << 5)) {
            player.x = ((4) << 5);
            player.vx = 0;
        }
        if (player.x > ((160 - 4) << 5)) {
            player.x = ((160 - 4) << 5);
            player.vx = 0;
        }

        if (player.y > ((144) << 5)) {
            player.y = ((144) << 5);
            player.vy = 0;
        }

        playerSpriteTick(&player);

        vsync();
    }
}