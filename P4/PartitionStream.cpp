// Bryan Kim
// 10/15
/* Revision History:
 * 10/11: created constructor dependency injections
 * 10/13: created public methods for functionality, also created private methods for decomp
 *        and encapsulation
 * 10/14: refactored write and reset methods
 * 11/9: commented out copy semantics because of suppression
 * 11/12: added overloaded operators
 */

/* class invariants:
-PartitionStream maxPartitions size limit is 26 and is non-zero and non-negative,
consequently, keys can only consist of chars a-z. (NOTE: partitions are also accessible with the
indexing operator)
-PartitionStream is dependent on client to provide both a MsgStream array and the length of
that array in the non-default constructor
-MsgStreams in the partStream are constructed with the default constructor
-May only push one at a time
-Duplicate keys are not allowed. keys may also not be defined by client
 */

#include "PartitionStream.h"

const int maxPartitions = 26;
const int lowerCaseA = 97;

PartitionStream::PartitionStream()
{
    this->partitionLimit = maxPartitions;
    this->partStream = std::make_unique<std::unique_ptr<MsgStream>[]>(this->partitionLimit);
    this->keys = std::make_unique<char[]>(this->partitionLimit);
}

//pre: partStream is dependent on the client to be injected. 0 < limit < maxPartition
PartitionStream::PartitionStream(std::unique_ptr<std::unique_ptr<MsgStream>[]> partStream, const int limit)
{
    if (limit <= 0)
    {
        throw std::invalid_argument("limit must be greater than 0");
    }
    if (limit > maxPartitions)
    {
        throw std::invalid_argument("limit must be less than or equal to max (26)");
    }
    this->partitionLimit = limit;
    this->partStream = std::make_unique<std::unique_ptr<MsgStream>[]>(this->partitionLimit);

    for (int i = 0; i < this->partitionLimit; i++)
    {
        this->partStream[i] = std::move(partStream[i]);
    }
    this->keys = std::make_unique<char[]>(this->partitionLimit);
}

//pre: other must be a valid PartitionStream object
//post: this' fields are the same as other's
void PartitionStream::Copy(const PartitionStream &other)
{
    this->partitionLimit = other.partitionLimit;
    this->keys = std::make_unique<char[]>(this->partitionLimit);
    this->partStream = std::make_unique<std::unique_ptr<MsgStream>[]>(this->partitionLimit);
    for (int i = 0; i < this->partitionLimit; i++) {
        this->partStream[i] = std::move(other.partStream[i]);
        this->keys[i] = other.keys[i];
    }
}

//pre: other must be a valid PartitionStream object
PartitionStream::PartitionStream(const PartitionStream &other)
{
    this->Copy(other);
}

//pre: other is a valid PartitionStream object
PartitionStream::PartitionStream(PartitionStream &&other)
{
    this->partitionLimit = other.partitionLimit;
    this->partStream = std::move(other.partStream);
    this->keys = std::move(other.keys);

    other.partStream = nullptr;
    other.keys = nullptr;
    other.partitionLimit = 0;
}

//pre: other is a valid PartitionStream
//post: this' fields are copied from other
PartitionStream& PartitionStream::operator=(const PartitionStream &other)
{
    if(this == &other)
    {
     return *this;
    }

    Copy(other);
    return *this;
}

//pre: other is a valid PartitionStream
//post: this' fields will be swapped with other
PartitionStream& PartitionStream::operator=(PartitionStream &&other)
{
    if(this == &other) {
        return *this;
    }

    std::swap(this->partitionLimit, other.partitionLimit);
    std::swap(this->partStream, other.partStream);
    std::swap(this->keys, other.keys);
    return *this;
}

PartitionStream::~PartitionStream()
{
    this->partitionLimit = 0;
}

//pre: the amount of elements that are = '' in this->keys is > 0
//post: returns index of mapped keys element which corresponds to an MsgStream object
int PartitionStream::Map()
{
    for (int i = 0; i < this->partitionLimit; i++)
    {
        if (this->keys[i] == '\0')
        {
            this->keys[i] = lowerCaseA + i;
            return i;
        }
    }
    return -1;
}

//pre: key is characters a-z
//post: returns index of the char that matches key
int PartitionStream::Search(const char key)
{
    for(int i = 0; i < this->partitionLimit; i++)
    {
        if(this->keys[i] == key)
        {
            return i;
        }
    }
    return -1;
}

//pre: key exists in keys
//post: returns all elements in MsgStream linked to key
std::string* PartitionStream::ReadStream(const char &key)
{
    int index = this->Search(key);
    if (index == -1) {
        throw std::invalid_argument("Key not found");
    }
    return this->partStream[index]->ReadAll();
}

//pre: key is characters a-z or >. message is less than 300 chars long
//post: if key = '>', then map the next available MsgStream object and push message
//      else, search this->keys and push at given index if key exists in this->keys
//      and if the MsgStream's operationCount > operationLimit, throw exception
void PartitionStream::WriteStream(const char &key, const std::string &message)
{
    int index = -1;
    if (key == '>')
    {
        index = this->Map();
        if (index == -1) {
            throw std::invalid_argument("Stream is full");
        }
    } else
    {
        index = this->Search(key);
    }

    if (index == -1)
    {
        throw std::invalid_argument("Invalid key");
    }

    try
    {
        this->partStream[index]->Push(message);
    }
    catch(std::runtime_error &e)
    {
        throw std::invalid_argument(e.what());
    }
}

//pre: key must exist within the keys
//post: MsgStream linked to key's stream is deleted and replaced with length 0 stream
//      and if the MsgStream's operationCount > operationLimit, throw exception
void PartitionStream::ResetStream(const char &key)
{
    int index = this->Search(key);
    if (index == -1)
    {
        throw std::invalid_argument("Invalid key");
    }

    try
    {
        this->partStream[index]->Reset();
    }
    catch(std::runtime_error &e)
    {
        throw std::invalid_argument(e.what());
    }
}

//pre: none
//post: all MsgStream's streams are deleted and replaced with length 0 stream
void PartitionStream::ResetAll()
{
    this->partStream = std::make_unique<std::unique_ptr<MsgStream>[]>(this->partitionLimit);
}

//pre: none
//post: PartitionStream is granted access to another pre-constructed MsgStream object IF not full
PartitionStream& PartitionStream::operator++(int)
{
    int index = this->Map();
    if (index == -1) {
        throw std::invalid_argument("Stream is full");
    }
    return *this;
}

//pre: none
//post: PartitionStream is granted access to another pre-constructed MsgStream object IF not full
PartitionStream& PartitionStream::operator+=(const std::string& message)
{
    this->WriteStream('>', message);
    return *this;
}

//pre: none
//post: returns the MsgStream object in the index specified
MsgStream& PartitionStream::operator[](int index)
{
    if (index < 0 || index >= this->partitionLimit) {
        throw std::invalid_argument("Invalid index");
    }
    try {
        this->ReadStream(static_cast<char>(index + lowerCaseA));
    } catch(std::invalid_argument &e) {
        throw std::invalid_argument(e.what());
    }
    return *(this->partStream[index]);
}

//pre: none
//post: returns true if all fields are the same between this and other
bool PartitionStream::operator==(const PartitionStream &other)
{
    if (this->partitionLimit != other.partitionLimit)
    {
        return false;
    }

    for (int i = 0; i < this->partitionLimit; i++)
    {
        if(this->keys[i] != other.keys[i] || this->partStream[i] != other.partStream[i]) {
            return true;
        }
    }
    return false;
}

//pre: none
//post: returns true if at least one field is different between this and other
bool PartitionStream::operator!=(const PartitionStream &other)
{
    if (this->partitionLimit != other.partitionLimit)
    {
        return true;
    }

    for (int i = 0; i < this->partitionLimit; i++)
    {
        if(this->keys[i] != other.keys[i] || this->partStream[i] != other.partStream[i]) {
            return false;
        }
    }
    return true;
}

//pre: none
//post: returns true if the limit of the partstream is < the other
bool PartitionStream::operator<(const PartitionStream &other)
{
    if (this->partitionLimit < other.partitionLimit) {
        return true;
    }
    return false;
}

//pre: none
//post: returns true if the limit of the partstream is <= the other
bool PartitionStream::operator<=(const PartitionStream &other)
{
    if (this->partitionLimit <= other.partitionLimit) {
        return true;
    }
    return false;
}

//pre: none
//post: returns true if the limit of the partstream is > the other
bool PartitionStream::operator>(const PartitionStream &other)
{
    if (this->partitionLimit > other.partitionLimit) {
        return true;
    }
    return false;
}

//pre: none
//post: returns true if the limit of the partstream is >= the other
bool PartitionStream::operator>=(const PartitionStream &other) {
    if (this->partitionLimit >= other.partitionLimit) {
        return true;
    }
    return false;
}

/*implementation invariants:
-All elements in partStream follows all implementation invariants from MsgStream
-this->partStream is initialized with a fixed size at this->partitionLimit
-this->keys is same size as this->partStream (also fixed size)
-copying is suppressed for the object
-Addition operators: are only implemented for increment and += as my implementation only allows
for appending
-Equality operators: for client's ease for comparing, compared all fields of this to other's
-Inequality operators: for client's ease of comparing, only comparing the limit of partitionStream
-Indexing operators: for client's ease of access. gives clients the option to access each
MsgStream with index or associated key
*/