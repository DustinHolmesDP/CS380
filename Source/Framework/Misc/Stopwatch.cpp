#include <pch.h>
#include "Stopwatch.h"

void Stopwatch::start()
{
    started = hrc::now();
}

void Stopwatch::stop()
{
    const auto stopped = hrc::now();

    delta = stopped - started;
}

size_t Stopwatch::now()
{
    return hrc::now().time_since_epoch().count();
}

std::chrono::nanoseconds Stopwatch::nanoseconds() const
{
    return delta;
}

std::chrono::microseconds Stopwatch::microseconds() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(delta);
}

std::chrono::milliseconds Stopwatch::milliseconds() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(delta);
}

std::chrono::seconds Stopwatch::seconds() const
{
    return std::chrono::duration_cast<std::chrono::seconds>(delta);
}
