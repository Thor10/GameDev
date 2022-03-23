#include <draw.h>
emp _le_draw_rect(SDL_Rect * obj, i16 r, i16 g, i16 b, i16 a, SDL_Renderer * renderer) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, obj);
}

emp _le_draw_le_entity_t(le_entity_t * entity, le_img_t * image, SDL_Renderer * renderer) {
    SDL_Rect ent_rect;
    
    entity -> w = image -> src.w;
    entity -> h = image -> src.h;

    ent_rect.w = entity -> w;
    ent_rect.h = entity -> h;

    ent_rect.x = entity -> pos.x;
    ent_rect.y = entity -> pos.y;

    SDL_RenderCopy(renderer, image -> tex, &image -> src, &ent_rect);

}

emp _le_draw_le_entity_t_rect(le_entity_t * entity, i16 r, i16 g, i16 b, i16 a, SDL_Renderer * renderer) {
    SDL_Rect ent_rect;

    ent_rect.w = entity -> w;
    ent_rect.h = entity -> h;

    ent_rect.x = entity -> pos.x;
    ent_rect.y = entity -> pos.y;

    _le_draw_rect(&ent_rect, r, g, b, a, renderer);
}

emp _le_draw_le_img_t(le_img_t * image, i32 x, i32 y, SDL_Renderer * renderer) {
    SDL_Rect rect;
    
    rect.x = x;
    rect.y = y;
    rect.w = image -> src.w;
    rect.h = image -> src.h;

    SDL_RenderCopy(renderer, image -> tex, &image -> src, &rect);

}

emp _le_draw_le_ui_label_t(le_ui_label_t * label, SDL_Renderer * renderer) {
    SDL_RenderCopy(renderer, label -> label_tex, NULL, &label -> pos);
}


emp _le_draw_le_entity_t_ex(le_entity_t * entity, le_img_t * img, SDL_Renderer * renderer, float angle, SDL_Point * center, SDL_RendererFlip flip) {
    SDL_Rect ent_rect;
    
    entity -> w = img -> src.w;
    entity -> h = img -> src.h;

    ent_rect.w = entity -> w;
    ent_rect.h = entity -> h;

    ent_rect.x = entity -> pos.x;
    ent_rect.y = entity -> pos.y;

    SDL_RenderCopyEx(renderer, img -> tex, &img -> src, &ent_rect, angle, center, flip);
}
