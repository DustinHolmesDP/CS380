#pragma once
#include <chrono>

// wrapper around chrono high res clock, which is wrapper around windows QPC
// just making it easier to use and read
class Stopwatch
{
public:
    void start();
    void stop();
    static size_t now();
    std::chrono::nanoseconds nanoseconds() const;
    std::chrono::microseconds microseconds() const;
    std::chrono::milliseconds milliseconds() const;
    std::chrono::seconds seconds() const;
private:
    using hrc = std::chrono::high_resolution_clock;
    hrc::time_point started;
    hrc::duration delta;
};