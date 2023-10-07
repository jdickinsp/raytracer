void raycast(int i, const int *A, const int *B, int *C) {
    C[i] = A[i] + B[i];
}

__kernel void raytrace_image(__global const int *A, __global const int *B, __global int *C) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);
 
    // Do the operation
    raycast(i, A, B, C);
}
