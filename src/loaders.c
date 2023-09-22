#include <loaders.h>

Mesh *load_wavefront_obj_model(const char *file_path) {
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        fprintf(stderr, "Unable to open file\n");
        exit(1);
    }
    Mesh *mesh = malloc(sizeof(Mesh));

    int c = 0;
    int c2 = 0;
    int idx_v = 0;
    int idx_n = 0;
    int idx_t = 0;
    int idx_f = 0;
    int idx_fv = 0;

    float fx, fy, fz;
    int fi, fj, fk;

    // first pass: count total vertex and faces
    while (!feof(fp)) {
        c = getc(fp);
        if (c == '#') { // ignore comment
            while (c != '\n') { c = getc(fp); }
        }
        if (c == 'u') { // ignore usemtl
            while (c != '\n') { c = getc(fp); }
        }
        if (c == 'v') {
            c2 = getc(fp);
            if (c == 'v' && c2 == ' ') { // vertices
                fscanf(fp, "%f %f %f", &fx, &fy, &fz);
                idx_v++;
            }
            if (c == 'v' && c2 == 'n') { // normals
                fscanf(fp, "%f %f %f", &fx, &fy, &fz);
                idx_n++;
            }
             if (c == 'v' && c2 == 't') { // texture coordinates
                fscanf(fp, "%f %f", &fx, &fy);
                idx_t++;
            }
        }
        if (c == 'f') { // face indexes
            while (c != '\n') {
                fscanf(fp, "%i/%i/%i", &fi, &fj, &fk);
                c = getc(fp);
                if (feof(fp)) {
                    break;
                }
                if (c == ' ') {
                    c = getc(fp);
                    if (c != '\n') {
                        ungetc(c, fp);
                    }
                }
                idx_f++;
            }
            idx_fv++;
        }
    }

    int *face_index = malloc(sizeof(int) * idx_fv);
    int *vertex_index = malloc(sizeof(int) * idx_f);
    int *normal_index = malloc(sizeof(int) * idx_f);
    int *texture_index = malloc(sizeof(int) * idx_f);
    Vec3 *fvertices = malloc(sizeof(Vec3) * idx_v);
    Vec3 *fnormals = malloc(sizeof(Vec3) * idx_n);
    Vec2 *ftextures = malloc(sizeof(Vec2) * idx_t);

    idx_v = 0;
    idx_n = 0;
    idx_t = 0;
    idx_fv = 0;
    idx_f = 0;

    fseek(fp, 0, SEEK_SET);

    // second pass: collect object data
    while (!feof(fp)) {
        c = getc(fp);
        if (c == '#') { // ignore comment
            while (c != '\n') { c = getc(fp); }
        }
        if (c == 'u') { // ignore usemtl
            while (c != '\n') { c = getc(fp); }
        }
        if (c == 'v') {
            c2 = getc(fp);
            if (c == 'v' && c2 == ' ') { // vertices
                fscanf(fp, "%f %f %f", &fx, &fy, &fz);
                fvertices[idx_v] = (Vec3) {fx, fy, fz};
                idx_v++;
            }
            if (c == 'v' && c2 == 'n') { // normals
                fscanf(fp, "%f %f %f", &fx, &fy, &fz);
                fnormals[idx_n] = (Vec3) {fx, fy, fz};
                idx_n++;
            }
             if (c == 'v' && c2 == 't') { // texture coordinates
                fscanf(fp, "%f %f", &fx, &fy);
                ftextures[idx_t] = (Vec2) {fx, fy};
                idx_t++;
            }
        }
        if (c == 'f') { // face indexes
            int fi, fj, fk;
            int face_vertex_count = 0;
            while (c != '\n') {
                // vertex_index / texture_index / normal_index
                fscanf(fp, "%i/%i/%i", &fi, &fj, &fk); 
                c = getc(fp);
                if (feof(fp)) {
                    break;
                }
                if (c == ' ') {
                    c = getc(fp);
                    if (c != '\n') {
                        ungetc(c, fp);
                    }
                }
                vertex_index[idx_f] = fi - 1;
                texture_index[idx_f] = fj - 1;
                normal_index[idx_f] = fk - 1;

                idx_f++;
                face_vertex_count++;
            }
            face_index[idx_fv] = face_vertex_count;
            idx_fv++;
        }
    }

    const int face_count = idx_fv;
    int max_vertices = 0;
    for (int n=0; n < face_count; n++) {
        max_vertices += (face_index[n] - 2) * 3;
    }
    printf("face_count: %d\n", face_count);
    printf("max_vertices: %d\n", max_vertices);

    Vec3 *vertices = malloc(sizeof(Vec3) * max_vertices);
    Vec3 *vertex_normals = malloc(sizeof(Vec3) * max_vertices);
    Vec2 *textures = malloc(sizeof(Vec2) * max_vertices);
    Vec3 *face_normals = malloc(sizeof(Vec3) * max_vertices / 3);

    // transfer data to model
    int t = 0;
    int p = 0;
    for (int n=0; n < face_count; n++) {
        for (int m=0; m < face_index[n] - 2; m++) {
            int total = face_index[n];
            int i = p;
            int j = p + m + 1;
            int k = p + m + 2;

            int vi = vertex_index[i];
            int vj = vertex_index[j];
            int vk = vertex_index[k];
            vertices[t] = fvertices[vi];
            vertices[t+1] = fvertices[vj];
            vertices[t+2] = fvertices[vk];

            int ni = normal_index[i];
            int nj = normal_index[j];
            int nk = normal_index[k];
            vertex_normals[t] = fnormals[ni];
            vertex_normals[t+1] = fnormals[nj];
            vertex_normals[t+2] = fnormals[nk];

            int ti = texture_index[i];
            int tj = texture_index[j];
            int tk = texture_index[k];
            textures[t] = ftextures[ti];
            textures[t+1] = ftextures[tj];
            textures[t+2] = ftextures[tk];
            t += 3;
        }
        p += face_index[n];
    }

    // calculate face normals
    mesh->face_count = face_count;
    mesh->vertex_count = max_vertices;
    mesh->face_index = face_index;
    mesh->vertex_index = vertex_index;
    mesh->vertices = vertices;
    mesh->vertex_normals = vertex_normals;
    mesh->texture_uv = textures;
    mesh_set_face_normals(mesh);

    fclose(fp);
    free(fvertices);
    free(fnormals);
    free(ftextures);

    return mesh;
}
