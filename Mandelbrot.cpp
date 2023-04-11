//
// Created by Samuel on 15/03/2023.
//

#include "Mandelbrot.h"

Mandelbrot::Mandelbrot(int maxIterations,
                       float viewportMinX,
                       float viewPortMaxX,
                       float viewPortMinY,
                       float viewPortMaxY,
                       int imageWidth,
                       int imageHeight) {

    this->maxIterations = maxIterations;
    this->viewportMinX = viewportMinX;
    this->viewPortMaxX = viewPortMaxX;
    this->viewPortMinY = viewPortMinY;
    this->viewPortMaxY = viewPortMaxY;
    this->imageWidth = imageWidth;
    this->imageHeight = imageHeight;
}

// Calculates pixels for mandelbrot visualization in a serial manner
RgbColor * Mandelbrot::serialCalculation() {
    auto colors = allocateColorsArray();

    // calculate pixel colors
    for (int x = 0; x < imageWidth; x++) {
        for (int y = 0; y < imageHeight; y++) {
            auto index = y * imageWidth + x;
            colors[index] = calculatePixel(x, y);
        }
    }

    return colors;
}

// Parallelize first loop using a static scheduler
RgbColor* Mandelbrot::parallelCalculationStaticSchedule(int numberOfThreads) {
    auto colors = allocateColorsArray();

    #pragma omp parallel for schedule(static) num_threads(numberOfThreads)
    for (int x = 0; x < imageWidth; x++) {
        for (int y = 0; y < imageHeight; y++) {
            auto index = y * imageWidth + x;
            colors[index] = calculatePixel(x, y);
        }
    }

    return colors;
}

// Parallelize the nested loops using a static scheduler
RgbColor* Mandelbrot::parallelCalculationStaticScheduleCollapsed(int numberOfThreads) {
    auto colors = allocateColorsArray();

    // calculate pixel colors in parallel
    #pragma omp parallel for collapse(2) num_threads(numberOfThreads)
    for (int x = 0; x < imageWidth; x++) {
        for (int y = 0; y < imageHeight; y++) {
            auto index = y * imageWidth + x;
            colors[index] = calculatePixel(x, y);
        }
    }

    return colors;
}

// Parallelize first loop using a dynamic scheduler
RgbColor* Mandelbrot::parallelCalculationDynamicSchedule(int numberOfThreads) {
    auto colors = allocateColorsArray();

    // calculate pixel colors in parallel
    #pragma omp parallel for schedule(dynamic) num_threads(numberOfThreads)
    for (int x = 0; x < imageWidth; x++) {
        for (int y = 0; y < imageHeight; y++) {
            auto index = y * imageWidth + x;
            colors[index] = calculatePixel(x, y);
        }
    }

    return colors;
}

// Parallelize the nested loops using a dynamic scheduler
RgbColor * Mandelbrot::parallelCalculationDynamicScheduleCollapsed(int numberOfThreads) {
    auto colors = allocateColorsArray();

    // calculate pixel colors in parallel
    #pragma omp parallel for collapse(2) schedule(dynamic) num_threads(numberOfThreads)
    for (int x = 0; x < imageWidth; x++) {
        for (int y = 0; y < imageHeight; y++) {
            auto index = y * imageWidth + x;
            colors[index] = calculatePixel(x, y);
        }
    }

    return colors;
}

// Partition pixel coordinate for calculation
// Parallelize using omp parallel across each of these partitions
RgbColor * Mandelbrot::parallelCalculationPartitioned(int numberOfThreads) {
    // Partition coordinates
    auto partitions = PartitionCoordinates(numberOfThreads);
    auto colors = allocateColorsArray();

    // Execute calculations for partitions in parallel
    #pragma omp parallel for num_threads(numberOfThreads)
    for (auto [from, to] : partitions) {
        for (int x = from; x < to; x++) {
            for (int y = 0; y < imageHeight; y++) {
                auto index = x * imageHeight + y;
                // threads write to colors array concurrently
                colors[index] = calculatePixel(x, y);
            }
        }
    }

    return colors;
}

// Partition pixel coordinate for calculation
// Parallelize using omp parallel across each of these partitions
// But in contrast to parallelCalculation2 the threads do write to a shared colors array during the main iteration
RgbColor * Mandelbrot::parallelCalculationPartitioned2(int numberOfThreads) {
    auto partitions = PartitionCoordinates(numberOfThreads);
    auto colors = allocateColorsArray();

    #pragma omp parallel for
    for (auto [from, to] : partitions) {
        // use private array to avoid changes to shared variables during main iteration
        auto size = (to - from) * imageHeight;
        auto privateColors = (RgbColor*) malloc(sizeof(RgbColor) * size);

        // calculate pixel colors (main iteration)
        for (int x = from; x < to; x++) {
            for (int y = 0; y < imageHeight; y++) {
                auto index = (x - from) * imageHeight + y;
                // threads write to colors array concurrently
                privateColors[index] = calculatePixel(x, y);
            }
        }

        // copy calculated colors from private array into shared color array
        auto offset = from * sizeof(RgbColor);
        memcpy(colors + offset, privateColors, sizeof(RgbColor) * size);
        free(privateColors);
    }

    return colors;
}

RgbColor* Mandelbrot::allocateColorsArray() {
    return (RgbColor*) malloc(sizeof(RgbColor) * imageWidth * imageHeight);
}

// Returns the color of a pixel
RgbColor Mandelbrot::calculatePixel(int px, int py) {
    auto cx = normalizeToViewport(px, viewportMinX, viewPortMaxX, imageWidth);
    auto cy = normalizeToViewport(py, viewPortMinY, viewPortMaxY, imageHeight);
    float zx = cx;
    float zy = cy;

    for (int n = 0; n < maxIterations; n++) {
        float x = (zx * zx - zy * zy) + cx;
        float y = (zy * zx + zx * zy) + cy;

        float magnitudeSquared = x * x + y * y;

        if (magnitudeSquared > 4) {
            return getMandelbrotColor(n);
        }

        zx = x;
        zy = y;
    }

    return {0, 0, 0};
}

// Normalizes a value of coordinates to the given viewport
float Mandelbrot::normalizeToViewport(int value, float viewPortMin, float viewPortMax, int maxImage) {
    float ratio = (float) (value) / (float) (maxImage);
    float normalizeToRange = ratio * (viewPortMax - viewPortMin) + viewPortMin;
    return normalizeToRange;
}

// Returns the color of a pixel based on the given iteration
RgbColor Mandelbrot::getMandelbrotColor(int n) {
    return this->mandelbrotColors[n];
}

// Pre-calculate all possible mandelbrot colors
void Mandelbrot::preCalculateMandelbrotColors() {
    mandelbrotColors.reserve(this->maxIterations);

    for (int i = 0; i < this->maxIterations; i++) {
        double t = (double) i / (double) this->maxIterations;
        auto red = (uint8_t) (9 * (1 - t) * std::pow(t, 3) * 255);
        auto green = (uint8_t) (15 * std::pow((1 - t), 2) * std::pow(t, 2) * 255);
        auto blue = (uint8_t) (8.5 * std::pow((1 - t), 3) * t * 255);

        mandelbrotColors.push_back({red, green, blue });
    }
}

// This function generates a partitioning of the x-coordinates of an image to be processed in  parallel by
// multiple threads. The image width is divided into several equal-sized ranges, with the number of ranges
// per thread determined by the subscriptionRate variable. The size of each range is calculated based on
// the number of threads (THREAD_NUM) and the subscription rate.
// Returns a vector of tuples representing the start and end coordinates of each range.
std::vector<std::tuple<int, int>> Mandelbrot::PartitionCoordinates(int numberOfThreads) {
    // How many ranges per core?
    auto subscriptionRate = 3;
    // Calculate range size
    auto rangeSize = imageWidth / (numberOfThreads * subscriptionRate);
    auto ranges = std::vector<std::tuple<int,int>>();

    // create set of x-coordinate ranges
    for (int i = 0; i < imageWidth; i += rangeSize) {
        auto to = i + rangeSize > imageWidth ? imageWidth : i + rangeSize;
        auto tuple = std::make_tuple(i, to);
        ranges.push_back(tuple);
    }

    return ranges;
}
