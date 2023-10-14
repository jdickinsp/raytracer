#ifndef EXAMPLES_H
#define EXAMPLES_H

#include <binary_tree.h>
#include <bvh.h>
#include <camera.h>
#include <collections.h>
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
Scene *create_scene_with_binary_tree(void);
Scene *create_scene_with_bvh(void);

#endif
