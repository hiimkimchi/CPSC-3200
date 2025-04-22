// Bryan Kim
// 10/27
/* Revision History:
 * 10/25: Created boilerplate and pseudocoded
 * 10/27: Wrote all tests covering all functionalities of objects
 */

namespace P3;

public class P3
{
    /*
    static void Main(string[] args)
    {
        MsgStream[] streamArray = new MsgStream[5];
        streamArray[0] = new MsgStream();
        streamArray[1] = new DurableStream();
        streamArray[2] = new DurableStream("test");
        streamArray[3] = new DurableStream("test", 3);
        streamArray[4] = new MsgStream(3);
        
        PartitionStream partStreamHetero = new PartitionStream(streamArray, 5);
        TestHetero(partStreamHetero);
        
        PartitionStream partStreamHomo = new PartitionStream();
        TestHomo(partStreamHomo);
        
        DurableStream durStream = new DurableStream("test");
        TestRestofDurable(durStream);
        
        MsgStream msgStream = new MsgStream();
        TestRestofMsg(msgStream);
    }
    
    //post: if partStream is a heterogenous, it should print the message 3 times
    //      if it is homogenous, it should print the string to initialize the key and then the message 2 times
    static void ReadWrite(PartitionStream partStream, char key, string message)
    {
        for (int i = 0; i < 3; i++)
        {
            partStream.WriteStream(key, message);
        }
        
        string[] writeArray = partStream.ReadStream(key);
        
        for (int i = 0; i < 3; i++)
        {
            Console.WriteLine(writeArray[i]);
        }
    }

    //post: DurableStream allows for read after reset, MsgStream does not
    static void ResetRead(PartitionStream partStream, char key)
    {
        try
        {
            partStream.ResetStream(key);
        }
        catch (Exception e)
        {
            Console.WriteLine("Should reach only if DurableStream length is 3");
        }

        try
        {
            partStream.ReadStream(key);
            Console.WriteLine("Should reach only if object is DurableStream\n");
        }
        catch (Exception e)
        {
            Console.WriteLine("Should reach only if object is MsgStream\n");
        }
    }

    //post: disposes twice, second time should throw an exception
    static void DoubleDispose(DurableStream durableStream)
    {
        durableStream.Dispose();
        try
        {
            durableStream.Dispose();
        }
        catch (Exception e)
        {
            Console.WriteLine("Dispose should only work once\n");
        }
    }

    static void ReadSectionWrite(MsgStream msgStream, string message)
    {
        for (int i = 0; i < 4; i++)
        {
            msgStream.Push(message);
        }

        for (int i = 0; i < 3; i++)
        {
            msgStream.Push("Should only read this section");
        }
        
        for (int i = 0; i < 4; i++)
        {
            msgStream.Push(message);
        }
        
        string[] array = msgStream.ReadSection(4, 6);
        for (int i = 0; i < array.Length; i++)
        {
            Console.WriteLine(array[i]);
        }
    }
    
    static void TestHetero(PartitionStream partStreamHetero)
    {
        ReadWrite(partStreamHetero, 'a', "Hello World Msg!");
        ResetRead(partStreamHetero, 'a');

        ReadWrite(partStreamHetero, 'b', "Hello World Durable!");
        ResetRead(partStreamHetero, 'b');
        
        ReadWrite(partStreamHetero, 'c', "Hello World Durable!");
        ResetRead(partStreamHetero, 'c');
        
        ReadWrite(partStreamHetero, 'd', "Hello World Durable!");
        ResetRead(partStreamHetero, 'd');
        
        ReadWrite(partStreamHetero, 'e', "Hello World Msg!");
        ResetRead(partStreamHetero, 'e');
        
        partStreamHetero.ResetAll();
    }

    static void TestHomo(PartitionStream partStreamHomo)
    {
        partStreamHomo.WriteStream('>', "This line should not be Hello World Msg!");
        ReadWrite(partStreamHomo, 'a', "Hello World Msg!");
        ResetRead(partStreamHomo, 'a');
        
        partStreamHomo.WriteStream('>', "This line should not be Hello World Msg!");
        ReadWrite(partStreamHomo, 'b', "Hello World Msg!");
        ResetRead(partStreamHomo, 'b');
        
        partStreamHomo.WriteStream('>', "This line should not be Hello World Msg!");
        ReadWrite(partStreamHomo, 'c', "Hello World Msg!");
        ResetRead(partStreamHomo, 'c');
    }

    static void TestRestofMsg(MsgStream msgStream)
    {
        ReadSectionWrite(msgStream, "Hello World Msg!");
    }

    static void TestRestofDurable(DurableStream durStream)
    {
        DoubleDispose(durStream);
    }
    */
}