// Bryan Kim
// 11/30
/* Revision History:
 * 11/30: Created Class with initial logic for SubscriberPartitionStream object
 * 12/1: Added functionality with WriteStream
 */

namespace P3;

/* class invariants:
-SubscriberPartitionStream extends PartitionStream
-WriteStream(char, string) is not allowed to be used in this extension
-Instead of a collection of MsgStream objects, is a collection of SubscriberMsgStream objects
 */
public class SubscriberPartitionStream : PartitionStream
{
    private new ISubscriber[] _partStream = new ISubscriber[MaxPartitions];
    
    //pre: none
    //post: all partitions are valid to be accessed via key
    private void MapAll()
    {
        for (int i = 0; i < GetLength(); i++)
        {
            int result = base.Map();
            if (result == -1)
            {
                break;
            }
        }
    }
    
    public SubscriberPartitionStream() : base()
    {
        for (int i = 0; i < GetLength(); i++)
        {
            _partStream[i] = new SubscriberMsgStream();
        }
        MapAll();
    }

    //pre: partStream must be in the SubscriberMsgStream family of objects
    //post: all subscribers are initialized as passed in
    public SubscriberPartitionStream(ISubscriber[] partStream, int limit) : base(CastToSubMsgStream(partStream, limit),
        limit)
    {
        for (int i = 0; i < GetLength(); i++)
        {
            _partStream[i] = new SubscriberMsgStream();
        }
        MapAll();
    }

    //pre: ISubscriber stream and its limit are provided by client
    //post: return stream, but typed to SubscriberMsgStream in order to work with the base constructor in PartitionStream
    private static SubscriberMsgStream[] CastToSubMsgStream(ISubscriber[] stream, int limit)
    {
        SubscriberMsgStream[] result = new SubscriberMsgStream[limit];
        for (int i = 0; i < limit; i++)
        {
            result[i] = new SubscriberMsgStream();
            result[i].DeepCopy(stream[i]);
        }
        return result;
    }
    
    //pre: client must provide any char and a string for their message
    //post: exception is thrown
    public override void WriteStream(char key, string message)
    {
        throw new NotImplementedException("This method is only available for PartitionStream!");
    }

    //pre: client must provide a string for their message
    //post: message is added to all partitions in the stream
    public void WriteStream(string message)
    {
        if (isDisposed)
        {
            throw new ObjectDisposedException("PartitionStream is disposed.");
        }
        for (int i = 0; i < GetLength(); i++)
        {
            _partStream[i].NewMessage(message);     
        }
    }

    //pre: none
    //post: all SubscriberMsgStreams are deleted and replaced with length 0 streams
    public override void ResetAll()
    {
        if (isDisposed)
        {
            throw new ObjectDisposedException("PartitionStream is disposed.");
        }
        this._partStream = new ISubscriber[GetLength()];
        for (int i = 0; i < GetLength(); i++)
        {
            this._partStream[i] = new SubscriberMsgStream();
        }
    }
}

/*
-Even if WriteStream(char, string) is not implemented, ReadStream(char) is still available, so all partitions
must be mapped in the constructor
-If client uses WriteStream(char, string), exception is thrown
-_partStream is shadowed via new, it is retyped to hold SubscriberMsgStreams
*/