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

Scene *create_scene_basic();
Scene *create_scene_rand_spheres();
Scene *create_scene_triangle();
Scene *create_scene_box();
Scene *create_scene_with_obj_file();
Scene *create_scene_with_obj_to_mesh();
Scene *create_scene_with_texture();

#endif
