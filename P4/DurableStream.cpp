// Bryan Kim
// 11/15
/* Revision History:
 * 10/22: Created class
 * 10/24: Created file writing logic
 * 10/25: Revised IO logic
 * 11/12: Revised IO logic to work in C++ syntax
 */

#include "DurableStream.h"
#include <fstream>


/*class invariants:
- filename is assumed to be within the working directory if it exists.
- all filenames will automatically be assigned a ".txt" extension
- copy assignment operator or constructor are not permitted
*/

const int AppendLimit = 2;

//pre: string of filename is provided
//post: returns a string array of each string in the file. also initializes originalcontentslength
std::unique_ptr<std::string[]> DurableStream::InitializeOriginalArray(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        this->createFile(filename);
        return nullptr;
    } else {
        std::unique_ptr<std::string[]> lines = ParseFile(file);
        file.close();
        return lines;
    }
}

//pre: file does not already exist
//post: a new file is created with filename
void DurableStream::createFile(const std::string &filename)
{
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    file.close();
}

//pre: filename is a valid existing file
//post: returns contents of the file as a unique pointer to an array of strings
//      also sets the originalContentsLength
std::unique_ptr<std::string[]> DurableStream::ParseFile(std::ifstream &filename)
{
    int size = 0;
    std::string* lines = nullptr;
    std::string* temp = nullptr;
    std::string line;
    while (std::getline(filename, line)) {
        temp = lines;
        size += 1;
        lines = new std::string[size];
        for (int i = 0; i < size - 1; i++) {
            lines[i] = temp[i];
        }
        lines[size - 1] = line;
    }

    this->originalContentsLength = size;
    std::unique_ptr<std::string[]> unique_lines = std::make_unique<std::string[]>(size);
    for (int i = 0; i < size; i++) {
        unique_lines[i] = lines[i];
    }
    return unique_lines;
}

DurableStream::DurableStream() : MsgStream()
{
    this->filename = "backup.txt";
    this->originalContents = this->InitializeOriginalArray(this->filename);
    for (int i = 0; i < this->originalContentsLength; i++)
    {
        MsgStream::Push(this->originalContents[i]);
    }

    MsgStream::operationCount = 0;
}

//pre: filename is non-empty
DurableStream::DurableStream (const std::string &filename) : MsgStream()
{
    this->filename = filename + ".txt";
    this->originalContents = this->InitializeOriginalArray(this->filename);
    for (int i = 0; i < this->originalContentsLength; i++)
    {
        MsgStream::Push(this->originalContents[i]);
    }
    MsgStream::operationCount = 0;
}

//pre: other is a valid DurableStream object
DurableStream::DurableStream (const DurableStream& other)
{
    this->filename = other.filename;
    this->originalContentsLength = other.originalContentsLength;
    this->originalContents = this->InitializeOriginalArray(this->filename);
    for (int i = 0; i < this->originalContentsLength; i++) {
        MsgStream::Push(other.originalContents[i]);
    }
    this->appendCount = other.appendCount;
}

//pre: other is a valid DurableStream
DurableStream::DurableStream (DurableStream&& other)
{
    this->filename = other.filename;
    this->originalContentsLength = other.originalContentsLength;
    this->originalContents = std::move(other.originalContents);
    this->appendCount = other.appendCount;

    other.originalContentsLength = 0;
    other.appendCount = 0;
    other.filename = nullptr;
    other.originalContents = nullptr;
}

//pre: other is a valid DurableStream
DurableStream& DurableStream::operator=(DurableStream&& other)
{
    if (this == &other) {
        return *this;
    }

    std::swap(this->filename, other.filename);
    std::swap(this->originalContentsLength, other.originalContentsLength);
    std::swap(this->originalContents, other.originalContents);
    std::swap(this->appendCount, other.appendCount);
    return *this;
}

//pre: filename is non-empty and limit is > 0
DurableStream::DurableStream(const std::string &filename, const int &limit) : MsgStream(limit)
{
    this->filename = filename + ".txt";
    this->originalContents = this->InitializeOriginalArray(this->filename);
    for (int i = 0; i < this->originalContentsLength; i++)
    {
        MsgStream::Push(this->originalContents[i]);
    }
    MsgStream::operationCount = 0;
}

//post: return true if count is within 0 and AppendLimit
//      return false if count reaches AppendLimit
bool DurableStream::ManageAppend()
{
    if (this->appendCount < AppendLimit)
    {
        this->appendCount++;
        return true;
    }
    else
    {
        this->appendCount = 0;
        return false;
    }
}

//pre: contents is not empty
//post: filename contains each element in contents in a separate line
void DurableStream::WriteToFile(std::unique_ptr<std::string[]> contents, int length)
{
    std::ofstream file(this->filename);
    for (int i = 0; i < length; i++) {
        file << contents[i] << '\n';
    }
}

//pre: base is not empty (otherwise exception is thrown)
//post: base is reset and file is reverted back to originalContents
void DurableStream::Reset()
{
    MsgStream::Reset();

    for (int i = 0; i < this->originalContentsLength; i++)
    {
        MsgStream::Push(this->originalContents[i]);
    }

    std::unique_ptr <std::string[]> originalContentsCopy = std::make_unique<std::string[]>(this->originalContentsLength);
    for (int i = 0; i < this->originalContentsLength; i++) {
        originalContentsCopy[i] = this->originalContents[i];
    }
    this->WriteToFile(std::move(originalContentsCopy), this->originalContentsLength);
}

//pre: message is a valid string
//post: if file is not Disposed, message is written to stream
void DurableStream::Push(const std::string &message)
{
    MsgStream::Push(message);
    if (!ManageAppend())
    {
        std::unique_ptr<std::string[]> allContentsCopy = std::make_unique<std::string[]>(getSize());
        for (int i = 0; i < getSize(); i++) {
            allContentsCopy[i] = MsgStream::stream[i];
        }
        this->WriteToFile(std::move(allContentsCopy), MsgStream::getSize());
    }
}

std::string DurableStream::GetType() {
    return "DurableStream";
}

/*implementation invariants:
- By default, DurableStream will have 1/2 of base's operationCount since mutator methods are being called
twice in vtab (once in child (extended), once in parent (original).
- All public methods extend from MsgStream (except Dispose) and assume the public methods from MsgStream
work as intended.
- originalContents stores the contents of the file IF the file exists already.
- copying is suppressed due to the risk of two DurableStreams writing to the same file
- this.appendCount can ONLY be within 0 and 3
*/
