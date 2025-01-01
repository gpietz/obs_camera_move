#pragma once

#include <obs-module.h>

OBS_DECLARE_MODULE();
OBS_MODULE_USE_DEFAULT_LOCALE("ObsMoveCamera", nullptr);

bool obs_module_load();
void obs_module_unload();
