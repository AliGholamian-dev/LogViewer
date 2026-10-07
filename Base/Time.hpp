#ifndef BASE_TIME_HPP
#define BASE_TIME_HPP

/// TODO: Add a template called Period which is a std::ratio for conversion
template<typename Representation>
struct TimeDuration
{
    using RepresentationType = Representation;
    // using PeriodType = Period;
    RepresentationType value;
};

using NanoSeconds  = TimeDuration<SInt64>;
using MicroSeconds = TimeDuration<SInt64>;
using MilliSeconds = TimeDuration<SInt64>;
using Seconds      = TimeDuration<SInt64>;
using Minutes      = TimeDuration<SInt64>;
using Hours        = TimeDuration<SInt64>;

template<typename Clock, typename Duration = typename Clock::DurationType>
struct TimePoint
{
    using ClockType = Clock;
    using DurationType = Duration;

    DurationType since;
};

struct TimestampClock
{
    using DurationType = MicroSeconds;
    using TimePointType = TimePoint<TimestampClock>;
};

template<typename Clock>
Clock::TimePointType GetNowTime();

template<>
TimestampClock::TimePointType GetNowTime<TimestampClock>();

#endif // BASE_TIME_HPP
