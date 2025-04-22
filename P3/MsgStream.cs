// Bryan Kim
// 10/03
/* Revision History:
   9/30 Created constructors and Push mechanism
   10/2 Created Read and Reset mechanisms
   10/3 Added documentation of invariants
   10/22 Created a DeepCopy method instead of copy constructor
*/

namespace P3;

/*class invariants:
- MsgStream size limit is non-zero and non-negative
- Strings in MsgStream cannot exceed 300 characters
- May only push 1 string at a time
- There can only be limit * 2 Push and/or Reset operations done
- Duplicates are allowed
*/
public class MsgStream
{
    private const int DefaultLimit = 256;
    private const int DefaultStreamSize = 0;
    private const int DefaultOperationLimit = DefaultLimit * 2;
    private const int MessageLimit = 300;
    
    private int _limit;
    private string[] _stream = new string[DefaultStreamSize];
    private int _operationLimit;
    protected int _operationCount = DefaultStreamSize;
    
    public MsgStream ()
    {
        _limit = DefaultLimit; 
        _operationLimit = DefaultOperationLimit;
    }
    
    //precond: limit must be > 0
    public MsgStream (int limit)
    {
        if (limit <= 0)
        {
            throw new ArgumentOutOfRangeException(nameof(limit), "Limit must be greater than zero");
        }
        _limit = limit;
        _operationLimit = limit * 2;
    }

    //precond: msgStream must be a valid initialized MsgStream object
    public virtual void DeepCopy(MsgStream msgStream)
    {
        if (msgStream == null)
        {
            throw new ArgumentNullException(nameof(msgStream));
        }
        
        this._limit = msgStream._limit;
        this._stream = new string[msgStream._stream.Length];
        for (int i = 0; i < _stream.Length; i++)
        {
            this._stream[i] = msgStream._stream[i];
        } 
        this. _operationLimit = msgStream._operationLimit;
    }
    
    //precond: none
    //postcond: none
    private bool IsEmpty()
    {
        return _stream.Length == 0;
    }
    
    //precond: amount must be >= 0 and < DefaultLimit
    //postcond: stream size is changed
    private protected void Resize(int amount)
    {
        if (amount < 0 || amount >= DefaultLimit)
        {
            throw new ArgumentOutOfRangeException(nameof(amount), "Amount must be greater than 0 and less than DefaultLimit.");
        }
        
        string[] newStream = new string[amount];
        if (amount > _stream.Length)
        {
            for (int i = 0; i < _stream.Length; i++)
            {
                newStream[i] = _stream[i];
            }
            _stream = newStream;
        } else if (amount < _stream.Length)
        {
            for (int i = 0; i < amount; i++)
            {
                newStream[i] = _stream[i];
            }
            _stream = newStream;
        }
        else
        {
            throw new ArgumentOutOfRangeException(nameof(amount), "Stream size has not changed.");
        }
    }
    
    //precond: leftIndex <= rightIndex and both indexes are in bounds of stream
    //postcond: return array is non-empty
    private string[] Read(int leftIndex, int rightIndex)
    {
        if (leftIndex < 0 || rightIndex > _stream.Length - 1 || leftIndex > rightIndex)
        {
            throw new IndexOutOfRangeException("Index Out of Range.");
        }
        else
        {
            string[] subStream = new string[rightIndex - leftIndex + 1];
            for (int i = leftIndex; i <= rightIndex; i++)
            {
                subStream[i - leftIndex] = _stream[i];
            }
            return subStream;
        }
    }

    //precond: operationCount is < operationLimit
    //postcond: operationCount++ if < operationLimit. Otherwise, throw exception
    private protected void CountUp()
    {
        _operationCount += 1;
        if (_operationCount > _operationLimit)
        {
            throw new IndexOutOfRangeException("Maximum number of operations reached.");
        }
    }
    
    //precond: stream is non-empty
    //postcond: stream is empty
    public virtual void Reset()
    {
        this.CountUp();
        if (this.IsEmpty())
        {
            throw new ArgumentException("MsgStream is already empty.");
        }
        this.Resize(DefaultStreamSize);
    }

    //precond: message length is < MessageLimit and stream length is < limit
    //postcond: message is appended to stream
    public virtual void Push(string message)
    {
        if (message.Length > MessageLimit)
        {
            throw new ArgumentOutOfRangeException(nameof(message), "Message is too large.");
        }

        if (_limit == _stream.Length)
        {
            throw new ArgumentOutOfRangeException(nameof(message), "MsgStream length exceeded.");
        }
        else
        {
            this.CountUp();
            this.Resize(_stream.Length + 1);
            _stream[_stream.Length - 1] = message;
        }
    }

    //precond: stream is non-empty
    //postcond: return value must encompass whole stream
    public string[] ReadAll()
    {
        if (this.IsEmpty())
        {
            throw new IndexOutOfRangeException("MsgStream is empty.");
        }
        return this.Read(0, _stream.Length - 1);
    }

    //precond: leftIndex <= rightIndex and both indexes are in bounds of stream
    //postcond: return array is non-empty
    public string[] ReadSection(int leftIndex, int rightIndex)
    {
        return this.Read(leftIndex, rightIndex);
    }
}

/*implementation invariants: 
- Indexes are within bounds for all read functions. If not, exception is thrown.
- _stream initializes at size 0 and is resized for every append (or reset)
- _operationLimit and _count work in tandem to make sure once _count reaches _operationLimit, 
 mutator methods will throw an exception
*/