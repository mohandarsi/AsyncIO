#pragma once

#ifndef PURE
# define PURE	=0 //for pure virtual functions
#endif

#ifndef interface
# define interface	struct //c,c++ does n't have keyword for interface
#endif

#define DEFAULT_DESTRUCTOR(type) virtual ~##type##() = default;

namespace FileAPI
{

/* Offset for read/write operations*/
typedef __int64 Offset;

const Offset OFFSET_NONE = -1;
const Offset OFFSET_END = LLONG_MAX;


enum class Status {
    /** Operation completed successfully. */
    OK,
    /** Request is currently pending for processing.       */
    PENDING,
    CANCELLATION_PENDING,
    CANCELING,
    PROCESSING,
    ABORT_PENDING,
    ABORTED,
    /** Request successfully processed. */
    COMPLETED,
    /** Request was canceled. */
    CANCELED,
    CLOSED,
    OTHER_FAILURE
};

struct IOStatus
{
    Status status;
    size_t transferedBytes;
};

}