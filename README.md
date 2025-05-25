# Object Oriented Programming (CPSC 3200)
Taken at Seattle University with Dr. Adair Dingle

## Main Topics Covered: 
- Contractual Design (keep the client in mind as the designer)
- Polymorphism (Inheritance, Dependency Injection, Interfaces)
- Abstraction (Custom operations)
- Dynamic memory management (C++ move + copy semantics, smart pointers)
##

### **P1:** 
- _Written in C#_
- Focused on defining a "MsgStream" object based on contractual design
- Wrote test cases in MSTest to ensure object worked as intended (and also did the same thing in subsequent assignments)

### **P2:**
- _Written in C++_
- Focused on defining a "PartitionStream" object that uses dependency injection of "MsgStream" object for polymorphism
- Rewrote "MsgStream" in C++, making sure to implement deep copy and move semantics (as well as overloading assignment + copy operators) to accomodate for C++'s dynamic memory management

### **P3:**
- _Written in C#_
- Focused on defining a "DurableStream" object that uses inheritance to build on the "MsgStream" object for polymorphism
- Does not change implementation already made to "PartitionStream"

### **P4:**
- _Written in C++_
- Focused on defining custom operators for each class (except DurableStream)
- Reconsidered the extent of custom operators due to contractual design (Some operators do not make sense to implement such as multiplication. We have additive operators but it acts as concatination/appending)

### **P5:**
- _Written in C#_
- Focused on multiple inheritance between "SubscriberMsgStream" and "SubscriberPartitionStream" objects using an interface "ISubscriber"
- Added extra type checking for DeepCopy. Ex: SubscriberMsgStream is not a MsgStream, but rather both an ISubscriber and MsgStream

## Languages, Resources, and IDEs used:
<img src="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
<img src="https://img.shields.io/badge/C%23-239120?style=for-the-badge&logo=csharp&logoColor=white" />
<img src="https://img.shields.io/badge/Rider-000000?style=for-the-badge&logo=Rider&logoColor=white" />
<img src="https://img.shields.io/badge/CLion-000000?style=for-the-badge&logo=clion&logoColor=white" />
