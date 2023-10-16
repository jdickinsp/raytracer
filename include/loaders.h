#ifndef LOADERS_H
#define LOADERS_H

#include <scene.h>
#include <stdio.h>
#include <string.h>
#include <vectors.h>

typedef enum { OBJ_FACE_INDEX_TYPE_1, OBJ_FACE_INDEX_TYPE_2, OBJ_FACE_INDEX_TYPE_3 } ObjFaceIndexType;

int buffer_offset(char *buffer, char match);
Mesh *load_wavefront_obj_model(const char *file_path);

#endif
