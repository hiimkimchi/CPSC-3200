// Bryan Kim
// 12/01
/* Revision History:
 * 12/01: Created the class along with constructors
 * 12/02: Fixed logic with DeepCopy
 */

namespace P3;

/* class invariants:
-SubscriberMsgStream extends both ISubscriber interface and the MsgStream object
 */
public class SubscriberMsgStream : MsgStream, ISubscriber
{
    public SubscriberMsgStream() : base() {}
    public SubscriberMsgStream(int limit) : base(limit) {}
    
    //pre: none
    //post: pushes message into the stream, conforming to ISubscriber interface
    public void NewMessage(string message)
    {
        base.Push(message);
    }

    //pre: client must provide a valid msgStream object
    //post: exception is thrown
    public override void DeepCopy(MsgStream msgStream)
    {
        throw new NotImplementedException("Not same object type. Deep copy will only work for same object type.");
    }

    //pre: client must provide a valid object conforming to the ISubscriber interface
    //post: this contains the same fields as subscriber
    public void DeepCopy(ISubscriber subscriber)
    {
        SubscriberMsgStream subscriberMsgStream = (SubscriberMsgStream)subscriber;
        base.DeepCopy(subscriberMsgStream);
    }
}

/* implementation invariants:
-DeepCopy is suppressed IF the input is an MsgStream object. It must extend ISubscriber interface
 */