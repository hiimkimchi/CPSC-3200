// Bryan Kim
// 10/27
/* Revision History:
 * 10/22: Created class
 * 10/24: Created file writing logic
 * 10/25: Revised IO logic
 * 12/02: Dispose conforms to IDisposable
 */

namespace P3;
using System.Text.RegularExpressions;

/*class invariants:
- filename is assumed to be within the working directory if it exists.
- all filenames will automatically be assigned a ".txt" extension
- this._appendCount can ONLY be within 0 and 3
- once file is Disposed, read and write into the stream is no longer possible.
*/
public class DurableStream : MsgStream, IDisposable
{
    private const int AppendLimit = 2;

    private string _filename;
    private string[] _originalContents;
    private int _appendCount = 0;

    //pre: string of filename is provided
    //post: returns a string array of each string in the file
    private string[] InitializeOriginalArray(string filename)
    {
        try
        {
            if (File.Exists(filename))
            {
                string[] lines = ParseFile(filename);
                return lines;
            }
            else
            {
                FileStream fs = File.Create(filename);
                fs.Close();
            }
        }
        catch (IOException e)
        {
            throw new IOException("Could not open file: " + filename, e);
        }
        return new string[0];
    }

    //pre: filename is a valid existing file
    //post: returns contents of the file as an array of strings
    private string[] ParseFile(string filename)
    {
        string contents = File.ReadAllText(filename);
        string[] delimiters = new string[] { "\r\n", "\n" };
        return contents.Split(delimiters, StringSplitOptions.RemoveEmptyEntries);
    }
    
    public DurableStream() : base()
    {
        this._filename = "backup.txt";
        this._originalContents = this.InitializeOriginalArray(this._filename);
        for (int i = 0; i < this._originalContents.Length; i++)
        {
            base.Push(this._originalContents[i]);
        }

        base._operationCount = 0;
    }

    //pre: filename is non-empty
    public DurableStream (string filename) : base()
    {
        this._filename = filename + ".txt";
        this._originalContents = this.InitializeOriginalArray(this._filename);
        for (int i = 0; i < this._originalContents.Length; i++)
        {
            base.Push(this._originalContents[i]);
        }
        base._operationCount = 0;
    }

    //pre: filename is non-empty and limit is > 0
    public DurableStream(string filename, int limit) : base(limit)
    {
        this._filename = filename + ".txt";
        this._originalContents = this.InitializeOriginalArray(this._filename);
        for (int i = 0; i < this._originalContents.Length; i++)
        {
            base.Push(this._originalContents[i]);
        }
        base._operationCount = 0;
    }
    
    //post: filename is set to an empty string and originalContents are removed.
    public void Dispose()
    {
        if (this._filename == "")
        {
            throw new Exception("Has already been disposed.");
        }
        this._filename = "";
        this._originalContents = null;
        GC.SuppressFinalize(this);
    }

    //post: return true if count is within 0 and AppendLimit
    //      return false if count reaches AppendLimit
    private bool ManageAppend()
    {
        if (this._appendCount < AppendLimit)
        {
            this._appendCount++;
            return true;
        }
        else
        {
            this._appendCount = 0;
            return false;
        }
    }

    //pre: contents is not empty
    //post: filename contains each element in contents in a separate line
    private void WriteToFile(string[] contents)
    {
        if (this._filename == "")
        {
            throw new IOException("File has been disposed (closed)");
        }
        else
        {
            using (StreamWriter writer = new StreamWriter(this._filename, false))
            {
                for (int i = 0; i < contents.Length; i++)
                {
                    writer.WriteLine(contents[i]);
                }
            }
        }
    }

    //pre: base is not empty (otherwise exception is thrown)
    //post: base is reset and file is reverted back to originalContents
    public override void Reset()
    {
        base.Reset();

        for (int i = 0; i < this._originalContents.Length; i++)
        {
            base.Push(this._originalContents[i]);
        }
        this.WriteToFile(this._originalContents);
    }

    //pre: message is a valid string
    //post: if file is not Disposed, message is written to stream
    public override void Push(string message)
    {
        base.Push(message);
        if (!ManageAppend())
        {
            this.WriteToFile(base.ReadAll());
        }
    }

    //pre: other is a valid DurableStream object
    public void DeepCopy(DurableStream other)
    {
        base.DeepCopy(other);
        this._filename = other._filename;
        for (int i = 0; i < other._originalContents.Length; i++)
        {
            this._originalContents[i] = other._originalContents[i];
        }
    }
}
/*implementation invariants:
- By default, DurableStream will have 1/2 of base's operationCount since mutator methods are being called
twice in vtab (once in child (extended), once in parent (original).
- All public methods extend from MsgStream (except Dispose) and assume the public methods from MsgStream
work as intended.
- _originalContents stores the contents of the file IF the file exists already.
*/