#ifndef EXAMPLES_H
#define EXAMPLES_H

#include <binary_tree.h>
#include <bvh.h>
#include <camera.h>
#include <collections.h>
#include <constants.h>
#include <loaders.h>
#include <material.h>
#include <math.h>
#include <matrix44.h>
#include <object_mesh.h>
#include <scene.h>
#include <shared.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include <vectors.h>

Scene *create_scene_basic(RenderOptions *options);
Scene *create_scene_rand_spheres(RenderOptions *options);
Scene *create_scene_triangle(RenderOptions *options);
Scene *create_scene_box(RenderOptions *options);
Scene *create_scene_with_obj_file(RenderOptions *options);
Scene *create_scene_with_obj_to_mesh(RenderOptions *options);
Scene *create_scene_with_texture(RenderOptions *options);
Scene *create_scene_with_binary_tree(RenderOptions *options);
Scene *create_scene_with_bvh(RenderOptions *options);
Scene *create_scene_with_bvh_from_obj(RenderOptions *options);

#endif
