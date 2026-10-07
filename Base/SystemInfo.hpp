#ifndef BASE_SYSTEM_INFO_HPP
#define BASE_SYSTEM_INFO_HPP

/// TODO: Reconsider SizeType
struct SystemInfo
{
    SizeType logicalProcessorCount;
    SizeType pageSize;
    SizeType largePageSize;
    SizeType allocationGranularity;
    Bool8 largePagesAllowed;
};

internal SystemInfo *SystemInfo_Get(void);

#endif // BASE_SYSTEM_INFO_HPP
