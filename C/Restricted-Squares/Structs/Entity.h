#pragma once

#include "../Includes/Vector.h"
#include "./Image.h"

typedef struct {
    vec2 pos;
    int w, h;
    image2d img;
    int right_collision;
    int left_collision;
    int top_collision;
    int bottom_collision;
} static_entity_t;

typedef struct {
    vec2 pos;
    vec2 vel;
    vec2 acc;
    int w, h;
    int right_collision;
    int left_collision;
    int top_collision;
    int bottom_collision;
} dynamic_entity_t;


void init_static_entity_t(static_entity_t * e, float x, float y, int w, int h, image2d * img) {
    e -> pos.x = x;
    e -> pos.y = y;

    e -> w = w;
    e -> h = h;

    e -> img = *img;
}

void init_dynamic_entity(dynamic_entity_t * e, float x, float y, int w, int h, int accX, int accY) {
    e -> pos.x = x;
    e -> pos.y = y;

    e -> w = w;
    e -> h = h;

    e -> vel.x = 0;
    e -> vel.y = 0;

    e -> acc.x = accX;
    e -> acc.y = accY;
}

/* No update as collision_detect needs a custom
   movement updating type system */

#define collision_detect(m, l) \
    (m.pos.x + m.w >= l.pos.x) &&  \
    (l.pos.x + l.w >= m.pos.x) &&  \
    (m.pos.y + m.h >= l.pos.y) &&  \
    (l.pos.y + l.h >= m.pos.y)     \

