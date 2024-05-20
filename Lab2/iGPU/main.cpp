#include <iostream>
#include <cmath>
#include <fstream>
#include <chrono>
#include <iomanip>
#define CL_TARGET_OPENCL_VERSION 300
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

std::ofstream my_file("values.csv");
double iteration_time = 0.0f;

double Y(const double x) {
    return (0.25 * std::log((1 + x) / (1 - x)) + 0.5 * std::atan(x));
}

void S(const double x, double eps) {
    double sum = 0;
    int i = 0;
    int k = 0;
    const double yVal = Y(x);
    auto *X = static_cast<double *>(malloc(sizeof(double) * 1));
    const auto cycles = static_cast<int *>(malloc(sizeof(int) * 1));
    X[0] = x;
    cycles[0]=0;
    char *source_str;
    size_t source_size;
    FILE *fp = fopen("lab2.cl", "r");
    source_str = static_cast<char*>(malloc(MAX_SOURCE_SIZE));
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    cl_platform_id platform_id = nullptr;
    cl_device_id device_id = nullptr;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1,
            &device_id, &ret_num_devices);

    const cl_context context = clCreateContext( nullptr, 1, &device_id, nullptr, nullptr, &ret);

    const cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    cl_mem X_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
             1 * sizeof(double), nullptr, &ret);
    cl_mem cycles_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
            1 * sizeof(int), nullptr, &ret);
    cl_mem results_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
            2048 * sizeof(double), nullptr, &ret);

    ret = clEnqueueWriteBuffer(command_queue, X_mem_obj, CL_TRUE, 0,
            1 * sizeof(double), X, 0, nullptr, nullptr);
    ret = clEnqueueWriteBuffer(command_queue, cycles_mem_obj, CL_TRUE, 0,
           1 * sizeof(int), cycles, 0, nullptr, nullptr);

    const cl_program program = clCreateProgramWithSource(context, 1,
                                                         const_cast<const char **>(&source_str), &source_size, &ret);

    ret = clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);

    const cl_kernel kernel = clCreateKernel(program, "lab2", &ret);

    auto *results = new double[2048];

    const auto start = std::chrono::high_resolution_clock::now();

    while(true){
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&X_mem_obj);
        ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&results_mem_obj);
        ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&cycles_mem_obj);
        ret = clEnqueueWriteBuffer(command_queue, cycles_mem_obj, CL_TRUE, 0,
            1 * sizeof(int), cycles, 0, nullptr, nullptr);

        size_t global_item_size = 2048;
        size_t local_item_size = 64;
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr,
            &global_item_size, &local_item_size, 0, nullptr, nullptr);

        ret = clEnqueueReadBuffer(command_queue, results_mem_obj, CL_TRUE, 0,
            2048 * sizeof(double), results, 0, nullptr, nullptr);

        while (true) {
            sum += results[i];
            ++i;
            ++k;
            if (std::abs(results[i]) <= eps)
                break;
        }
        break;
    }

    const auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_iteration = end - start;
    iteration_time += elapsed_iteration.count();

    if (yVal != sum)
        sum = yVal;

    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(results_mem_obj);
    ret = clReleaseMemObject(cycles_mem_obj);
    ret = clReleaseMemObject(X_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(X);
    free(cycles);
    delete[] results;

    std::cout << std::scientific << std::showpos << " | " << x << " | " << yVal << " | " << sum
    << " | " << std::abs(std::ceil(yVal) - std::ceil(sum)) << " | " << std::noshowpos
    << std::right << std::setw(2) << k + 3 << " | " << "\n";

    my_file << k + 3 << "," << iteration_time << "\n";
}

int main()
{
    double a, b, h, eps;
    std::cout << "Enter a, b, h, eps: ";
    std::cin >> a >> b >> h >> eps;

    for (double x = a; std::abs(x - b) >= eps; x += h) {
        S(x, eps);
    }

    return 0;
}