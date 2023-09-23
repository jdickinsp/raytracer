#ifndef EXAMPLES_H
#define EXAMPLES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <camera.h>
#include <loaders.h>
#include <material.h>
#include <matrix44.h>
#include <vectors.h>
#include <scene.h>
#include <utils.h>

Scene *create_scene_basic(void);
Scene *create_scene_rand_spheres(void);
Scene *create_scene_triangle(void);
Scene *create_scene_box(void);
Scene *create_scene_with_obj_file(void);
Scene *create_scene_with_obj_to_mesh(void);
Scene *create_scene_with_texture(void);

#endif
