#include "scope_common.h"

// This file handles the scope shadow (front)

float scope_custom_shadow(Scope s) {
    return smoothstep(s.radius * 0.9, s.radius * 1.1, distance(s.exit_pupil, s.center));
}