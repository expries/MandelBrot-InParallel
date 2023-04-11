#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#pragma ide diagnostic ignored "cert-err34-c"
#include <iostream>
#include <vector>
#include <chrono>

#include "Tga.h"
#include "Mandelbrot.h"

// Execute the mandelbrot calculations in a serial way and then in parallel using
// omp and a dynamic scheduler
void compareImplementationSpeed(Mandelbrot mandelbrot, int numberOfRuns, int maxThreads) {
    mandelbrot.preCalculateMandelbrotColors();
    auto totalTime = 0;

    for (int i = 0; i < numberOfRuns; i++) {
        auto t1 = std::chrono::high_resolution_clock::now();
        auto colors = mandelbrot.serialCalculation();
        auto t2 = std::chrono::high_resolution_clock::now();

        free(colors);

        // convert execution duration to milliseconds
        auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

        // sum up duration for average
        totalTime += (int) ms_int.count();

    }

    auto averageSerial = totalTime / numberOfRuns;
    std::cout << "Serial: " << averageSerial << " ms" << std::endl;

    for (int numberOfThreads = 1; numberOfThreads <= maxThreads; numberOfThreads++) {
        totalTime = 0;

        for (int i = 0; i < numberOfRuns; i++) {
            auto t1 = std::chrono::high_resolution_clock::now();
            auto colors = mandelbrot.parallelCalculationDynamicSchedule(numberOfThreads);
            auto t2 = std::chrono::high_resolution_clock::now();

            free(colors);

            // convert execution duration to milliseconds
            auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

            // sum up duration for average
            totalTime += (int) ms_int.count();
        }

        auto averageParallel = totalTime / numberOfRuns;
        auto speedup = (float) averageSerial / (float) averageParallel;

        std::cout << "Parallel (" << numberOfThreads << " threads): " << averageParallel << " ms, speedup: " << speedup << std::endl;
    }
}


int main(int argc, char* argv[]) {
    if (argc < 10) {
        std::cout << "Usage: mandelbrot.exe <imageWidth> <imageHeight> <viewPortMinX> <viewPortMinY> <viewPortMaxX> <viewPortMaxY> <maxIterations> <maxThreads> <outputFile>" << std::endl;
        return 0;
    }

    int imageWidth = atoi( argv[1]);
    int imageHeight = atoi(argv[2]);
    float viewPortMinX = atof(argv[3]);
    float viewPortMinY = atof(argv[4]);
    float viewPortMaxX = atof(argv[5]);
    float viewPortMaxY = atof(argv[6]);
    int maxIterations = atoi(argv[7]);
    int maxThreads = atoi(argv[8]);
    char* outputPath = argv[9];

    const int numberOfRuns = 5;

    // Perform benchmark
    Mandelbrot mandelbrot(maxIterations, viewPortMinX, viewPortMaxX, viewPortMinY, viewPortMaxY, imageWidth, imageHeight);
    compareImplementationSpeed(mandelbrot, numberOfRuns, maxThreads);

    // Calculate colors
    mandelbrot.preCalculateMandelbrotColors();
    auto colors = mandelbrot.parallelCalculationDynamicSchedule(maxThreads);

    // Save as tga image
    Tga tga(imageHeight, imageWidth);
    tga.save(colors, outputPath);

    return 0;
}

#pragma clang diagnostic pop