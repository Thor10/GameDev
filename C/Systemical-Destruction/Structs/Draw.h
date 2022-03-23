#pragma once

#include "../Includes/Engine.h"

#include "./Entity.h"
#include "./Image.h"

#include <SDL2/SDL.h>

void draw_entity_img(static_entity_t * e, image2d * image) {
    SDL_Rect entity;
   
    e -> w = image -> srcR.w;
    e -> h = image -> srcR.h;

    entity.w = e -> w;
    entity.h = e -> h;

    entity.x = e -> pos.x;
    entity.y = e -> pos.y;
    
    SDL_RenderCopy(le_g_renderer, image -> tex, &image -> srcR, &entity); 
}

void draw_static_entity_rect(static_entity_t * e, int r, int g, int b, int a) {
    SDL_Rect entity;

    entity.x = e -> pos.x;
    entity.y = e -> pos.y;
    entity.w = e -> w;
    entity.h = e -> h;

    le_draw_rect(&entity, r, g, b, a);
}

void draw_dynamic_entity_img(dynamic_entity_t * e, image2d * image, float angle, SDL_Point * center, SDL_RendererFlip flip) {
    SDL_Rect entity;
   
    e -> w = image -> srcR.w;
    e -> h = image -> srcR.h;

    entity.w = e -> w;
    entity.h = e -> h;

    entity.x = e -> pos.x;
    entity.y = e -> pos.y;
    
    SDL_RenderCopyEx(le_g_renderer, image -> tex, &image -> srcR, &entity, angle, center, flip); 
}

void draw_dynamic_entity_rect(dynamic_entity_t * e, int r, int g, int b, int a) {
    SDL_Rect entity;

    entity.x = e -> pos.x;
    entity.y = e -> pos.y;
    entity.w = e -> w;
    entity.h = e -> h;

    le_draw_rect(&entity, r, g, b, a);
}
