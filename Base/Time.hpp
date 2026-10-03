#ifndef BASE_TIME_HPP
#define BASE_TIME_HPP

template<typename Representation, typename Period>
struct TimeDuration
{
    using RepresentationType = Representation;
    using PeriodType = Period;
    RepresentationType value;
};

using NanoSeconds  = TimeDuration<SInt64, std::nano>;
using MicroSeconds = TimeDuration<SInt64, std::micro>;
using MilliSeconds = TimeDuration<SInt64, std::milli>;
using Seconds      = TimeDuration<SInt64, std::ratio<1>>;
using Minutes      = TimeDuration<SInt64, std::ratio<60>>;
using Hours        = TimeDuration<SInt64, std::ratio<3600>>;

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

    static TimePointType Now();
};

#endif // BASE_TIME_HPP
