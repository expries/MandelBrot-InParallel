//
// Created by Samuel on 15/03/2023.
//

#ifndef MANDELBROT_MANDELBROT_H
#define MANDELBROT_MANDELBROT_H


#include <vector>
#include "RgbColor.h"

class Mandelbrot {
private:
    int maxIterations;
    float viewportMinX;
    float viewPortMaxX;
    float viewPortMinY;
    float viewPortMaxY;
    int imageWidth;
    int imageHeight;

    // Holds all possible mandelbrot colors
    std::vector<RgbColor> mandelbrotColors;

    // Returns the color of a pixel
    RgbColor calculatePixel(int px, int py);

    // Allocates array for colors
    RgbColor* allocateColorsArray();

    // Normalizes a value of coordinates to the given viewport
    static float normalizeToViewport(int value, float viewPortMin, float viewPortMax, int maxImage);

    // Returns the color of a pixel based on the given iteration
    RgbColor getMandelbrotColor(int n);

    // This function generates a partitioning of the x-coordinates of an image to be processed in  parallel by
    // multiple threads. The image width is divided into several equal-sized ranges, with the number of ranges
    // per thread determined by the subscriptionRate variable. The size of each range is calculated based on
    // the number of threads (THREAD_NUM) and the subscription rate.
    // Returns a vector of tuples representing the start and end coordinates of each range.
    std::vector<std::tuple<int, int>> PartitionCoordinates(int numberOfThreads);

public:
    Mandelbrot(int maxIterations,
               float viewportMinX,
               float viewPortMaxX,
               float viewPortMinY,
               float viewPortMaxY,
               int imageWidth,
               int imageHeight);

    // Pre-calculate all possible mandelbrot colors
    void preCalculateMandelbrotColors();

    // Calculates pixels for mandelbrot visualization in a serial manner
    RgbColor * serialCalculation();

    // Parallelize first loop using a static scheduler
    RgbColor* parallelCalculationStaticSchedule(int numberOfThreads);

    // Parallelize the nested loops using a static scheduler
    RgbColor* parallelCalculationStaticScheduleCollapsed(int numberOfThreads);

    // Parallelize first loop using a dynamic scheduler
    RgbColor* parallelCalculationDynamicSchedule(int numberOfThreads);

    // Parallelize the nested loops using a dynamic scheduler
    RgbColor * parallelCalculationDynamicScheduleCollapsed(int numberOfThreads);

    // Partition pixel coordinate for calculation
    // Parallelize using omp parallel across each of these partitions
    RgbColor * parallelCalculationPartitioned(int numberOfThreads);

    // Partition pixel coordinate for calculation
    // Parallelize using omp parallel across each of these partitions
    // But in contrast to parallelMandelbrot2 the threads do write to a shared colors array during the main iteration
    RgbColor * parallelCalculationPartitioned2(int numberOfThreads);
};


#endif //MANDELBROT_MANDELBROT_H
