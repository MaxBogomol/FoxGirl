#include "player.h"

#include "../obj/foxgirl_idle.h"
#include "../obj/foxgirl_run.h"
#include "../obj/foxgirl_jump.h"
#include "../obj/foxgirl_fall.h"
#include "../obj/foxgirl_crouch.h"

Player player;

void playerTick(void) {

}

void playerSpriteTick(Player* player) {
    bool spriteChange = false;

    if (player->oldState != player->state) {
        player->spriteIndex = 0;
        player->spriteTick = 0;
        if (player->state == PLAYER_STATE_RUN) {
            player->spriteIndex = 1;
        }
        spriteChange = true;
    }

    player->spriteX = (player->x >> 5) + DEVICE_SPRITE_PX_OFFSET_X;
    player->spriteY = (player->y >> 5) + DEVICE_SPRITE_PX_OFFSET_Y - 8;

    switch (player->state) {
        case PLAYER_STATE_IDLE: {
            if (player->spriteTick > 30) {
                player->spriteIndex = (player->spriteIndex + 1) % 2;
                player->spriteTick = 0;
                spriteChange = true;
            }
            player->tiles = foxgirl_idle_tiles;
            player->sprite = foxgirl_idle_metasprites;
            break;
        }
        case PLAYER_STATE_RUN: {
            if (player->spriteTick > 8) {
                player->spriteIndex = (player->spriteIndex + 1) % 4;
                player->spriteTick = 0;
                spriteChange = true;
            }
            player->tiles = foxgirl_run_tiles;
            player->sprite = foxgirl_run_metasprites;
            break;
        }
        case PLAYER_STATE_JUMP: {
            player->spriteIndex = 0;
            if (player->vy > 0) {
                player->spriteIndex = 1;
                spriteChange = true;
            }
            player->tiles = foxgirl_jump_tiles;
            player->sprite = foxgirl_jump_metasprites;
            break;
        }
        case PLAYER_STATE_FALL: {
            player->spriteIndex = 0;
            player->tiles = foxgirl_fall_tiles;
            player->sprite = foxgirl_fall_metasprites;
            break;
        }
        case PLAYER_STATE_CROUCH: {
            if (player->spriteTick > 30) {
                player->spriteIndex = (player->spriteIndex + 1) % 2;
                player->spriteTick = 0;
                spriteChange = true;
            }
            player->tiles = foxgirl_crouch_tiles;
            player->sprite = foxgirl_crouch_metasprites;
            break;
        }
        default: break;
    }
    player->spriteTick++;

    if (spriteChange) set_sprite_data(0, 4, player->tiles + (player->spriteIndex * 64));

    if (player->spriteFlip) {
        move_metasprite_flipx(player->sprite[0], 0, 0, 0, player->spriteX, player->spriteY);
    } else {
        move_metasprite(player->sprite[0], 0, 0, player->spriteX, player->spriteY);
    }
}