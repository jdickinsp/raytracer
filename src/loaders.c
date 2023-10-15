#include <loaders.h>

Mesh *load_wavefront_obj_model(const char *file_path) {
    FILE *fp;
    fopen_s(&fp, file_path, "rb");
    if (!fp) {
        fprintf(stderr, "Unable to open obj model %s\n", file_path);
        exit(1);
    }
    fseek(fp, 0L, SEEK_END);
    int offset = 0;
    int buffer_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    printf("buffer_size: %i\n", buffer_size);
    char *buffer = malloc(buffer_size + 1);
    int bytesread = fread_s(buffer, buffer_size, 1, buffer_size, fp);
    fclose(fp);
    if (bytesread != buffer_size) {
        fprintf(stderr, "Failed to copy file to buffer");
        abort();
    }
    // add EOF in case it's missing
    buffer[buffer_size - 1] = '\0';
    // printf("buffer: %s\n", buffer);
    char sub_buffer[32];

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
    while (offset < buffer_size) {
        c = buffer[offset++];
        if (c == '#') {  // ignore comment
            while (c != '\n') {
                c = buffer[offset++];
            }
        }
        if (c == 'u') {  // ignore usemtl
            while (c != '\n') {
                c = buffer[offset++];
            }
        }
        if (c == 'v') {
            c2 = buffer[offset++];
            if (c == 'v' && c2 == ' ') {  // vertices
                offset += 20;
                idx_v++;
            }
            if (c == 'v' && c2 == 'n') {  // normals;
                offset += 20;
                idx_n++;
            }
            if (c == 'v' && c2 == 't') {  // texture coordinates
                offset += 20;
                idx_t++;
            }
        }
        if (c == 'f') {  // face indexes
            while (c != '\n') {
                c = buffer[offset++];
                if (offset >= buffer_size) {
                    printf("offset overflow\n");
                    break;
                }
                char peek = buffer[offset + 1];
                if (peek == '\n') {
                    break;
                }
                if (c == ' ') {
                    offset += 5;
                    idx_f++;
                }
            }
            idx_fv++;
        }
    }
    printf("indexes: idx_fv:%i, %i, %i, %i, %i\n", idx_fv, idx_f, idx_v, idx_n, idx_t);

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

    offset = 0;

    // second pass: collect object data
    while (offset < buffer_size) {
        c = buffer[offset++];
        if (c == '#') {  // ignore comment
            while (c != '\n') {
                c = buffer[offset++];
            }
        }
        if (c == 'u') {  // ignore usemtl
            while (c != '\n') {
                c = buffer[offset++];
            }
        }
        if (c == 'v') {
            c2 = buffer[offset++];
            if (c == 'v' && c2 == ' ') {  // vertices
                // memcpy(&sub_buffer, &buffer[offset], 32);
                // sscanf_s(sub_buffer, "%f %f %f", &fx, &fy, &fz);
                char *next = &buffer[offset];
                fx = atof(next);
                next = strchr(next, ' ') + 1;
                fy = atof(next);
                next = strchr(next, ' ') + 1;
                fz = atof(next);
                fvertices[idx_v] = (Vec3){fx, fy, fz};
                // printf("off: %i\n", off);
                // printf("fx: %f, %f, %f\n", fx, fy, fz);
                offset += 20;
                idx_v++;
            }
            if (c == 'v' && c2 == 'n') {  // normals
                // memcpy(&sub_buffer, &buffer[offset], 32);
                // sscanf_s(sub_buffer, "%f %f %f", &fx, &fy, &fz);
                char *next = &buffer[offset] + 1;
                fx = atof(next);
                next = strchr(next, ' ') + 1;
                fy = atof(next);
                next = strchr(next, ' ') + 1;
                fz = atof(next);
                fnormals[idx_n] = (Vec3){fx, fy, fz};
                // next = strchr(next, '\n');
                // int off = next - &buffer[offset];
                // printf("fx: %f, %f, %f\n", fx, fy, fz);
                offset += 20;
                idx_n++;
            }
            if (c == 'v' && c2 == 't') {  // texture coordinates
                // memcpy(&sub_buffer, &buffer[offset], 32);
                // sscanf_s(sub_buffer, "%f %f %f", &fx, &fy, &fz);
                char *next = &buffer[offset] + 1;
                fx = atof(next);
                next = strchr(next, ' ') + 1;
                fy = atof(next);
                ftextures[idx_t] = (Vec2){fx, fy};
                // next = strchr(next, '\n');
                // int off = next - &buffer[offset];
                // printf("fx: %f, %f\n", fx, fy);
                offset += 20;
                idx_t++;
            }
        }

        if (c == 'f') {  // face indexes
            int fi, fj, fk;
            int face_vertex_count = 0;
            while (c != '\n') {
                c = buffer[offset++];
                if (offset >= buffer_size) {
                    printf("offset overflow\n");
                    break;
                }
                char peek = buffer[offset + 1];
                if (peek == '\n') {
                    break;
                }
                if (c == ' ') {
                    // memcpy(&sub_buffer, &buffer[offset], 32);
                    // sscanf_s(sub_buffer, "%i/%i/%i", &fi, &fj, &fk);
                    char *next = &buffer[offset];
                    fi = atoi(next);
                    next = strchr(next, '/') + 1;
                    fj = atoi(next);
                    next = strchr(next, '/') + 1;
                    fk = atoi(next);
                    // next = strchr(next, '\n');
                    // int off = next - &buffer[offset];
                    // printf("off: %i\n", off);
                    // printf("fx: %i, %i, %i\n", fi, fj, fk);
                    vertex_index[idx_f] = fi - 1;
                    texture_index[idx_f] = fj - 1;
                    normal_index[idx_f] = fk - 1;
                    offset += 5;
                    idx_f++;
                    face_vertex_count++;
                }
            }
            face_index[idx_fv] = face_vertex_count;
            idx_fv++;
        }
    }

    const int face_count = idx_fv;
    int max_vertices = 0;
    for (int n = 0; n < face_count; n++) {
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
    for (int n = 0; n < face_count; n++) {
        for (int m = 0; m < face_index[n] - 2; m++) {
            int total = face_index[n];
            int i = p;
            int j = p + m + 1;
            int k = p + m + 2;

            int vi = vertex_index[i];
            int vj = vertex_index[j];
            int vk = vertex_index[k];
            vertices[t] = fvertices[vi];
            vertices[t + 1] = fvertices[vj];
            vertices[t + 2] = fvertices[vk];

            int ni = normal_index[i];
            int nj = normal_index[j];
            int nk = normal_index[k];
            vertex_normals[t] = fnormals[ni];
            vertex_normals[t + 1] = fnormals[nj];
            vertex_normals[t + 2] = fnormals[nk];

            int ti = texture_index[i];
            int tj = texture_index[j];
            int tk = texture_index[k];
            textures[t] = ftextures[ti];
            textures[t + 1] = ftextures[tj];
            textures[t + 2] = ftextures[tk];
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

    free(buffer);
    free(fvertices);
    free(fnormals);
    free(ftextures);

    return mesh;
}
