#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <map>

double iteration_time;

struct Result {
    double x;
    double S;
    double Y;
    double absDiff;
    int n;
    double iteration_time;

    Result(double x, double S, double Y, double absDiff, int n, double iteration_time) : x(x), S(S), Y(Y), absDiff(absDiff), n(n), iteration_time(iteration_time) {}
};

double calculateY(double x) {
    return 0.25 * log((1 + x) / (1 - x)) + 0.5 * atan(x);
}

double calculateS(double x, double eps, int &n) {
    double S = 0.0;
    double term = 1.0;
    n = 0;

    while (std::abs(term) >= eps) {
        term = pow(x, 4 * n + 1) / (4 * n + 1);
        S += term;
        ++n;
    }

    return S;
}

void task(std::queue<double>& tasks, double eps, std::mutex& mtx, std::condition_variable& cv, std::map<double, Result>& results, bool& done) {
    auto start_time = std::chrono::high_resolution_clock::now();

    while (true) {
        double x;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return !tasks.empty() || done; });

            if (tasks.empty() && done) {
                break;
            }

            x = tasks.front();
            tasks.pop();
        }

        int n = 0;

        double Y = calculateY(x);
        double S = calculateS(x, eps, n);

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = end_time - start_time;
        iteration_time += duration.count();

        std::lock_guard<std::mutex> lock(mtx);
        results.emplace(x, Result(x, S, Y, std::abs(Y - S), n, iteration_time));
    }

}

int main() {
    double a, b, h, eps;
    std::cout << "Enter a, b, h, eps: ";
    std::cin >> a >> b >> h >> eps;

    std::ofstream my_file("values.csv");
    std::mutex mtx;
    std::condition_variable cv;

    std::queue<double> tasks;
    for (double x = a; x <= b; x += h) {
        tasks.push(x);
    }

    bool done = false;
    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    std::map<double, Result> results;

    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(task, std::ref(tasks), eps, std::ref(mtx), std::ref(cv),
                                      std::ref(results), std::ref(done));
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
    }
    cv.notify_all();

    for (auto &t: threads) {
        t.join();
    }

    double my_time = 0;

    for (const auto& pair: results) {
        const auto& result = pair.second;

        my_time += result.iteration_time;

        my_file << result.n << ',' << my_time << "\n";

        std::cout << std::scientific << std::showpos << result.x << " | " << result.Y << " | " << result.S << " | "
        << result.absDiff << " | " << std::noshowpos << std::right << std::setw(2) << result.n << "\n";
    }

    my_file.close();
    return 0;
}
