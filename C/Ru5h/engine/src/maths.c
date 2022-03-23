#include <maths.h>
#include <types.h>
#include <math.h>

f64 le_dist_2d(f64 x1, f64 y1, f64 x2, f64 y2) {
    return sqrt(le_sqr(le_dist_1d(x1, x2)) + le_sqr(le_dist_1d(y1, y2)));
}

f64 le_dir_2d(f64 x1, f64 y1, f64 x2, f64 y2) {
    return atan2(y2 - y1, x2 - x1);  
}

f64 le_inv_sqrt(f64 num) {
    i64 i;
    f64 x2, y;

    x2 = num * 0.5f;
    y = num;
    i = * (i64 *) &y;
    i = 0x5f3759df - (i >> 1);
    y = * (f64 *) &i;
    y = y * (1.5f - (x2 * le_sqr(y)));

    return y; 
}

f64 le_lerp_f(f64 start, f64 stop, f64 amt) {
    return start + (stop - start) * amt; 
}

emp init_le_vec2_t(le_vec2_t * v, f64 x, f64 y) {
    v -> x = x;
    v -> y = y;
}

emp add_le_vec2_t(le_vec2_t * v1, le_vec2_t * v2) {
    v1 -> x += v2 -> x;
    v1 -> y += v2 -> y;
}

emp sub_le_vec2_t(le_vec2_t * v1, le_vec2_t * v2) {
    v1 -> x = v2 -> x - v1 -> x;
    v1 -> y = v2 -> y - v1 -> y;
}

emp lerp_le_vec2_t(le_vec2_t * v1, le_vec2_t * v2, f64 amt) {
    v1 -> x = le_lerp_f(v1 -> x, v2 -> x, amt);
    v1 -> y = le_lerp_f(v1 -> y, v2 -> y, amt);
}

emp limit_le_vec2_t(le_vec2_t * v, f64 scalar) {
    f64 len = v -> x * v -> x + v -> y * v -> y;
    f64 len_t = scalar * scalar;
    if (len > len_t) {
        normal_le_vec2_t(v);
        scale_le_vec2_t(v, scalar);
    }        
}

emp scale_le_vec2_t(le_vec2_t * v, f64 scalar) {
    v -> x *= scalar;
    v -> y *= scalar;
}

emp normal_le_vec2_t(le_vec2_t * v) {
    f64 mag = get_le_vec2_t_mag(v);
    v -> x = v -> x / mag;
    v -> y = v -> y / mag;   
}

emp rotate_le_vec2_t (le_vec2_t * v, f64 theta) {
    f64 temp = v -> x;
    v -> x = v -> x * cos(theta) - v -> y * sin(theta);
    v -> y = temp * sin(theta) + v -> y * cos(theta);
}

emp set_le_vec2_t_mag(le_vec2_t * v, f64 scalar) {
    normal_le_vec2_t(v);
    scale_le_vec2_t(v, scalar);
}

f64 get_le_vec2_t_mag(le_vec2_t * v) {
    return (sqrt(v -> x * v -> x + v -> y * v -> y)); 
}

f64 get_angle_le_vec2_t(le_vec2_t * v) {
    return atan2(v -> y, v -> x);
}

f64 get_dot_prod_le_vec2_t(le_vec2_t * v1, le_vec2_t * v2) {
    return v1 -> x * v2 -> x + v1 -> y * v2 -> y; 
}

f64 get_dist_between_le_vec2_t(le_vec2_t * v1, le_vec2_t * v2) {
    f64 dx = v1 -> x - v2 -> x;
    f64 dy = v1 -> y - v2 -> y;
    return sqrt(dx * dx + dy * dy);
}

f64 get_angle_of_two_le_vec2_t(le_vec2_t * v1, le_vec2_t * v2) {
    if (v1 -> x == 0 && v1 -> y == 0) return 0.0f;
    if (v2 -> x == 0 && v2 -> y == 0) return 0.0f;

    f64 dot = get_dot_prod_le_vec2_t(v1, v2);
    f64 v1mag = get_le_vec2_t_mag(v1);
    f64 v2mag = get_le_vec2_t_mag(v2);
    f64 amt = dot / (v1mag * v2mag);

    if (amt <= -1) {
        return M_PI;
    } else if (amt >= 1) {
        return 0;
    }
    return acos(amt);
}

le_vec2_t project_on_le_vec2_t(le_vec2_t * v1, le_vec2_t * v2) {
    f64 dot = get_dot_prod_le_vec2_t(v1, v2);
    f64 len = get_dot_prod_le_vec2_t(v2, v2);
    if (len == 0.0f) return *v2;

    le_vec2_t return_val = *v2;
    scale_le_vec2_t(&return_val, dot / len);
    return return_val;
}

emp init_le_vec3_t(le_vec3_t * v, f64 x, f64 y, f64 z) {
    v -> x = x;
    v -> y = y;
    v -> z = z;
}

emp add_le_vec3_t(le_vec3_t * v1, le_vec3_t * v2) {
    v1 -> x += v2 -> x;
    v1 -> y += v2 -> y;
    v1 -> z += v2 -> z;
}

emp sub_le_vec3_t(le_vec3_t * v1, le_vec3_t * v2) {
    v1 -> x = v2 -> x - v1 -> x;
    v1 -> y = v2 -> y - v2 -> y;
    v1 -> z = v2 -> z - v2 -> z;
}

emp lerp_le_vec3_t(le_vec3_t * v1, le_vec3_t * v2, f64 amt) {
    v1 -> x = le_lerp_f(v1 -> x, v2 -> x, amt);
    v1 -> y = le_lerp_f(v1 -> y, v2 -> y, amt);   
    v1 -> z = le_lerp_f(v1 -> z, v2 -> z, amt);
}

emp limit_le_vec3_t(le_vec3_t * v, f64 scalar) {
    f64 len = v -> x * v -> x + v -> y * v -> y;
    f64 len_t = scalar * scalar;
    if (len > len_t) {
        normal_le_vec3_t(v);
        scale_le_vec3_t(v, scalar);
    }   
}

emp scale_le_vec3_t(le_vec3_t * v, f64 scalar) {
    v -> x *= scalar;
    v -> y *= scalar;
    v -> z *= scalar;
}

emp normal_le_vec3_t(le_vec3_t * v) {
    f64 mag = get_le_vec3_t_mag(v);
    v -> x = v -> x / mag;
    v -> y = v -> y / mag;
    v -> z = v -> z / mag;
}

emp set_le_vec3_t_mag(le_vec3_t * v, f64 scalar) {
    normal_le_vec3_t(v);
    scale_le_vec3_t(v, scalar);
}

f64 get_le_vec3_t_mag(le_vec3_t * v) {
    return (sqrt(v -> x * v -> x + v -> y * v -> y + v -> z * v -> z)); 
}

f64 get_dot_prod_le_vec3_t(le_vec3_t * v1, le_vec3_t * v2) {
    return v1 -> x * v2 -> x + v1 -> y * v2 -> y + v1 -> z * v2 -> z;  
}

f64 get_dist_between_le_vec3_t(le_vec3_t * v1, le_vec3_t * v2) {
    f64 dx = v2 -> x - v1 -> x;
    f64 dy = v2 -> y - v1 -> y;
    f64 dz = v2 -> z - v1 -> z;
    return sqrt(dx * dx + dy * dy + dz * dz); 
}

le_vec3_t get_cross_prod_le_vec3_t(le_vec3_t * v1, le_vec3_t * v2) {
    le_vec3_t v_o;
    v_o.x = v1 -> y * v2 -> z - v1 -> z * v2 -> y;
    v_o.y = v1 -> z * v2 -> x - v1 -> z * v2 -> z;
    v_o.z = v1 -> x * v2 -> y - v1 -> y * v2 -> x;
    return v_o;
}


le_vec3_t project_on_le_vec3_t(le_vec3_t * v1, le_vec3_t * v2) {
    f64 dot = get_dot_prod_le_vec3_t(v1, v2);
    f64 len = get_dot_prod_le_vec3_t(v2, v2);
    if (len == 0.0f) return *v2;

    le_vec3_t return_val = *v2;
    scale_le_vec3_t(&return_val, dot / len);
    return return_val;   
}

