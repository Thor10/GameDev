#ifndef _light_engine_maths

    #define _light_engine_maths
    
    #include "types.h"
   
    #define le_pi 3.14159265358979
    #define le_deg_to_rad(deg) deg * (le_pi / 180)
    #define le_rad_to_deg(rad) rad * (180 / le_pi)
    #define le_sqr(e) e * e
    #define le_dist_1d(c1, c2) c2 - c1
   
    f64 le_dist_2d (f64 x1, f64 y1, f64 x2, f64 y2);
    f64 le_dir_2d  (f64 x1, f64 y1, f64 x2, f64 y2);
    f64 le_inv_sqrt(f64 num);
    f64 le_lerp_f  (f64 start, f64 stop, f64 amt);
   
    typedef struct {
        f64 x; // 64-bit x float
        f64 y; // 64-bit y float
    } le_vec2_t;

    #define  le_v2_zero() (le_vec2_t) { .x = 0.0f, .y = 0.0f }
    #define   le_v2_one() (le_vec2_t) { .x = 1.0f, .y = 1.0f }
    #define le_v2(_x, _y) (le_vec2_t) { .x = _x, .y = _y }

    emp init_le_vec2_t   (le_vec2_t * v, f64 x, f64 y);    
    emp add_le_vec2_t    (le_vec2_t * v1, le_vec2_t * v2);
    emp sub_le_vec2_t    (le_vec2_t * v1, le_vec2_t * v2);
    emp lerp_le_vec2_t   (le_vec2_t * v1, le_vec2_t * v2, f64 amt);
    emp limit_le_vec2_t  (le_vec2_t * v, f64 scalar);
    emp scale_le_vec2_t  (le_vec2_t * v, f64 scalar);
    emp normal_le_vec2_t (le_vec2_t * v);
    emp rotate_le_vec2_t (le_vec2_t * v, f64 theta);
    emp set_le_vec2_t_mag(le_vec2_t * v, f64 scalar);

    f64 get_le_vec2_t_mag         (le_vec2_t * v);
    f64 get_angle_le_vec2_t       (le_vec2_t * v);
    f64 get_dot_prod_le_vec2_t    (le_vec2_t * v1, le_vec2_t * v2);
    f64 get_dist_between_le_vec2_t(le_vec2_t * v1, le_vec2_t * v2);
    f64 get_angle_of_two_le_vec2_t(le_vec2_t * v1, le_vec2_t * v2);

    le_vec2_t project_on_le_vec2_t(le_vec2_t * v1, le_vec2_t * v2);

    typedef struct {
        f64 x; // 64-bit x float
        f64 y; // 64-bit y float
        f64 z; // 64-bit z float
    } le_vec3_t;

    #define le_v3_zero() { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    #define le_v3_one () { .x = 1.0f, .y = 1.0f, .z = 1.0f };
    #define le_v3(_x, _y, _z) { .x = _x, .y = _y, .z = _z };

    emp init_le_vec3_t   (le_vec3_t * v, f64 x, f64 y, f64 z);    
    emp add_le_vec3_t    (le_vec3_t * v1, le_vec3_t * v2);
    emp sub_le_vec3_t    (le_vec3_t * v1, le_vec3_t * v2);
    emp lerp_le_vec3_t   (le_vec3_t * v1, le_vec3_t * v2, f64 amt);
    emp limit_le_vec3_t  (le_vec3_t * v, f64 scalar);
    emp scale_le_vec3_t  (le_vec3_t * v, f64 scalar);
    emp normal_le_vec3_t (le_vec3_t * v);
    emp set_le_vec3_t_mag(le_vec3_t * v, f64 scalar);
    
    f64 get_le_vec3_t_mag         (le_vec3_t * v);
    f64 get_dot_prod_le_vec3_t    (le_vec3_t * v1, le_vec3_t * v2);
    f64 get_dist_between_le_vec3_t(le_vec3_t * v1, le_vec3_t * v2); 

    le_vec3_t get_cross_prod_le_vec3_t(le_vec3_t * v1, le_vec3_t * v2);
    /* projects v1 on v2 */
    le_vec3_t project_on_le_vec3_t    (le_vec3_t * v1, le_vec3_t * v2);

#endif

