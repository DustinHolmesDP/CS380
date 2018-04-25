#pragma once
#include <string>
#include "Stopwatch.h"
#include <sstream>

template <typename T = std::chrono::nanoseconds>
class TimeTracker
{
public:
    TimeTracker();
    void reset();
    void start();
    void stop();
    const std::wstring &get_text();
private:
    Stopwatch watch;
    std::chrono::nanoseconds accumulated;
    std::wstring text;
    const std::wstring units;

    void update_text();
};

template<typename T>
inline TimeTracker<T>::TimeTracker()
{
    static_assert("Non std::chrono::unit");
}

template <>
inline TimeTracker<std::chrono::seconds>::TimeTracker() : units(L" s")
{}

template <>
inline TimeTracker<std::chrono::milliseconds>::TimeTracker() : units(L" ms")
{}

template <>
inline TimeTracker<std::chrono::microseconds>::TimeTracker() : units(L" µs")
{}

template <>
inline TimeTracker<std::chrono::nanoseconds>::TimeTracker() : units(L" ns")
{}

template <typename T>
inline void TimeTracker<T>::reset()
{
    accumulated = accumulated.zero();
}

template <typename T>
inline void TimeTracker<T>::start()
{
    watch.start();
}

template <typename T>
inline void TimeTracker<T>::stop()
{
    watch.stop();

    accumulated += watch.nanoseconds();

    update_text();
}

template <typename T>
inline const std::wstring &TimeTracker<T>::get_text()
{
    return text;
}

template <typename T>
inline void TimeTracker<T>::update_text()
{
    static_assert("Non std::chrono::unit");
}

template <>
inline void TimeTracker<std::chrono::seconds>::update_text()
{
    text = std::to_wstring(std::chrono::duration_cast<std::chrono::seconds>(accumulated).count()) + units;
}

template <>
inline void TimeTracker<std::chrono::milliseconds>::update_text()
{
    text = std::to_wstring(std::chrono::duration_cast<std::chrono::milliseconds>(accumulated).count()) + units;
}

template <>
inline void TimeTracker<std::chrono::microseconds>::update_text()
{
    text = std::to_wstring(std::chrono::duration_cast<std::chrono::microseconds>(accumulated).count()) + units;
}

template <>
inline void TimeTracker<std::chrono::nanoseconds>::update_text()
{
    text = std::to_wstring(accumulated.count()) + units;
}