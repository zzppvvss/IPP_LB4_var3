#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
#include <omp.h>
#include <windows.h>

const float percentage = 10.0;

double calculateFunction(double x) {
    return 5 * sin(0.3 * pow(x, 4)) + 0.5 * x;
}

int main(int argc, char** argv) {
    double min = 0;
    double max = 0;
    int tid;
    int numot = omp_get_max_threads();

    MEMORYSTATUSEX stat;
    stat.dwLength = sizeof(stat);
    GlobalMemoryStatusEx(&stat);
    long long size = static_cast<long long>(round((stat.ullAvailPhys / 8.0 * percentage / 100.0)));

    std::cout << round(stat.ullAvailPhys / 1000000000.0 * 9.313) / 10 << "GB available memory" << std::endl;
    std::cout << "vector of " << size << " doubles (~" << round(sizeof(double) * size / 100000000.0 * 9.313) / 100 << " GB)\n";

    for (int thrd = 1; thrd <= numot; thrd++) {
        std::vector<double> array(size);
        auto start = std::chrono::steady_clock::now();

#pragma omp parallel num_threads(thrd) private(tid)
        {
            tid = omp_get_thread_num();

#pragma omp for
            for (long long i = 0; i < array.size(); i++) {
                array[i] = -10 * (rand() % 3061) + (rand() % 1006) * (rand() % 13061) / 13.5;
                array[i] = calculateFunction(array[i]);
            }

            double localMax = array[0];
            double localMin = array[0];

#pragma omp for
            for (long long i = 0; i < array.size(); i++) {
                if (localMax < array[i]) localMax = array[i];
                if (localMin > array[i]) localMin = array[i];
            }

#pragma omp critical
            {
                if (localMax > max) max = localMax;
                if (localMin < min) min = localMin;
            }

            if (tid == 0) std::cout << "max = " << max << " min = " << min << std::endl;
        }

        auto end = std::chrono::steady_clock::now();
        std::cout << "time for [" << thrd << "] threads: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    }

    system("pause");
    return 0;
}
