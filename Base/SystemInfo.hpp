#ifndef BASE_SYSTEM_INFO_HPP
#define BASE_SYSTEM_INFO_HPP

struct SystemInfo
{
    UInt64 logicalProcessorCount;
    SizeType pageSize;
    SizeType largePageSize;
    SizeType allocationGranularity;
    Bool8 largePagesAllowed;
};

internal SystemInfo *SystemInfo_Get(void);

#endif // BASE_SYSTEM_INFO_HPP
