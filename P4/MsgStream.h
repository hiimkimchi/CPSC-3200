// Bryan Kim
// 10/15
/* Revision History:
 * 10/5: Created .h file
 * 10/15: Added a size field
 * 11/9: Replaced raw ptr logic with unique ptr
 */

#include <string>

#ifndef MSGSTREAM_H
#define MSGSTREAM_H

class MsgStream {
    private:
        int limit;
        int operationLimit;
        int size = 0;

        bool isEmpty();
        void Resize(const int amount);
        std::string* Read(const int leftIndex, const int rightIndex);
        void CountUp();
        void Copy(const MsgStream& other);

    protected:
        int operationCount = 0;
        std::unique_ptr<std::string[]> stream;
        int getSize();

    public:
        MsgStream();
        MsgStream(const int limit);
        MsgStream(const MsgStream& other);
        MsgStream(MsgStream&& other);
        MsgStream& operator=(const MsgStream& other);
        MsgStream& operator=(MsgStream&& other);
        virtual ~MsgStream() = default;

        //destructive
        MsgStream& operator++(int);
        MsgStream& operator<<(const std::string& message);
        MsgStream& operator+=(const std::string& message);

        //non-destructive
        bool operator==(const MsgStream& other);
        bool operator!=(const MsgStream& other);
        MsgStream operator+(const std::string& message);
        std::string operator[](const int index);

        void virtual Reset();
        void virtual Push(const std::string &message);
        std::string* ReadAll();
        std::string* ReadSection(const int leftIndex, const int rightIndex);
        std::string virtual GetType();
};

MsgStream operator+(const std::string &message, MsgStream& stream);

#endif //MSGSTREAM_H
