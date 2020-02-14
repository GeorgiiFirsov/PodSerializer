# PodSerializer

PodSerializer is actually two-in-one library: it contains various static reflection tools along with classes for serialization of POD's.

> Developing in progress (pointers and bit-fields are not supported now)

> Documentation in progress

## Brief introduction and review
### Serialization

Currently are fully developed two ways to serialize POD's: to binary buffer and to I/O streams. Main featores of serialization tools from this library can be included with `Serialization.h` and `StreamOperators.h` files. Here is an example:
```cpp
struct MyStruct
{
  char field1;
  int  field2;
};

// ...

MyStruct original{ 'a', 42 };
MyStruct loaded{ '\0', 0 };

BinarySerializer<MyStruct> serializer;    // Serializer class (it is a specialization of generic template)
BinaryBuffer<MyStruct> buffer;            // Buffer instance to hold serialized value

serializer.Serialize( original, buffer ); // Save our struct

assert( !buffer.IsEmpty() );              // Here buffer will never be empty

serializer.Deserialize( loaded, buffer ); // Now load data into a new variable

assert( original.field1, loaded.field1 ); // Ensure equality of fields of loaded and original structs
assert( original.field2, loaded.field2 );
```

Serialization to I/O streams differs from example above only by usage of classes `StringStreamSerializer` and `StringStreamBuffer` instead of `BinarySerializer` and `BinaryBuffer` respectively.

Moreover now you are allowed to write the following code:

```cpp
#include "StreamOperators.h"

using operators::operator <<;
using operators::operator >>;

// ...

// No operators << and >> are defined for MyStruct!!!
MyStruct obj{ 'a', 42 };

std::cout << obj; // Will print "a 42"
std::cin >> obj;  // Input each field from keyboard and put them directly into 'obj'
```

### Reflection

Another half of th library contains several reflection tools. All of them can be included with file `Reflection.h`. Using this header you can now look inside of some POD structure and, for instance, enumerate each its field (I wish I could find out names of fields... But today it is impossible in C++). Here is a couple examples:

##### Example #1
```cpp
#include "Reflection.h"

struct MyStruct
{
  int    field1;
  double field2;
  char   field3;
};

// ...

MyStruct obj{ 42, 3.14, 'a' };

std::cout << "MyStruct has "                   // Will print "MyStruct has 3 fields"
          << reflection::GetFieldsCount( obj ) 
          << " fields" << std::endl;
```

##### Example #2
```cpp
#include "Reflection.h"
#include "Tuple.h" // Here is rewritten tuple class and some tools for it

// ...

MyStruct obj{ 42, 3.14, 'a' };

// Convert our struct into a tuple
auto tpl = reflection::ToTuple( obj ); // tpl is instance of type types::Tuple<int, double, char>

assert( types::get<0>( tpl ), obj.field1 ); // Tuple contains exactly the same values as 'obj'
assert( types::get<1>( tpl ), obj.field2 );
assert( types::get<2>( tpl ), obj.field3 );

// Will be printed: "42 3.14 a"
// It is really important to use a generic lambda (with template invocation function) inside types::for_each
types::for_each( tpl, []( const auto& elem ) {
  std::cout << elem << " ";
});
```

##### Example #3
```cpp
#include "Reflection.h"
#include "Tuple.h"

// ...

MyStruct obj{ 42, 2.71, 'a' };
types::Tuple<int, double, char> tpl{ -42, 2.71, 'b' };

// Load values from tuple directly into 'obj'
obj = reflection::FromTuple<MyStruct>( tpl );

assert( obj.field1, -42  ); // 'obj' now contains exactly the same values as tuple
assert( obj.field2, 2.71 );
assert( obj.field3, 'b'  );
```

## Requirements
- C++14 support.
- Reflected (serialization uses reflection inside) structure must not contain static fields (they are simply ignored), bit-fields (they can cause some errors), unions and references. *Currently* pointers are not supported too (coming soon).
- Reflected struct must be constexpr aggregate initializable.
- Other limitations are listed in file [Support.h](https://github.com/GeorgyFirsov/PodSerializer/blob/master/PodSerializer/Support.h)

## License
[GNU General Public License v3.0](https://github.com/GeorgyFirsov/PodSerializer/blob/master/LICENSE)
