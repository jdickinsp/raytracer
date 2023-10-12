#include <examples.h>

Scene* create_scene_basic() {
    Scene* scene = malloc(sizeof(Scene));
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Vec3 l_position = {0, 4, 2};
    Vec3 l_color = {1, 1, 1};
    PointLight* light = point_light_create(l_position, l_color, 20);
    object_list_add(lights, (Object*)light, PointLightType);

    Material* s_material = material_create(0.1f, 0.3f, 0.6f, true, (Vec3){1, 0, 0}, 0.f);
    Vec3 s_center = {.x = 0, .y = -.1, .z = -1};
    Sphere* sphere = sphere_create(s_center, 0.5, s_material);
    object_list_add(objects, (Object*)sphere, SphereType);

    Material* s_material2 = material_create(0.4, 0.3, 0.6, false, (Vec3){1, 1, 0}, 0);
    Vec3 s_center2 = {.x = .9, .y = -.1, .z = -1};
    Sphere* sphere2 = sphere_create(s_center2, 0.4, s_material2);
    object_list_add(objects, (Object*)sphere2, SphereType);

    Material* s_material3 = material_create(0.4, 0.3, 0.6, false, (Vec3){1, 1, 0}, 1.5);
    Vec3 s_center3 = {.x = -.9, .y = -.1, .z = -1};
    Sphere* sphere3 = sphere_create(s_center3, 0.4, s_material3);
    object_list_add(objects, (Object*)sphere3, SphereType);

    Material* p_material = material_create(0.1, 0.3, 0.6, false, (Vec3){1, 1, 1}, 0);
    p_material->checkerboard = true;
    Vec3 p_center = {.x = 0, .y = 0.5, .z = -1};
    Vec3 p_normal = {.x = 0, .y = 1, .z = 0};
    Plane* plane = plane_create(p_center, p_normal, p_material);
    object_list_add(objects, (Object*)plane, PlaneType);

    scene->lights = lights;
    scene->objects = objects;
    return scene;
}

Scene* create_scene_rand_spheres() {
    // srand(time(NULL));
    Scene* scene = malloc(sizeof(Scene));
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

Scene* create_scene_triangle() {
    Scene* scene = malloc(sizeof(Scene));
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

Scene* create_scene_box() {
    Scene* scene = malloc(sizeof(Scene));
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

Scene* create_scene_with_obj_file() {
    Scene* scene = malloc(sizeof(Scene));
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

Scene* create_scene_with_obj_to_mesh() {
    Scene* scene = malloc(sizeof(Scene));
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Mesh* mesh = load_wavefront_obj_model("./assets/cube.obj");
    Material* material = material_create(0.22, 0.3, 0.2, true, (Vec3){1, 0, 0}, 0);
    // material->texture = texture_load("./assets/12273_Lion_Diffuse.jpg");
    ObjectMesh* obj_mesh = object_mesh_create(mesh, material);
    printf("mesh->vertex_count: %i\n", obj_mesh->mesh->vertex_count);
    Sphere s = obj_mesh->bounding_sphere->sphere;
    printf("sphere: (%f,%f,%f) r:%f\n", s.position.x, s.position.y, s.position.z, s.radius);
    object_list_add(objects, (Object*)obj_mesh, ObjectMeshType);

    Vec3 l_position = {0, 5, 1};
    Vec3 l_color = {1, 1, 1};
    PointLight* light = point_light_create(l_position, l_color, 20);
    object_list_add(lights, (Object*)light, PointLightType);

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

Scene* create_scene_with_texture() {
    Scene* scene = malloc(sizeof(Scene));
    ObjectList* objects = object_list_create();
    ObjectList* lights = object_list_create();

    Vec3 l_position = {0, 4, 2};
    Vec3 l_color = {1, 1, 1};
    PointLight* light = point_light_create(l_position, l_color, 20);
    object_list_add(lights, (Object*)light, PointLightType);

    Material* s_material = material_create(0.5, 0.3, 0.2, true, (Vec3){1, 0, 0}, 0);
    s_material->texture = texture_load("./assets/2k_neptune.jpg");
    Vec3 s_center = {.x = 0, .y = -.1, .z = -1};
    Sphere* sphere = sphere_create(s_center, 0.9, s_material);
    object_list_add(objects, (Object*)sphere, SphereType);

    Material* s_material2 = material_create(0.6, 0.3, 0.2, true, (Vec3){1, 0, 0}, 0);
    s_material2->texture = texture_load("./assets/2k_mars.jpg");
    Vec3 s_center2 = {.x = -1.5, .y = -.1, .z = -4};
    Sphere* sphere2 = sphere_create(s_center2, 0.8, s_material2);
    object_list_add(objects, (Object*)sphere2, SphereType);

    Material* s_material3 = material_create(0.5, 0.3, 0.2, true, (Vec3){1, 0, 0}, 0);
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

Scene* create_scene_with_bvh() {
    Scene* scene = malloc(sizeof(Scene));

    // TriangleBVH triangles[4] = {(TriangleBVH){
    //                                 (Vec3){-1, 1, -1},
    //                                 (Vec3){1, 1, 1},
    //                                 (Vec3){1, 1, -1},
    //                             },
    //                             (TriangleBVH){
    //                                 (Vec3){1, 1, 1},
    //                                 (Vec3){-1, -1, 1},
    //                                 (Vec3){1, -1, 1},
    //                             },
    //                             (TriangleBVH){
    //                                 (Vec3){-1, 1, 1},
    //                                 (Vec3){-1, -1, -1},
    //                                 (Vec3){-1, -1, 1},
    //                             }};
    // BVHPreparedData* data = bvh_prepare_data(triangles, 4);
    // for (int i = 0; i < data->size; i++) {
    //     vec3_debug_print(data->centroids[i]);
    // }

    // Queue* queue = queue_init();
    // printf("queue_init\n");
    // Vec3 array[6] = {(Vec3){1, 2, 3}, (Vec3){2, 0, 0}, (Vec3){3, 0, 0},
    //                  (Vec3){4, 0, 0}, (Vec3){2, 0, 0}, (Vec3){2, 0, 0}};
    // queue_add(queue, &array, sizeof(Vec3) * 6);
    // Vec3 array2[6] = {(Vec3){7, 7, 4}, (Vec3){2, 0, 1}, (Vec3){3, 0, 1},
    //                   (Vec3){4, 0, 1}, (Vec3){2, 0, 1}, (Vec3){2, 0, 1}};
    // queue_add(queue, &array2, sizeof(Vec3) * 6);
    // printf("queue_add\n");
    // Vec3 p_data[6];
    // while (queue->count > 0) {
    //     queue_popleft(queue, &p_data);
    //     for (int i = 0; i < 6; i++) {
    //         vec3_debug_print(p_data[i]);
    //     }
    // }
    // // for (int i = 0; i < 10; i++) {
    // //     Vec3 rand_vec = rand_in_unit_sphere();
    // //     vec3_debug_print(rand_vec);
    // //     queue_add(queue, &rand_vec, sizeof(Vec3));
    // // }
    // queue_free(queue);

    // test binary tree
    float array[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand_range(0, 10000);
    }
    BinaryNode* tree = binary_tree_build(array, ARRAY_SIZE);
    binary_tree_traversal(tree);

    return scene;
}