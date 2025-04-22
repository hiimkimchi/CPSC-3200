// Bryan Kim
// 12/02
/* Revision History:
 * 12/02: created boilerplate and pseudocode
 * 12/03: finished adding tests
 */
namespace P3;

public class P5
{
    static void Main(string[] args)
    {
        MsgStream[] streamArray = new MsgStream[5];
        streamArray[0] = new MsgStream();
        streamArray[1] = new DurableStream();
        streamArray[2] = new SubscriberMsgStream();
        streamArray[3] = new DurableStream("test", 3);
        streamArray[4] = new MsgStream(3);

        ISubscriber[] subscriberArray = new ISubscriber[2];
        subscriberArray[0] = new SubscriberMsgStream();
        subscriberArray[1] = new SubscriberMsgStream(5);
        
        PartitionStream partStreamHetero = new PartitionStream(streamArray, 5);
        SubscriberPartitionStream subPartStream = new SubscriberPartitionStream(subscriberArray, 2);
        TestHetero(partStreamHetero);
        TestSubscriber(subPartStream);
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
            Console.WriteLine("Should reach only if object is MsgStream or SubscriberMsgStream\n");
        }
    }

    //post: SubscriberPartitionStream does not support WriteStream(char, string)
    static void WriteException(SubscriberPartitionStream partStream, char key, string message)
    {
        try
        {
            partStream.WriteStream(key, message);
        }
        catch (Exception e)
        {
            Console.WriteLine("SubscriberPartitionStream does not support write(char, string)");
        }
    }

    static void DisposeErases(SubscriberPartitionStream partStream, string message)
    {
        partStream.Dispose();
        try
        {
            partStream.WriteStream(message);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }
    }
    
    static void TestHetero(PartitionStream partStreamHetero)
    {
        ReadWrite(partStreamHetero, 'a', "Hello World Msg!");
        ResetRead(partStreamHetero, 'a');

        ReadWrite(partStreamHetero, 'b', "Hello World Durable!");
        ResetRead(partStreamHetero, 'b');
        
        ReadWrite(partStreamHetero, 'c', "Hello World Subscriber!");
        ResetRead(partStreamHetero, 'c');
        
        ReadWrite(partStreamHetero, 'd', "Hello World Durable!");
        ResetRead(partStreamHetero, 'd');
        
        ReadWrite(partStreamHetero, 'e', "Hello World Msg!");
        ResetRead(partStreamHetero, 'e');
        
        partStreamHetero.ResetAll();
    }

    static void TestSubscriber(SubscriberPartitionStream subscriberStream)
    {
        WriteException(subscriberStream, 'a', "Hello World Subscriber!");
        DisposeErases(subscriberStream, "Hello World Subscriber!");
    }
}