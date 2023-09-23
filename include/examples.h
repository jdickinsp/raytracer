#ifndef EXAMPLES_H
#define EXAMPLES_H

#include <camera.h>
#include <loaders.h>
#include <material.h>
#include <math.h>
#include <matrix44.h>
#include <scene.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include <vectors.h>

Scene *create_scene_basic(void);
Scene *create_scene_rand_spheres(void);
Scene *create_scene_triangle(void);
Scene *create_scene_box(void);
Scene *create_scene_with_obj_file(void);
Scene *create_scene_with_obj_to_mesh(void);
Scene *create_scene_with_texture(void);

#endif
