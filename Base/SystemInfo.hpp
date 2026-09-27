#ifndef BASE_SYSTEm_INFO_HPP
#define BASE_SYSTEm_INFO_HPP

struct SystemInfo
{
    UInt64 numberOfLogicalProcessors;
    SizeType pageSize;
    SizeType largePageSize;
    SizeType allocationGranularity;
};

internal SystemInfo *SystemInfo_Get(void);


#endif // BASE_SYSTEm_INFO_HPP
