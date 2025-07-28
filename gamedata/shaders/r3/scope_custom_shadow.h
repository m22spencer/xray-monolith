#include "scope_common.h"

// This file handles the scope shadow (front)

float scope_custom_shadow(Scope s) {
    return smoothstep(.45, .6, distance(s.exit_pupil, float2(0.5, 0.5)));
}