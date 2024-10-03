// See https://aka.ms/new-console-template for more information
namespace P1;
using Microsoft.VisualStudio.TestTools.UnitTesting;

[TestClass]
public class MsgStreamTest
{
    [TestMethod("Copy constructor works as intended.")]
    public void CopyConstructorWorksAsIntended()
    {
        //Arrange
        MsgStream stream1 = new MsgStream();
        stream1.Push("Hello World.");
        
        //Act
        MsgStream stream2 = new MsgStream(stream1);
        
        //Assert
        CollectionAssert.AreEqual(stream1.ReadAll(), stream2.ReadAll());
    }

    [TestMethod("When using the Push() method, it will throw an exception if string exceeds 500 characters.")]
    public void WhenUsingPushMethodThrowsExceptionIfStringIsTooLong()
    {
        //Arrange
        MsgStream stream1 = new MsgStream();
        string message = 
            "This message is way too long and I feel like I'm writing an essay. Ahhhhhh so many characters, I think I'm going crazy. " +
            "I like apples and bananas and I also think that cherries are nice in pies. I am also keen on key lime pie. " +
            "I think dogs have a peculiar Fritos smell to them when they come up next to you. My desk could use a remodel.";
        
        //Act & Assert
        Assert.ThrowsException<ArgumentOutOfRangeException>(() => stream1.Push(message));
    }

    [TestMethod(
        "When using the Push() method, it will throw an exception if there are too many strings in the stream.")]
    public void WhenUsingPushMethodThrowsExceptionIfTooManyStrings()
    {
        //Arrange
        const int limit = 5;
        const string message = "filler message";
        MsgStream stream1 = new MsgStream(limit);

        for (int i = 0; i < limit; i++)
        {
            stream1.Push(message);
        }
        
        //Act & Assert
        Assert.ThrowsException<ArgumentOutOfRangeException>(() => stream1.Push(message));
    }

    [TestMethod("ReadAll() method reads all strings in the stream.")]
    public void ReadAllStringsReadAllStringsInTheStream()
    {
        //Arrange
        MsgStream stream1 = new MsgStream();
        string[] comparisonArray = new string[10];
        
        for (int i = 0; i < 10; i++)
        {
            stream1.Push(i.ToString());
            comparisonArray[i] = i.ToString();
        }

        //Act
        string[] readAll = stream1.ReadAll();
        
        //Assert
        CollectionAssert.AreEqual(readAll, comparisonArray);
    }

    [TestMethod("ReadSection() method reads selected section of indexes in the stream.")]
    public void ReadSectionReadsSelectedSection()
    {
        //Arrange
        MsgStream stream1 = new MsgStream();
        string[] comparisonArray = new string[3];
        
        for (int i = 0; i < 10; i++)
        {
            stream1.Push(i.ToString());
            if (i >= 2 && i <= 4)
            {
                comparisonArray[i - 2] = i.ToString();
            }
        }
        
        //Act
        string[] readSome = stream1.ReadSection(2, 4);
        
        //Assert
        CollectionAssert.AreEqual(readSome, comparisonArray);
    }

    [TestMethod("Reset() method removes all strings in stream and causes an " +
                "exception when any Read method is done on it.")]
    public void ResetResultsInExceptionWhenRead()
    {
        //Arrange
        MsgStream stream1 = new MsgStream();
        
        for (int i = 0; i < 100; i++)
        {
            stream1.Push(i.ToString());
        }
        
        //Act
        stream1.Reset();
        
        //Assert
        Assert.ThrowsException<IndexOutOfRangeException>(() => stream1.ReadAll());
    }
    
    [TestMethod("When _operationLimit is exceeded, " +
                "it will throw an exception when another mutator method is called.")]
    public void WhenOperationLimitIsExceeded()
    {
        //Arrange
        MsgStream stream1 = new MsgStream(10);
        string message = "Hello World.";
        for (int i = 0; i < 10; i++)
        {
            stream1.Push(i.ToString());
        }
        
        stream1.Reset();
        
        for (int i = 0; i < 9; i++)
        {
            stream1.Push(i.ToString());
        }
        
        //Act & Assert
        Assert.ThrowsException<IndexOutOfRangeException>(() => stream1.Push(message));
    }
}