// Bryan Kim
// 11/15
/* Revision History
 * 11/13: started driver code and tested overloaded objects' functionality
 * 11/15: wrote test cases for operators
 */

#include "DurableStream.h"
#include "MsgStream.h"
#include "PartitionStream.h"
#include <string>
#include <memory>
#include <iostream>

//pre: stream1 is a valid MsgStream object
//post: only ++ should result in empty string. first two strings are dummy data to test
void test_msg_additive_and_stream_operators(MsgStream& stream1, std::string message) {
    MsgStream stream2 = stream1;
    stream2 = stream2 + message;
    stream2 = message + stream2;
    stream2++;
    stream2 += message;
    stream2 << message;
    for (int i = 0; i < 7; i++) {
        std::cout << "Printing: " + stream2[i] << std::endl;
    }
}

//pre: stream1 and stream2 are valid MsgStream objects
void test_msg_equality_operators(MsgStream& stream1, MsgStream& stream2) {
    if (stream1 == stream2) {
        std::cout << "Streams are equal" << std::endl;
    } else if (stream1 != stream2) {
        std::cout << "Streams are not equal" << std::endl;
    }
}

//pre: stream1 is a valid MsgStream object
void test_msg_indexing_operators(MsgStream& stream1, int index, int maxIndex) {
    std::cout << stream1[index] << std::endl;
    try {
        std::cout << stream1[maxIndex + 1] << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Index exceeded" << std::endl;
    }
}

//pre: stream is an object in the MsgStream hierarchy
//post: return 0 if MsgStream, 1 if DurableStream
int MsgStream_hierarchy(MsgStream& stream) {
    if (stream.GetType() == "MsgStream") {
        return 0;
    }
    if (stream.GetType() == "DurableStream") {
        return 1;
    }
}

//pre: stream1 is a valid PartitionStream object
//post: prints first index of each MsgStream in the PartitionStream.
void test_part_additive_operators(PartitionStream& stream1, int size, std::string message)
{
    stream1 += message;
    stream1++;

    try {
        for (int i = 0; i < size; i++) {
            std::cout << "Printing: " + stream1[i][0] << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

}

//pre: stream1 and stream2 are valid PartitionStream objects
void test_part_equality_operators(PartitionStream& stream1, PartitionStream& stream2)
{
    if (stream1 == stream2) {
        std::cout << "Streams are equal" << std::endl;
    } else if (stream1 != stream2) {
        std::cout << "Streams are not equal" << std::endl;
    }
}

//pre: stream1 and stream2 are valid PartitionStream objects
void test_part_inequality_operators(PartitionStream& stream1, PartitionStream& stream2)
{
    if (stream1 > stream2) {
        std::cout << "Stream1 > stream2" << std::endl;
    } else if (stream1 < stream2) {
        std::cout << "Stream1 < stream2" << std::endl;
    } else {
        std::cout << "Stream1 == stream2, therefore, Stream1 <= Stream2 and Stream1 >= Stream2" << std::endl;
    }
}

//pre: stream is a valid PartitionStream object, consisting of MsgStream and DurableStream objects
void test_part_hetero_collection (PartitionStream& stream, int size, std::string message) {
    for (int i = 0; i < size; i++) {
        try {
            for (int j = 0; j < 4; j++) {
                stream[i].Push(message);
            }
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}

int main()
{
    std::unique_ptr<std::unique_ptr<MsgStream>[]> msgStreamList = std::make_unique<std::unique_ptr<MsgStream>[]>(3);
    std::unique_ptr<std::unique_ptr<MsgStream>[]> mixedStreamList = std::make_unique<std::unique_ptr<MsgStream>[]>(4);
    std::unique_ptr<std::unique_ptr<MsgStream>[]> mixedStreamList2 = std::make_unique<std::unique_ptr<MsgStream>[]>(4);

    for (int i = 0; i < 3; i++) {
        msgStreamList[i] = std::make_unique<MsgStream>();
    }

    for (int i = 0; i < 4; i++) {
        if (i < 2) {
            mixedStreamList[i] = std::make_unique<DurableStream>();
            mixedStreamList2[i] = std::make_unique<DurableStream>();
        } else {
            mixedStreamList[i] = std::make_unique<MsgStream>();
            mixedStreamList2[i] = std::make_unique<DurableStream>();
        }
    }

    std::unique_ptr<PartitionStream[]> partStreamList = std::make_unique<PartitionStream[]>(3);

    msgStreamList[0]->Push("Hello");
    msgStreamList[0]->Push("World");
    msgStreamList[2]->Push("Hello");
    msgStreamList[2]->Push("World");

    test_msg_additive_and_stream_operators(*msgStreamList[0], "Hello World!");
    test_msg_equality_operators(*msgStreamList[0], *msgStreamList[1]);
    test_msg_equality_operators(*msgStreamList[0], *msgStreamList[2]);
    test_msg_indexing_operators(*msgStreamList[0], 0, 6);

    partStreamList[0] = PartitionStream(std::move(msgStreamList), 3);
    partStreamList[1] = PartitionStream(std::move(mixedStreamList), 4);
    partStreamList[2] = PartitionStream(std::move(mixedStreamList2), 4);

    test_part_equality_operators(partStreamList[0], partStreamList[1]);
    test_part_equality_operators(partStreamList[1], partStreamList[2]);
    test_part_inequality_operators(partStreamList[0], partStreamList[1]);
    test_part_inequality_operators(partStreamList[1], partStreamList[0]);
    test_part_inequality_operators(partStreamList[1], partStreamList[2]);

    test_part_additive_operators(partStreamList[0], 3, "Hello World!");
    test_part_hetero_collection(partStreamList[1], 4, "Hello World!");
    return 0;
}