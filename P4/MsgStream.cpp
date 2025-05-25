// Bryan Kim
// Created by Bryan Kim on 10/6/24.
/* Revision History:
 * 10/5: created methods + constructors + asgmnt operators+ destructor according to P1
 * 10/8: created move constr + asgmnt operator
 * 10/15: modified all methods that use this->size
 * 11/9: replaced raw pointer logic with unique_ptr
 * 11/12: added overloaded operators
 */

/*class invariants:
-MsgStream size limit is non-zero and non-negative
-Strings in MsgStream cannot exceed 300 characters
-May only push 1 string at a time
-There can only be limit * 2 Push and/or Reset operations done
-Duplicates are allowed
-Addition overloading is only compatible with std::strings, not with other MsgStreams
*/

#include "MsgStream.h"
#include <string>

const int DefaultLimit = 256;
const int DefaultStreamSize = 0;
const int DefaultOperationLimit = DefaultLimit * 2;
const int MessageLimit = 300;

MsgStream::MsgStream()
{
    this->limit = DefaultLimit;
    this->stream = std::make_unique<std::string[]>(DefaultStreamSize);
    this->operationLimit = DefaultOperationLimit;
}

//pre: limit must be > 0
MsgStream::MsgStream(const int limit)
{
    if (limit <= 0)
    {
        throw std::invalid_argument("limit must be greater than 0");
    }
    this->limit = limit;
    this->stream = std::make_unique<std::string[]>(DefaultStreamSize);
    this->operationLimit = limit * 2;
}

//pre: other must be a valid MsgStream object
//post: this' fields are the same as other's
void MsgStream::Copy(const MsgStream& other)
{
    this->limit = other.limit;
    this->operationLimit = other.operationLimit;
    this->operationCount = other.operationCount;

    this->size = other.size;
    this->stream = std::make_unique<std::string[]>(this->size);
    for(int i = 0; i < this->size; i++) {
        this->stream[i] = other.stream[i];
    }
}

//pre: other is a valid MsgStream
MsgStream::MsgStream(const MsgStream& other)
{
    Copy(other);
}

//pre: other is a valid MsgStream
MsgStream::MsgStream(MsgStream&& other)
{
    this->limit = other.limit;
    this->operationLimit = other.operationLimit;
    this->stream = std::move(other.stream);
    this->operationCount = other.operationCount;
    this->size = other.size;

    other.stream = nullptr;
    other.operationCount = 0;
    other.limit = 0;
    other.operationLimit = 0;
    other.size = 0;
}

//pre: other is a valid MsgStream
//post: this' fields are copied from other
MsgStream& MsgStream::operator=(const MsgStream& other)
{
    if(this == &other)
    {
        return *this;
    }

    Copy(other);
    return *this;
}

//pre: other is a valid MsgStream
//post: this' fields will be swapped with other
MsgStream& MsgStream::operator=(MsgStream&& other)
{
    if(this == &other)
    {
        return *this;
    }

    std::swap(this->stream, other.stream);
    std::swap(this->operationLimit, other.operationLimit);
    std::swap(this->operationCount, other.operationCount);
    std::swap(this->limit, other.limit);
    std::swap(this->size, other.size);
    return *this;
}

//pre: none
//post: none
bool MsgStream::isEmpty()
{
    return this->size == 0;
}

//pre: none
//post: none
int MsgStream::getSize() {
    return this->size;
}

//pre: amount must be >= 0 and < DefaultLimit
//post: stream size is changed
void MsgStream::Resize(int const amount)
{
    if (amount < 0 || amount >= DefaultLimit)
    {
        throw std::out_of_range("MsgStream::Resize");
    }
    std::unique_ptr<std::string[]> newStream = std::make_unique<std::string[]>(amount);
    if (amount > this->size)
    {
        for (int i = 0; i < this->size; i++)
        {
            newStream[i] = this->stream[i];
        }
        this->stream = std::move(newStream);
        newStream = nullptr;
        this->size = amount;
    } else if (amount < this->size)
    {
        for (int i = 0; i < amount; i++)
        {
            newStream[i] = this->stream[i];
        }
        this->stream = std::move(newStream);
        newStream = nullptr;
        this->size = amount;
    }
}

//pre: leftIndex <= rightIndex and both indexes are in bounds of stream
//post: return array is non-empty
std::string* MsgStream::Read(const int leftIndex, const int rightIndex)
{
    if (leftIndex < 0 || rightIndex > this->size || leftIndex > rightIndex)
    {
        throw std::out_of_range("left and/or right index out of range.");
    }

    std::string* subStream = new std::string[rightIndex - leftIndex + 1];
    for (int i = leftIndex; i <= rightIndex; i++)
    {
        subStream[i - leftIndex] = this->stream[i];
    }
    return subStream;
}

//pre: operation count is < operation limit
//post: operation count is incremented by 1
void MsgStream::CountUp()
{
    operationCount++;
    if (operationCount > operationLimit)
    {
        throw std::runtime_error("Limit of operations reached.");
    }
}

//pre: stream is non-empty
//post: stream is empty
void MsgStream::Reset()
{
    this->CountUp();
    if (this->isEmpty())
    {
        throw std::out_of_range("MsgStream is empty.");
    }
    this->Resize(DefaultStreamSize);
}

//pre: message length is < MessageLimit and stream length is < limit
//post: message is appended to stream
void MsgStream::Push(const std::string &message)
{
    if (message.size() > MessageLimit)
    {
        throw std::out_of_range("Message is too big.");
    }
    if (this->limit == this->size)
    {
        throw std::out_of_range("Limit of stream reached.");
    }
    this->CountUp();
    this->Resize(this->size + 1);
    stream[this->size - 1] = message;
}

//pre: stream is non-empty
//post: return value must encompass whole stream
std::string* MsgStream::ReadAll()
{
    if(this->isEmpty())
    {
        throw std::out_of_range("MsgStream is empty.");
    }
    return this->Read(0, this->size - 1);
}

//pre: leftIndex <= rightIndex and both indexes are in bounds of stream
//post: return array is non-empty
std::string* MsgStream::ReadSection(const int leftIndex, const int rightIndex)
{
    return this->Read(leftIndex, rightIndex);
}

std::string MsgStream::GetType()
{
    return "MsgStream";
}

//pre: none
//post: pushes an empty string to the stream
MsgStream& MsgStream::operator++(int)
{
    this->Push("");
    return *this;
}

//pre: none
//post: pushes a client specified message into the stream
MsgStream& MsgStream::operator<<(const std::string& message)
{
    this->Push(message);
    return *this;
}

//pre: none
//post: adds a client specified message into the stream
MsgStream MsgStream::operator+(const std::string &message)
{
    MsgStream result(*this);
    result.Push(message);
    return result;
}

//pre: none
//post: adds a client specified message into the stream
MsgStream& MsgStream::operator+=(const std::string &message)
{
    this->Push(message);
    return *this;
}

//pre: other must be a valid MsgStream object
//post: returns true if other has same fields as this
bool MsgStream::operator==(const MsgStream &other)
{
    if (this->limit != other.limit) {
        return false;
    }

    for (int i = 0; i < this->size; i++) {
        if (this->stream[i] != other.stream[i]) {
            return false;
        }
    }
    return true;
}

//pre: other must be a valid MsgStream object
//post: returns true if other has one field different from this
bool MsgStream::operator!=(const MsgStream &other)
{
    if (this->limit != other.limit) {
        return true;
    }

    for (int i = 0; i < this->size; i++) {
        if (this->stream[i] != other.stream[i]) {
            return true;
        }
    }
    return false;
}

//pre: none
//post: returns message at index
std::string MsgStream::operator[](const int index)
{
    if (index < 0 || index >= this->size)
    {
        throw std::out_of_range("index out of range.");
    }
    return *(this->ReadSection(index, index));
}

//pre: stream is a valid MsgStream object
//post: returns stream with client decided message
MsgStream operator+(const std::string &message, MsgStream& stream)
{
    stream.Push(message);
    return stream;
}


/*implementation invariants:
-Indexes are within bounds for all read functions. If not, exception is thrown.
-this->stream initializes at size 0 and is resized for every append (or reset)
-this->operationLimit and this->opertaionCount work in tandem to make sure once
this->operationCount reaches this->operationLimit, mutator methods will throw an exception
-Addition operator: for ease of access for client to add a message to the stream. Same goes
for increment and +=.
-Equality operators: to check if streams are the same length and if the contents are the same
-Indexing operator: to easily access a specific message in the stream
-Stream I/O: for client's ease of use for Push.
*/