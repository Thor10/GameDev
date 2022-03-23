#pragma once
#include <math.h>

#define square(expression) expression * expression

#define distance_between_1d_points(c1, c2) c2 - c1

#define distance_between_2d_points(x1, y1, x2, y2) sqrt(square(distance_between_1d_points(x1, x2)) \
			    	                      + square(distance_between_1d_points(y1, y2)))

#define direction_between_points(x1, y1, x2, y2) atan2(distance_between_1d_points(y1, y2), \
						       distance_between_1d_points(x1, x2))

#define deg_to_rad(deg) deg * (M_PI / 180)
#define rad_to_deg(rad) rad * (180 / M_PI)

#define PI 3.14159265358979
#define TAU 6.283184

typedef struct {
    float x, y;
} vec2;

float lerp_f(float start, float stop, float amt) {
    return start + (stop-start) * amt;
}

void add_vec2(vec2 * vec_one, vec2 * vec_two) {
    vec_one -> x = vec_two -> x + vec_one -> x;
    vec_one -> y = vec_two -> y + vec_one -> y;
}

void sub_vec2(vec2 * vec_one, vec2 * vec_two) {
    vec_one -> x = vec_two -> x - vec_one -> x;
    vec_one -> y = vec_two -> y - vec_one -> y;
}

double get_vec2_mag(vec2 * vec) {
    return (sqrt(vec -> x * vec -> x + vec -> y * vec -> y)); 
}

void normalize_vec2(vec2 * vec, double mag) {
    vec -> x = vec -> x / mag;
    vec -> y = vec -> y / mag;
}

void multiply_vec2_by_scalar(vec2 * vec, float scalar) {
    vec -> x = vec -> x * scalar;
    vec -> y = vec -> y * scalar;
}

void divide_vec2_by_scalar(vec2 * vec, float scalar) {
    vec -> x = vec -> x / scalar;
    vec -> y = vec -> y / scalar;
}

void set_vec2_mag(vec2 * vec, float scalar) {
    normalize_vec2(vec, get_vec2_mag(vec));
    multiply_vec2_by_scalar(vec, scalar);
}

float get_angle_of_vec2(vec2 * vec) {
    return atan2(vec -> y, vec -> x);
}

void rotate_vec2(vec2 * vec, float theta) {
    float temp = vec -> x;
    /* Might need to check for rounding errors like with angleBetween function? */
    vec -> x = vec -> x * cos(theta) - vec -> y * sin(theta);
    vec -> y = temp * sin(theta) + vec -> y * cos(theta);
}

void lerp_vec2(vec2 * vec_one, vec2 * vec_two, float amt) {
    vec_one -> x = lerp_f(vec_one -> x, vec_two -> x, amt);
    vec_one -> y = lerp_f(vec_one -> y, vec_two -> y, amt);
}

float get_distance_between_two_vec2(vec2 * vec_one, vec2 * vec_two) {
    float dx = vec_one -> x - vec_two -> x;
    float dy = vec_one -> y - vec_two -> y;
    return sqrt(dx * dx + dy * dy);
}

float get_dot_product_vec2(vec2 * vec_one, vec2 * vec_two) {
    return vec_one -> x * vec_two -> x + vec_one -> y * vec_two -> y;
}

float get_angle_between_two_vec2(vec2 * v1, vec2 * v2) {
    if (v1 -> x == 0 && v1 -> y == 0) return 0.0f;
    if (v2 -> x == 0 && v2 -> y == 0) return 0.0f;

    double dot = v1 -> x * v2 -> x + v1 -> y * v2 -> y;
    double v1mag = get_vec2_mag(v1);
    double v2mag = get_vec2_mag(v2);
    double amt = dot / (v1mag * v2mag);

    if (amt <= -1) {
      return M_PI;
    } else if (amt >= 1) {
      return 0;
    }
    return acos(amt);
}


void limit_vec2(vec2 * vec, float scalar) {
    double len = vec -> x * vec -> x + vec -> y * vec -> y;
    double len_t = scalar * scalar;
    if (len > len_t) {
        double mag = get_vec2_mag(vec);
        normalize_vec2(vec, mag);
        multiply_vec2_by_scalar(vec, scalar);
    }     
}
