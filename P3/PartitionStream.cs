// Bryan Kim
// 10/27
/* Revision History:
 * 10/21: created class according to P2
 * 10/22: made changes to constructors and deepcopy
 * 10/26: non-default constructor supports heterogeneous collection
 */

namespace P3;

/* class invariants
- PartitionStream size limit is 26, and is non-zero and non-negative
- Default constructor does not initialize any keys, client must use WriteStream('>', string) to have a usable key
- PartitionStream's second constructor is meant to consider the possibility of MsgStream and its child 
DurableStream together as a heterogeneous collection
- Second constructor also initializes keys of the limit provided to the constructor
- May only push one string at a time in one MsgStream
- Operation Limit is determined by the MsgStream hierarchy
- Duplicates are allowed
*/
public class PartitionStream : IDisposable
{
    private protected const int MaxPartitions = 26;
    private protected const int LowercaseA = 97;
    
    private protected MsgStream[] _partStream = new MsgStream[MaxPartitions];
    private protected bool isDisposed = false;
    private char[] _keys = new char[MaxPartitions];
    private int _partitionLimit;

    public PartitionStream()
    {
        this._partitionLimit = MaxPartitions;
        for (int i = 0; i < this._partStream.Length; i++)
        {
            this._partStream[i] = new MsgStream();
        }
    }
    
    //pre: partStream is dependent on the client to be injected. 0 < limit < maxPartition
    //     limit must also be the same size as partStream
    //post: keys will be initialized instead of empty.
    public PartitionStream(MsgStream[] partStream, int limit)
    {
        if (limit <= 0)
        {  
            throw new ArgumentException("limit must be greater than 0", nameof(limit));
        }
        if (limit > MaxPartitions)
        {
            throw new ArgumentException("limit must be less than or equal to MaxPartitions", nameof(limit));
        }
        this._partitionLimit = limit;
        this._partStream = new MsgStream[this._partitionLimit];
        for (int i = 0; i < this._partitionLimit; i++)
        {
            if (partStream[i] is DurableStream)
            {
                this._partStream[i] = new DurableStream();                
            }
            else
            {
                this._partStream[i] = new MsgStream();
            }
            this._partStream[i].DeepCopy(partStream[i]);
        }

        this._keys = new char[this._partitionLimit];
        for (int i = 0; i < this._partitionLimit; i++)
        {
            this._keys[i] = (char)(LowercaseA + i);
        }
    }

    //pre: none
    //post: returns length of partStream
    protected int GetLength()
    {
        return this._partStream.Length;
    }
    
    //pre: the amount of elements that are = '' in this->keys is > 0
    //post: returns index of mapped keys element which corresponds to an MsgStream
    private protected int Map()
    {
        for (int i = 0; i < this._partitionLimit; i++)
        {
            if (this._keys[i] == '\0')
            {
                this._keys[i] = (char)(LowercaseA + i);
                return i;
            }
        }
        return -1;
    }
    
    //pre: key is characters a-z
    //post: returns index of the char that matches key
    private int Search(char key)
    {
        for(int i = 0; i < this._partitionLimit; i++)
        {
            if(this._keys[i] == key)
            {
                return i;
            }
        }
        return -1;
    }
    
    //pre: other is a valid PartitionStream object
    //post: this' fields are copied from other
    public void DeepCopy(PartitionStream other)
    {
        if (isDisposed)
        {
            throw new ObjectDisposedException("PartitionStream is disposed.");
        }
        this._partitionLimit = other._partitionLimit;
        this._partStream = new MsgStream[other._partStream.Length];
        for (int i = 0; i < _partStream.Length; i++)
        {
            this._partStream[i] = new MsgStream();
            this._partStream[i].DeepCopy(other._partStream[i]);
        }
        
        this._keys = new char[other._keys.Length];
        for (int i = 0; i < _keys.Length; i++)
        {
            this._keys[i] = other._keys[i];
        }
    }
    
    //pre: key exists in keys
    //post: returns all elements in MsgStream linked to key
    public string[] ReadStream(char key)
    {
        if (isDisposed)
        {
            throw new ObjectDisposedException("PartitionStream is disposed.");
        }
        int index = this.Search(key);
        if (index == -1) {
            throw new KeyNotFoundException();
        }
        return this._partStream[index].ReadAll();
    }
    
    //pre: key is characters a-z or >. message is less than 300 chars long
    //post: if key = '>', then map the next available MsgStream object and push message
    // else, search this->keys and push at given index if key exists in this->keys
    // and if the MsgStream's operationCount > operationLimit, throw exception
    public virtual void WriteStream(char key, string message)
    {
        if (isDisposed)
        {
            throw new ObjectDisposedException("PartitionStream is disposed.");
        }
        int index = -1;
        if (key == '>')
        {
            index = this.Map();
            if (index == -1) {
                throw new IndexOutOfRangeException();
            }
        } else
        {
            index = this.Search(key);
        }
        if (index == -1)
        {
            throw new KeyNotFoundException();
        }
        try
        {
            this._partStream[index].Push(message);
        }
        catch (ArgumentOutOfRangeException)
        {
            throw new IndexOutOfRangeException();
        }
    }
    
    //pre: key must exist within the keys
    //post: MsgStream linked to key's stream is deleted and replaced with length stream
    //      and if the MsgStream's operationCount > operationLimit, throw exception
    public void ResetStream(char key)
    {
        if (isDisposed)
        {
            throw new ObjectDisposedException("PartitionStream is disposed.");
        }
        int index = this.Search(key);
        if (index == -1)
        {
            throw new KeyNotFoundException();
        }
        try
        {
            this._partStream[index].Reset();
        }
        catch(ArgumentException)
        {
            throw new IndexOutOfRangeException("Key is out of range");
        }
    }
    
    //pre: none
    //post: all MsgStream's streams are deleted and replaced with length 0 stream
    public virtual void ResetAll()
    {
        this._partStream = new MsgStream[this._partitionLimit];
        for (int i = 0; i < this._partStream.Length; i++)
        {
            if (this._partStream[i] is DurableStream)
            {
                this._partStream[i] = new DurableStream();                
            }
            else
            {
                this._partStream[i] = new MsgStream();
            }
        }
    }

    //pre: PartitionStream is not already disposed
    //post: disables functionality for PartitionStream as well as set values to null
    public void Dispose()
    {
        if (isDisposed)
        {
            throw new ObjectDisposedException("PartitionStream is already disposed.");
        }
        else
        {
            for (int i = 0; i < this._partStream.Length; i++)
            {
                _partStream[i] = null;
            }
            _partStream = null;
            isDisposed = true;
        }
    }
}
/*implementation invariants:
- _keys and _partStream must have the same length at all times
- _keys and _partStream work in tandem to map a unique key (a-z) to an MsgStream via sharing a same index
- Currently, non-default constructor only accounts for MsgStream and DurableStream being a part of the 
potentially heterogeneous collection
*/