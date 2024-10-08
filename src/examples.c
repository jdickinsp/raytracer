#include <examples.h>

Scene* create_scene_basic(RenderOptions* options) {
    Scene* scene = scene_create(options, NULL);
    scene->camera = malloc(sizeof(Camera));
    Vec3 lookfrom = vec3_create(-1.6f, -1.5f, 2.0f);
    options->vfov = 40;
    camera_init(scene->camera, options, &lookfrom, NULL);
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Vec3 l_position = {0, -2, 4};
    Vec3 l_color = {1, 0, 0};
    PointLight* light = point_light_create(l_position, l_color, 50);
    object_list_add(lights, (Object*)light, PointLightType);

    Vec3 l_position2 = {0, -2, -4};
    Vec3 l_color2 = {0, 1, 0};
    PointLight* light2 = point_light_create(l_position2, l_color2, 50);
    object_list_add(lights, (Object*)light2, PointLightType);

    Material* s_material2 = material_create(0.4, 0.3, 0.6, false, (Vec3){1, 1, 0}, 0);
    Vec3 s_center2 = {.x = 1.5, .y = -.1, .z = -1};
    Sphere* sphere2 = sphere_create(s_center2, 0.5, s_material2);
    object_list_add(objects, (Object*)sphere2, SphereType);

    Material* s_material3 = material_create(0.4, 0.3, 0.6, true, (Vec3){1, 1, 0}, 1.5);
    Vec3 s_center3 = {.x = -1.5, .y = -.1, .z = -1};
    Sphere* sphere3 = sphere_create(s_center3, 0.4, s_material3);
    object_list_add(objects, (Object*)sphere3, SphereType);

    Material* s_material4 = material_create(0.4, 0.3, 0.6, false, (Vec3){1, 1, 0}, 0);
    Vec3 s_center4 = {.x = 0, .y = -.1, .z = -1};
    Sphere* sphere4 = sphere_create(s_center4, 0.5, s_material4);
    object_list_add(objects, (Object*)sphere4, SphereType);

    Material* p_material = material_create(0.1, 0.3, 0.6, false, (Vec3){1, 1, 1}, 0);
    p_material->checkerboard = true;
    p_material->scale = 0.5f;
    Vec3 p_center = {.x = 0, .y = 0.5, .z = -1};
    Vec3 p_normal = {.x = 0, .y = 1, .z = 0};
    Plane* plane = plane_create(p_center, p_normal, p_material);
    object_list_add(objects, (Object*)plane, PlaneType);

    scene->lights = lights;
    scene->objects = objects;
    return scene;
}

Scene* create_scene_rand_spheres(RenderOptions* options) {
    // srand(time(NULL));
    Scene* scene = scene_create(options, NULL);
    scene->camera = malloc(sizeof(Camera));
    Vec3 lookfrom = vec3_create(-1.6f, -1.5f, 2.0f);
    camera_init(scene->camera, options, &lookfrom, NULL);
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Vec3 l_position = {0, 4, 2};
    Vec3 l_color = {1, 1, 1};
    PointLight* light = point_light_create(l_position, l_color, 10);
    object_list_add(lights, (Object*)light, PointLightType);

    Vec3 l2_direction = {0, -1, -1};
    Vec3 l2_color = {1, 1, 1};
    DirectionalLight* d_light = directional_light_create(vec3_norm(l2_direction), l2_color, 10);
    object_list_add(lights, (Object*)d_light, DirectionalLightType);

    for (int i = 0; i < 50; i++) {
        Vec3 rand_color = {rand_range(0, 1), rand_range(0, 1), rand_range(0, 1)};
        bool reflective = rand_range(0, 1) > 0.8 ? true : false;
        float ior = rand_range(0, 1) > 0.8 ? 1.5 : 0;
        Material* rand_material =
            material_create(rand_range(0, .9), rand_range(0, .9), rand_range(0, .4), reflective, rand_color, ior);
        Vec3 rand_pos = {rand_range(-1.5, 1.5), 0, rand_range(-8, 0)};
        float rand_radius = rand_range(0.08, 0.18);
        Sphere* sphere = sphere_create(rand_pos, rand_radius, rand_material);
        object_list_add(objects, (Object*)sphere, SphereType);
    }

    Material* p_material = material_create(0.1, 0.3, 0.6, false, (Vec3){1, 1, 0}, 0);
    Vec3 p_center = {.x = 0, .y = 0.5, .z = -1};
    Vec3 p_normal = {.x = 0, .y = 1, .z = 0};
    Plane* plane = plane_create(p_center, p_normal, p_material);
    object_list_add(objects, (Object*)plane, PlaneType);

    scene->objects = objects;
    scene->lights = lights;
    return scene;
}

Scene* create_scene_triangle(RenderOptions* options) {
    Scene* scene = scene_create(options, NULL);
    scene->camera = malloc(sizeof(Camera));
    camera_init(scene->camera, options, NULL, NULL);
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Vec3 l_position = {0, 4, 2};
    Vec3 l_color = {1, 1, 1};
    PointLight* light = point_light_create(l_position, l_color, 20);
    object_list_add(lights, (Object*)light, PointLightType);

    Material* t_material = material_create(0.22, 0.3, 0.2, false, (Vec3){1, 0, 0}, 0);
    Vec3 a = {-0.2, 0, -1};
    Vec3 b = {-0.2, 0.2, -1};
    Vec3 c = {0, 0, -0.7};
    Triangle* triangle = triangle_create(a, b, c, t_material);
    object_list_add(objects, (Object*)triangle, TriangleType);

    scene->lights = lights;
    scene->objects = objects;
    return scene;
}

Scene* create_scene_box(RenderOptions* options) {
    Scene* scene = scene_create(options, NULL);
    scene->camera = malloc(sizeof(Camera));
    camera_init(scene->camera, options, NULL, NULL);
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Vec3 l_position = {0, 5, 1};
    Vec3 l_color = {1, 1, 1};
    PointLight* light = point_light_create(l_position, l_color, 20);
    object_list_add(lights, (Object*)light, PointLightType);

    Vec3 vertex[8] = {{1.000000, 1.000000, -1.000000},  {1.000000, -1.000000, -1.000000},
                      {1.000000, 1.000000, 1.0000000},  {1.000000, -1.000000, 1.000000},
                      {-1.000000, 1.000000, -1.000000}, {-1.000000, -1.000000, -1.000000},
                      {-1.000000, 1.000000, 1.000000},  {-1.000000, -1.000000, 1.000000}};
    int face_index[6] = {4, 4, 4, 4, 4, 4};
    int vertex_index[6 * 4] = {1, 5, 7, 3, 4, 3, 7, 8, 8, 7, 5, 6, 6, 2, 4, 8, 2, 1, 3, 4, 6, 5, 1, 2};

    int face_count = 6;
    int p = 0;
    for (int n = 0; n < face_count; n++) {
        for (int m = 0; m < face_index[n] - 2; m++) {
            int i = vertex_index[p] - 1;
            int j = vertex_index[p + m + 1] - 1;
            int k = vertex_index[p + m + 2] - 1;
            Vec3 color1 = {1, n % 2 == 0 ? 0 : 1, 0};
            Material* material = material_create(0.22, 0.3, 0.2, true, color1, 0);
            Triangle* triangle = triangle_create(vertex[i], vertex[j], vertex[k], material);
            object_list_add(objects, (Object*)triangle, TriangleType);
            printf("c: (%i,%i,%i)\n", i, j, k);
        }
        p += face_index[n];
    }

    Material* p_material = material_create(0.1, 0.3, 0.6, false, (Vec3){1, 1, 0}, 1.3);
    Vec3 p_center = {.x = 0, .y = 1, .z = -1};
    Vec3 p_normal = {.x = 0, .y = 1, .z = 0};
    Plane* plane = plane_create(p_center, p_normal, p_material);
    object_list_add(objects, (Object*)plane, PlaneType);

    scene->lights = lights;
    scene->objects = objects;
    return scene;
}

Scene* create_scene_with_obj_file(RenderOptions* options) {
    Scene* scene = scene_create(options, NULL);
    scene->camera = malloc(sizeof(Camera));
    camera_init(scene->camera, options, NULL, NULL);
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Vec3 l_position = {0, 5, 1};
    Vec3 l_color = {1, 1, 1};
    PointLight* light = point_light_create(l_position, l_color, 20);
    object_list_add(lights, (Object*)light, PointLightType);

    Mesh* mesh = load_wavefront_obj_model("./assets/cube.obj");

    for (int n = 0; n < mesh->vertex_count / 3; n++) {
        int i = n * 3;
        int j = n * 3 + 1;
        int k = n * 3 + 2;
        Vec3 t_color1 = {1, n % 2 == 0 ? 0 : 1, 0};
        Material* t_material1 = material_create(0.5, 0.3, 0.5, true, t_color1, 0);
        Triangle* triangle1 =
            triangle_create_with_normals(mesh->vertices[i], mesh->vertices[j], mesh->vertices[k], t_material1,
                                         mesh->vertex_normals[i], mesh->vertex_normals[j], mesh->vertex_normals[k]);
        object_list_add(objects, (Object*)triangle1, TriangleType);
    }

    Material* p_material = material_create(0.7, 0.3, 0.6, false, (Vec3){1, 1, 1}, 0);
    p_material->checkerboard = true;
    Vec3 p_center = {.x = 0, .y = 1, .z = -1};
    Vec3 p_normal = {.x = 0, .y = 1, .z = 0};
    Plane* plane = plane_create(p_center, p_normal, p_material);
    object_list_add(objects, (Object*)plane, PlaneType);

    scene->lights = lights;
    scene->objects = objects;
    return scene;
}

Scene* create_scene_with_obj_to_mesh(RenderOptions* options) {
    Scene* scene = scene_create(options, NULL);
    scene->camera = malloc(sizeof(Camera));
    Vec3 lookfrom = vec3_create(-1.6f, -1.5f, 6.0f);
    options->vfov = 60.f;
    camera_init(scene->camera, options, &lookfrom, NULL);
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Mesh* mesh = load_wavefront_obj_model("./assets/suzanne.obj");
    Material* material = material_create(0.22, 0.3, 0.02, false, (Vec3){1, 0, 0}, 0);
    material->checkerboard = true;
    material->scale = 0.002f;
    material->texture = texture_load("./assets/2k_mars.jpg");
    Vec3 offset = (Vec3){1.5, 0, 3};
    MeshInfo* mesh_info = object_mesh_create(mesh, material, offset);
    printf("mesh->vertex_count: %i\n", mesh_info->mesh->vertex_count);
    Sphere s = mesh_info->bounding_sphere->sphere;
    printf("sphere: (%f,%f,%f) r:%f\n", s.position.x, s.position.y, s.position.z, s.radius);
    object_list_add(objects, (Object*)mesh_info, MeshInfoType);

    Mesh* mesh2 = load_wavefront_obj_model("./assets/teapot2.obj");
    float ior = 1.5;
    Material* material2 = material_create(0.22, 0.3, 0.2, false, (Vec3){.2, .3, .1}, 0);
    material2->texture = texture_load("./assets/2k_mars.jpg");
    material2->checkerboard = false;
    material2->scale = 0.01f;
    Vec3 rand_offset = {0, 1, -1};
    MeshInfo* mesh_info2 = object_mesh_create(mesh2, material2, rand_offset);
    object_list_add(objects, (Object*)mesh_info2, MeshInfoType);

    Vec3 l_position = {0, -10, -4};
    Vec3 l_color = {1, 0, 1};
    PointLight* light = point_light_create(l_position, l_color, 150);
    object_list_add(lights, (Object*)light, PointLightType);

    Vec3 l_position2 = {0, -10, 4};
    Vec3 l_color2 = {1, 1, 0};
    PointLight* light2 = point_light_create(l_position2, l_color2, 150);
    object_list_add(lights, (Object*)light2, PointLightType);

    Material* p_material = material_create(0.7, 0.3, 0.6, false, (Vec3){1, 1, 1}, 0);
    p_material->checkerboard = true;
    p_material->scale = 0.5f;
    Vec3 p_center = {.x = 0, .y = 1, .z = -1};
    Vec3 p_normal = {.x = 0, .y = 1, .z = 0};
    Plane* plane = plane_create(p_center, p_normal, p_material);
    object_list_add(objects, (Object*)plane, PlaneType);

    Vec3 eye = {2, -2, 4};
    Vec3 center = {0, 0, 0};
    Vec3 up = {0, 1, 0};
    Matrix44 camera_matrix;
    camera_lookat(eye, center, up, &camera_matrix);

    scene->world_matrix = camera_matrix;
    scene->lights = lights;
    scene->objects = objects;
    return scene;
}

Scene* create_scene_with_rand_cubes(RenderOptions* options) {
    Scene* scene = scene_create(options, NULL);
    scene->camera = malloc(sizeof(Camera));
    Vec3 lookfrom = vec3_create(-1.6f, -1.5f, 6.0f);
    camera_init(scene->camera, options, &lookfrom, NULL);
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Mesh* mesh = load_wavefront_obj_model("./assets/cube4.obj");
    Material* material = material_create(0.22, 0.3, 0.2, true, (Vec3){1, 0, 0}, 0);
    // material->checkerboard = true;
    material->scale = 0.001f;
    material->texture = texture_load("./assets/2k_earth_clouds.jpg");
    Vec3 offset = (Vec3){0, 0, 3};
    MeshInfo* mesh_info = object_mesh_create(mesh, material, offset);
    printf("mesh->vertex_count: %i\n", mesh_info->mesh->vertex_count);
    Sphere s = mesh_info->bounding_sphere->sphere;
    printf("sphere: (%f,%f,%f) r:%f\n", s.position.x, s.position.y, s.position.z, s.radius);
    object_list_add(objects, (Object*)mesh_info, MeshInfoType);

    for (int i = 0; i < 50; i++) {
        float ior3 = rand_range(0, 1) > 0.8 ? 1.5 : 0;
        Vec3 rand_color = vec3_rand(0, 1);
        Material* material3 = material_create(0.22, 0.3, 0.2, true, rand_color, ior3);
        material3->checkerboard = true;
        material3->scale = rand_range(0.001f, 0.05f);
        Vec3 rand_offset3 = (Vec3){rand_range(-6, 6), 0, rand_range(0, -15)};
        MeshInfo* mesh_info3 = object_mesh_create(mesh, material3, rand_offset3);
        object_list_add(objects, (Object*)mesh_info3, MeshInfoType);
    }

    Vec3 l_position = {0, 10, 0};
    Vec3 l_color = {1, 1, 1};
    PointLight* light = point_light_create(l_position, l_color, 15);
    object_list_add(lights, (Object*)light, PointLightType);

    Vec3 l_position2 = {0, -10, 0};
    Vec3 l_color2 = {1, 1, 1};
    PointLight* light2 = point_light_create(l_position2, l_color2, 5);
    object_list_add(lights, (Object*)light2, PointLightType);

    Material* p_material = material_create(0.7, 0.3, 0.6, false, (Vec3){.2, .3, .1}, 0);
    p_material->checkerboard = true;
    p_material->scale = 0.5f;
    Vec3 p_center = {.x = 0, .y = 1, .z = -1};
    Vec3 p_normal = {.x = 0, .y = 1, .z = 0};
    Plane* plane = plane_create(p_center, p_normal, p_material);
    object_list_add(objects, (Object*)plane, PlaneType);

    Vec3 eye = {2, -2, 4};
    Vec3 center = {0, 0, 0};
    Vec3 up = {0, 1, 0};
    Matrix44 camera_matrix;
    camera_lookat(eye, center, up, &camera_matrix);

    scene->world_matrix = camera_matrix;
    scene->lights = lights;
    scene->objects = objects;
    return scene;
}

Scene* create_scene_with_texture(RenderOptions* options) {
    Scene* scene = scene_create(options, NULL);
    scene->camera = malloc(sizeof(Camera));
    Vec3 lookfrom = {0, 0, 4.0f};
    camera_init(scene->camera, options, &lookfrom, NULL);
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Vec3 l_position = {0, 4, 2};
    Vec3 l_color = {1, 1, 1};
    PointLight* light = point_light_create(l_position, l_color, 100);
    object_list_add(lights, (Object*)light, PointLightType);

    Material* s_material = material_create(0.5, 0.3, 0.2, false, (Vec3){1, 0, 0}, 0);
    s_material->texture = texture_load("./assets/2k_neptune.jpg");
    Vec3 s_center = {.x = 0, .y = -.1, .z = -1};
    Sphere* sphere = sphere_create(s_center, 0.9, s_material);
    object_list_add(objects, (Object*)sphere, SphereType);

    Material* s_material2 = material_create(0.6, 0.3, 0.2, false, (Vec3){1, 0, 0}, 0);
    s_material2->texture = texture_load("./assets/2k_mars.jpg");
    Vec3 s_center2 = {.x = -1.5, .y = -.1, .z = -4};
    Sphere* sphere2 = sphere_create(s_center2, 0.8, s_material2);
    object_list_add(objects, (Object*)sphere2, SphereType);

    Material* s_material3 = material_create(0.5, 0.3, 0.2, false, (Vec3){1, 0, 0}, 0);
    s_material3->texture = texture_load("./assets/2k_mercury.jpg");
    Vec3 s_center3 = {.x = 3.0, .y = -.1, .z = -3};
    Sphere* sphere3 = sphere_create(s_center3, 0.8, s_material3);
    object_list_add(objects, (Object*)sphere3, SphereType);

    Material* p_material = material_create(0.7, 0.3, 0.6, false, (Vec3){1, 1, 1}, 0);
    p_material->checkerboard = true;
    Vec3 p_center = {.x = 0, .y = 0.5, .z = -1};
    Vec3 p_normal = {.x = 0, .y = 1, .z = 0};
    Plane* plane = plane_create(p_center, p_normal, p_material);
    object_list_add(objects, (Object*)plane, PlaneType);

    scene->lights = lights;
    scene->objects = objects;
    return scene;
}

#define ARRAY_SIZE 2000

Scene* create_scene_with_binary_tree(RenderOptions* options) {
    // test binary tree
    float array[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand_range(0, 10000);
    }
    BinaryNode* tree = binary_tree_build(array, ARRAY_SIZE);
    binary_tree_pprint(tree);

    Scene* scene = scene_create(options, NULL);
    return scene;
}

Scene* create_scene_with_bvh(RenderOptions* options) {
    BVTriangle triangles[12] = {(BVTriangle){
                                    (Vec3){-1, 1, -1},
                                    (Vec3){1, 1, 1},
                                    (Vec3){1, 1, -1},
                                },
                                (BVTriangle){
                                    (Vec3){1, 1, 1},
                                    (Vec3){-1, -1, 1},
                                    (Vec3){1, -1, 1},
                                },
                                (BVTriangle){
                                    (Vec3){-1, 1, 1},
                                    (Vec3){-1, -1, -1},
                                    (Vec3){-1, -1, 1},
                                },

                                (BVTriangle){
                                    (Vec3){1, -1, -1},
                                    (Vec3){-1, -1, 1},
                                    (Vec3){-1, -1, -1},
                                },
                                (BVTriangle){
                                    (Vec3){1, 1, 1},
                                    (Vec3){1, -1, 1},
                                    (Vec3){1, -1, -1},
                                },
                                (BVTriangle){
                                    (Vec3){-1, 1, -1},
                                    (Vec3){1, -1, -1},
                                    (Vec3){-1, -1, -1},
                                },

                                (BVTriangle){
                                    (Vec3){-1, 1, -1},
                                    (Vec3){-1, 1, 1},
                                    (Vec3){1, 1, 1},
                                },
                                (BVTriangle){
                                    (Vec3){1, 1, 1},
                                    (Vec3){-1, 1, 1},
                                    (Vec3){-1, -1, 1},
                                },
                                (BVTriangle){
                                    (Vec3){-1, 1, 1},
                                    (Vec3){-1, 1, -1},
                                    (Vec3){-1, -1, -1},
                                },

                                (BVTriangle){
                                    (Vec3){1, -1, -1},
                                    (Vec3){1, -1, 1},
                                    (Vec3){-1, -1, 1},
                                },
                                (BVTriangle){
                                    (Vec3){1, 1, -1},
                                    (Vec3){1, 1, 1},
                                    (Vec3){1, -1, 1},
                                },
                                (BVTriangle){
                                    (Vec3){-1, 1, -1},
                                    (Vec3){1, 1, -1},
                                    (Vec3){1, -1, -1},
                                }

    };

    for (int i = 0; i < 12; i++) {
        vec3_debug_print(triangles[i].v1);
        vec3_debug_print(triangles[i].v2);
        vec3_debug_print(triangles[i].v3);
    }

    Primitives* primitives = bvh_prepare_data(triangles, 12);
    BVHNode* bvh_tree = bvh_build_tree(primitives);
    bvh_pprint(bvh_tree);
    bvh_traverse_tree(bvh_tree);

    Vec3 origin = {0, 0, 2};
    Ray ray;
    BVHitInfo bv_hit;
    find_ray_from_triangle(origin, &triangles[0], &ray);
    printf("hit_info: %i\n", bv_hit.has_hit);

    Scene* scene = scene_create(options, NULL);
    return scene;
}

Scene* create_scene_with_bvh_from_obj(RenderOptions* options) {
    // Mesh* mesh = load_wavefront_obj_model("./assets/12273_Lion_v1_l3.obj");
    Mesh* mesh = load_wavefront_obj_model("./assets/cube4.obj");

    int n_size = mesh->vertex_count / 3;
    BVTriangle* triangles = malloc(sizeof(BVTriangle) * n_size);
    for (int n = 0; n < mesh->vertex_count / 3; n++) {
        int i = n * 3;
        int j = n * 3 + 1;
        int k = n * 3 + 2;
        triangles[n] = (BVTriangle){mesh->vertices[i], mesh->vertices[j], mesh->vertices[k]};
    }

    Primitives* primitives = bvh_prepare_data(triangles, n_size);
    BVHNode* bvh_tree = bvh_build_tree(primitives);
    Scene* scene = scene_create(options, NULL);
    return scene;
}
