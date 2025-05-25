// Bryan Kim
// 11/5
/* Revision History:
 * 11/11: translated from C# into C++
 */

#include <string>
#include <fstream>

#include "MsgStream.h"

#ifndef DURABLESTREAM_H
#define DURABLESTREAM_H

class DurableStream : public MsgStream {
    private:
        std::string filename;
        std::unique_ptr<std::string[]> originalContents;
        int originalContentsLength = 0;
        int appendCount = 0;

        DurableStream(const DurableStream& other);
        DurableStream& operator=(const DurableStream &other);
        std::unique_ptr<std::string[]> InitializeOriginalArray(const std::string &filename);
        void createFile(const std::string &filename);
        std::unique_ptr<std::string[]> ParseFile(std::ifstream &filename);
        bool ManageAppend();
        void WriteToFile(std::unique_ptr<std::string[]> contents, int length);
    public:
        DurableStream();
        DurableStream(const std::string &filename);
        DurableStream(const std::string &filename, const int &limit);
        DurableStream(DurableStream &&other);
        DurableStream& operator=(DurableStream &&other);
        ~DurableStream() override = default;

        void Push(const std::string &message) override;
        void Reset() override;
        std::string GetType() override;
};
#endif //DURABLESTREAM_H
