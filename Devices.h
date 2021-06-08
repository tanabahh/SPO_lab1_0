#include <blkid/blkid.h>


typedef struct fsInfo
{
    blkid_cache blkCache;
}fsInfo;

int DeviceList(fsInfo* info);
int IteratorDevice(fsInfo*);
int Init(fsInfo*);