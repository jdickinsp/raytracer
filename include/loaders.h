#ifndef LOADERS_H
#define LOADERS_H

#include <scene.h>
#include <stdio.h>
#include <string.h>
#include <vectors.h>

int buffer_offset(char *buffer, char match);
Mesh *load_wavefront_obj_model(const char *file_path);

#endif
