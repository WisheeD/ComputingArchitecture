__kernel void lab2(__global const double *X, __global double* results, __global const double *cycles) {
    int id = get_global_id(0);

    results[id] =  pow(X[0], (4 * (id + 1 + 1024 * cycles[0] + 1))) / (4 * (id + 1 + 1024 * cycles[0] + 1));
}
