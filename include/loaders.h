#ifndef LOADERS_H
#define LOADERS_H

#include <scene.h>
#include <stdio.h>
#include <string.h>
#include <vectors.h>

Mesh *load_wavefront_obj_model(const char *file_path);

#endif
