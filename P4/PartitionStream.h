// Bryan Kim
// 10/15
/* Revision History:
* 10/10: created .h file
* 10/13: added public and private methods
* 11/9: set copy constructor and assignment op to delete to suppress copying and changed
* raw ptrs to smart
*/

#include "MsgStream.h"

#ifndef PARTITIONSTREAM_H
#define PARTITIONSTREAM_H

class PartitionStream {
    private:
        std::unique_ptr<std::unique_ptr<MsgStream>[]> partStream;
        std::unique_ptr<char[]> keys;
        int partitionLimit;

        int Map();
        int Search(const char key);
        void Copy(const PartitionStream &other);
        PartitionStream& operator=(const PartitionStream &other);
        PartitionStream(const PartitionStream &other);

    public:
        PartitionStream();
        PartitionStream(std::unique_ptr<std::unique_ptr<MsgStream>[]> partStream, const int);
        PartitionStream(PartitionStream &&other);
        PartitionStream& operator=(PartitionStream &&other);
        ~PartitionStream();

        //destructive
        PartitionStream& operator++(int);
        PartitionStream& operator+=(const std::string& message);

        //non-destructive
        MsgStream& operator[](int index);
        bool operator==(const PartitionStream &other);
        bool operator!=(const PartitionStream &other);
        bool operator<(const PartitionStream &other);
        bool operator<=(const PartitionStream &other);
        bool operator>(const PartitionStream &other);
        bool operator>=(const PartitionStream &other);

        std::string* ReadStream(const char &key);
        void WriteStream(const char &key, const std::string &message);
        void ResetStream(const char &key);
        void ResetAll();
};



#endif //PARTITIONSTREAM_H
