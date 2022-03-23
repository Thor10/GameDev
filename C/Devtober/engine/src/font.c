#include <font.h>


le_ui_label_t mk_label(i32 x, i32 y, le_str text, le_str font, i32 r, i32 g, i32 b, i32 a, i32 size,
                        SDL_Renderer * renderer) {
    le_ui_label_t label;
    label.pos.x = x;
    label.pos.y = y;
    label.label_text = text;
    label.label_font = le_load_font(font, size);
    label.text_color.r = r;
    label.text_color.g = g;
    label.text_color.b = b;
    label.text_color.a = a;
    label.label_tex = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(label.label_font,
                                                                                    label.label_text,
                                                                                    label.text_color));
    SDL_QueryTexture(label.label_tex, NULL, NULL, &label.pos.w, &label.pos.h);
    return label;
}

