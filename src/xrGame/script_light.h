#pragma once
#include "script_export_space.h"
#include "script_light_inline.h"

add_to_type_list(ScriptLight)
#undef script_type_list
#define script_type_list save_type_list(ScriptLight)