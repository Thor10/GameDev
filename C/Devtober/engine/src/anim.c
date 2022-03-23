#include <anim.h>
#include <stdlib.h>
#include <str.h>
#include <engine.h>

le_img_t * le_load_images(le_str prefixed_path, i32 frames, i32 w, i32 h) {
    le_img_t * imgs = calloc(frames, sizeof(le_img_t));
    le_str     curr_image;
    le_str     curr_frame;
    le_str     concat_ext = ".png";
    le_str     concatted;

    for (int i = 0; i < frames; i++) {
        curr_frame = i32_to_le_str(i);
        concatted = le_str_concat(curr_frame, concat_ext);
        curr_image = le_str_concat(prefixed_path, concatted);

        for (int i = 0; i < le_str_length(curr_image); i++) {
            if (curr_image[i] == 'g') {
                curr_image[i + 1] = '\0';
            }
        }

        imgs[i] = mk_le_img_t(curr_image, 0, 0, w, h);
    }

    free(curr_frame);
    free(concatted);
    free(curr_image);
    return imgs;
}
