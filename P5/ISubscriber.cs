// Bryan Kim
// 11/30
/* Revision History:
 * 11/30: Created Class with ISubscriber interface
 * 12/02: Added DeepCopy method
 */

namespace P3;

/* class invariants:
-ISubscriber defines the family of objects that adhere to it, which results in it being
allowed inside SubscriberPartitionStream.
 */
public interface ISubscriber
{
    void NewMessage(string message);
    void DeepCopy(ISubscriber subscriber);
}

/* implementation invariants:
-Each object that extends this interface must define their own fields 
 */
 