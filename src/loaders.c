#include <loaders.h>

int buffer_offset(char *buffer, char match) {
    char *next = buffer;
    next = strchr(next, match);
    return next - buffer;
}

int buffer_skip_line(char *buffer, int *offset) {
    int c = buffer[*offset];
    while (c != '\n' && c != '\0') {
        c = buffer[(*offset)++];
    }
    return c;
}

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
    char *buffer = malloc(buffer_size);
    int bytes_read = fread_s(buffer, buffer_size, 1, buffer_size, fp);
    fclose(fp);
    if (bytes_read != buffer_size) {
        fprintf(stderr, "Failed to copy file to buffer");
        abort();
    }
    // printf("buffer: %s\n", buffer);

    Mesh *mesh = malloc(sizeof(Mesh));

    int c = 0;
    int idx_v = 0;
    int idx_n = 0;
    int idx_t = 0;
    int idx_f = 0;
    int idx_fv = 0;
    float fx, fy, fz;
    int fi, fj, fk;
    char sub_buffer[32];
    ObjFaceIndexType face_index_type = OBJ_FACE_INDEX_TYPE_2;

    // first pass: count total vertex and faces
    c = buffer[offset];
    while (offset <= buffer_size) {
        if (c == '#') {  // ignore comment
            c = buffer_skip_line(buffer, &offset);
        } else if (c == 'u') {  // ignore usemtl
            c = buffer_skip_line(buffer, &offset);
        } else if (c == 'v') {
            char peek = buffer[offset++];
            if (c == 'v' && peek == ' ') {  // vertices
                idx_v++;
            } else if (c == 'v' && peek == 'n') {  // normals;
                idx_n++;
            } else if (c == 'v' && peek == 't') {  // texture coordinates
                idx_t++;
            }
            c = buffer_skip_line(buffer, &offset);
        } else if (c == 'f') {  // face indexes
            if (idx_fv == 0) {
                memcpy(&sub_buffer, &buffer[offset], 32);
                sscanf_s(sub_buffer, "%i//%i//%i", &fi, &fj, &fk);
                // determine face index type, 1//2//3, or 1/2/3 or 1 2 3
                char *forward_dash = strchr(sub_buffer, '/');
                if (forward_dash != NULL) {
                    int look = forward_dash - sub_buffer + 1;
                    char peek = buffer[offset + look];
                    face_index_type = peek == '/' ? OBJ_FACE_INDEX_TYPE_2 : OBJ_FACE_INDEX_TYPE_1;
                } else {
                    face_index_type = OBJ_FACE_INDEX_TYPE_3;
                }
            }
            while (c != '\n') {
                char peek = buffer[offset + 1];
                if (peek == '\n' || peek == '\0') {
                    break;
                }
                if (c == ' ') {
                    idx_f++;
                }
                c = buffer[offset++];
            }
            idx_fv++;
            c = buffer_skip_line(buffer, &offset);
        } else {
            c = buffer_skip_line(buffer, &offset);
        }
        c = buffer[offset++];
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
    c = buffer[offset];
    // second pass: collect object data
    while (offset <= buffer_size) {
        if (c == '#') {  // ignore comment
            c = buffer_skip_line(buffer, &offset);
        } else if (c == 'u') {  // ignore usemtl
            c = buffer_skip_line(buffer, &offset);
        } else if (c == 'v') {
            char peek = buffer[offset++];
            if (c == 'v' && peek == ' ') {  // vertices
                char *next = &buffer[offset] - 1;
                fx = strtof(++next, &next);
                fy = strtof(++next, &next);
                fz = strtof(++next, &next);
                fvertices[idx_v] = (Vec3){fx, fy, fz};
                offset += next - &buffer[offset] - 1;
                idx_v++;
            } else if (c == 'v' && peek == 'n') {  // normals
                char *next = &buffer[offset] + 1;
                fx = strtof(++next, &next);
                fy = strtof(++next, &next);
                fz = strtof(++next, &next);
                fnormals[idx_n] = (Vec3){fx, fy, fz};
                offset += next - &buffer[offset] - 1;
                idx_n++;
            } else if (c == 'v' && peek == 't') {  // texture coordinates
                char *next = &buffer[offset] + 1;
                fx = strtof(++next, &next);
                fy = strtof(++next, &next);
                ftextures[idx_t] = (Vec2){fx, fy};
                offset += next - &buffer[offset] - 1;
                idx_t++;
            }
        } else if (c == 'f') {  // face indexes
            int fi, fj, fk;
            int face_vertex_count = 0;
            while (c != '\n') {
                char peek = buffer[offset + 1];
                if (peek == '\n' || peek == '\0') {
                    break;
                }
                if (c == ' ') {
                    if (face_index_type == OBJ_FACE_INDEX_TYPE_1) {
                        char *next = &buffer[offset];
                        fi = strtol(next, &next, 10);
                        fj = strtol(++next, &next, 10);
                        fk = strtol(++next, &next, 10);
                    } else if (face_index_type == OBJ_FACE_INDEX_TYPE_2) {
                        memcpy(&sub_buffer, &buffer[offset], 32);
                        sscanf_s(sub_buffer, "%i//%i//%i", &fi, &fj, &fk);
                    } else {
                        memcpy(&sub_buffer, &buffer[offset], 32);
                        sscanf_s(sub_buffer, "%i %i %i", &fi, &fj, &fk);
                    }
                    // printf("f(%i,%i,%i)\n", fi, fj, fk);
                    vertex_index[idx_f] = fi - 1;
                    texture_index[idx_f] = fj - 1;
                    normal_index[idx_f] = fk - 1;
                    idx_f++;
                    face_vertex_count++;
                }
                c = buffer[offset++];
            }
            face_index[idx_fv] = face_vertex_count;
            idx_fv++;
            c = buffer_skip_line(buffer, &offset);
        } else {
            c = buffer_skip_line(buffer, &offset);
        }
        c = buffer[offset++];
    }

    const int face_count = face_index[0] == 1 ? idx_fv / 3 : idx_fv;
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
