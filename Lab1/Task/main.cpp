#include <iostream>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <fstream>

void Task(float a, float b, float h, float eps);

int main()
{
    float a, b, h, eps;

    std::cout << "Enter a, b, h, eps:" << "\n";

    std:: cin >> a >> b >> h >> eps;

    Task(a, b, h, eps);

    return 0;
}

void Task(float a, float b, float h, float eps)
{
    std::ofstream my_file("values.csv");
    float four = 4.0f; float one = 1.0f; float half = 0.5f; float part = 0.25f;
    float res = 0.0f; float tempY = 0.0f; float tempS = 0.0f;
    float S = 0.0f; float Y = 0.0f;
    float k = 0.0f; float xk = 0.0f;

    float accumulated_time = 0.0f;
    float iteration_time = 0.0f;

    std::cout << std::setprecision(3) << std::scientific << std::showpos << "eps = "
    << eps << "\n";

    std::cout << " |     x      |     S(x)   |     Y(x)   | abs(Y - S) |  n |"
    << std::noshowpos << std::right << std::setw(2) << "\n";

    auto start_program=std::chrono::high_resolution_clock::now();

    for (float x = a; x <= b; x += h)
    {
        auto start_iteration=std::chrono::high_resolution_clock::now();

        Y = 0.0f;

        // (1 + x) / (1 - x)
        asm("fld %1;" "fld %2;" "fsub;" "fld %1;" "fld %2;" "fadd;" "fdiv;" "fstp %0;": "=m" (res): "m" (x), "m" (one));

        // 0.25 * ln((1 + x) / (1 - x))
        asm("fld1;" "fld %0;" "fyl2x;" "fldln2;" "fmul;" "fld %1;" "fmul;" "fstp %0;": "+m" (res): "m" (part));

        // 0.5 * arctg(x)
        asm("fld %1;" "fld %2;" "fpatan;" "fld %3;" "fmul;" "fstp %0;": "=m" (tempY): "m" (x), "m" (one), "m" (half));

        // 0.25 * ln((1 + x) / (1 - x)) + 0.5 * arctg(x)
        asm("fld %1;" "fld %2;" "fadd;" "fstp %0;": "=m" (Y): "m" (tempY), "m" (res));

        S = 0.0f;

        while(std::abs(Y - S) > eps)
        {
            auto start_cycle=std::chrono::high_resolution_clock::now();

            xk = 1.0f;

            for(int j = 0; j < 4 * k + 1; ++j)       // x ^ 4 * k + 1
            {
                asm("fld %0;" "fld %1;" "fmul;" "fstp %0;" : "+m" (xk) : "m" (x));
            }

            // 4 * k + 1
            asm("fld %1;" "fld %3;" "fmul;" "fld %2;" "fadd;" "fstp %0;": "=m" (tempS): "m" (four), "m" (one), "m" (k));

            // sum(x ^ (4 * k + 1) / (4 * k + 1), k = 1..n)
            asm("fld %1;" "fld %2;" "fdiv;" "fld %0;" "fadd;" "fstp %0;" : "+m" (S): "m" (xk), "m" (tempS));

            k += 1.0f;

            auto end_cycle=std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_iterations = end_cycle - start_cycle;
            //std::cout << "Time for iteration: " << elapsed_iterations.count() << " seconds" << "\n";

            iteration_time += elapsed_iterations.count();

            if (k > 4.0f)
                break;
        }

        int n = k;

        std::cout << std::setprecision(3) << std::scientific << std::showpos << " | "
        << x << " | " << S << " | " << Y << " | " << std::abs(Y - S) << " | " << std::noshowpos
        << std::right << std::setw(2) << n << " | " << "\n";

        auto end_program = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed_program = end_program - start_program;

        accumulated_time += elapsed_program.count();
        my_file << iteration_time << "," << n << "\n";
    }

    my_file.close();
    return;
}