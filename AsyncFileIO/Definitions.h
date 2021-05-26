#pragma once

namespace AsyncFileIO
{

/* Offset for read/write operations*/
typedef __int64 Offset;

const Offset CURRENT_FILE_OFFSET = -1;

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
    Status status { Status::OTHER_FAILURE };
    size_t transferredBytes { 0 };
    IOStatus() = default;

    IOStatus(const Status s, const size_t numberOfTransferredBytes)
        : status(s)
        , transferredBytes(numberOfTransferredBytes)
    {
    }
};

}