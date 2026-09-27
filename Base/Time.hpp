#ifndef BASE_TIME_HPP
#define BASE_TIME_HPP

template<typename Rep, typename Period>
struct TimeDuration
{
    using Representation = Rep;
    Representation value;
};

using NanoSeconds  = TimeDuration<SInt64, std::nano>;
using MicroSeconds = TimeDuration<SInt64, std::micro>;
using MilliSeconds = TimeDuration<SInt64, std::milli>;
using Seconds      = TimeDuration<SInt64, std::ratio<1>>;
using Minutes      = TimeDuration<SInt64, std::ratio<60>>;
using Hours        = TimeDuration<SInt64, std::ratio<3600>>;

#endif // BASE_TIME_HPP
