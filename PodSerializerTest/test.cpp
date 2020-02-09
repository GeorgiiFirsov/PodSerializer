#include "pch.h"

// 
// Two fields in structure
// 
struct TwoFields
{
    int field1;
    int field2;
};
#define TwoFieldsCorrectAnswer 2

//
// Ten fields in structure
// 
struct TenFields
{
    char field1; int field2; int field3; double field4; short field5;
    char field6; int field7; int field8; double field9; short field10;
};
#define TenFieldsCorrectAnswer 10

//
// Struct with enum inside
// 
enum TestEnum1 : unsigned long long
{
    first1, second1
};

enum TestEnum2
{
    first2, second2
};

struct ThreeFieldsWithEnum
{
    char      field1;
    TestEnum1 field2;
    TestEnum2 field3; 
};
#define ThreeFieldsWithEnumCorrectAnswer 3


/************************************************************************************
 * Reflection tests
 */

TEST(GetFieldsCount, Correctness_ExplicitTemplate) 
{
    EXPECT_EQ(
        GetFieldsCount<TwoFields>(),
        TwoFieldsCorrectAnswer
    );

    EXPECT_EQ(
        GetFieldsCount<TenFields>(),
        TenFieldsCorrectAnswer
    );
}

TEST(GetFieldsCount, Correctness_ParameterPassing)
{
    TwoFields two_fields;
    TenFields ten_fields;

    EXPECT_EQ(
        GetFieldsCount( two_fields ),
        TwoFieldsCorrectAnswer
    );

    EXPECT_EQ(
        GetFieldsCount( ten_fields ),
        TenFieldsCorrectAnswer
    );
}

TEST(GetFieldsCount, ContainsEnum)
{
    ThreeFieldsWithEnum three_fields;

    EXPECT_EQ(
        GetFieldsCount( three_fields ),
        ThreeFieldsWithEnumCorrectAnswer
    );
}

TEST(ToTuple, Correctness)
{
    TwoFields two_fields{ 2, 4 };

    auto two_tpl = ToTuple( two_fields );

    EXPECT_EQ( types::get<0>( two_tpl ), 2 );
    EXPECT_EQ( types::get<1>( two_tpl ), 4 );

    TenFields ten_fields{ 'a', 25, 4, 3.14, 0, 'b', 54, 32, 2.71, 9 };

    auto ten_tpl = ToTuple( ten_fields );

    EXPECT_EQ( types::get<0>( ten_tpl ), 'a'  );
    EXPECT_EQ( types::get<1>( ten_tpl ), 25   );
    EXPECT_EQ( types::get<2>( ten_tpl ), 4    );
    EXPECT_EQ( types::get<3>( ten_tpl ), 3.14 );
    EXPECT_EQ( types::get<4>( ten_tpl ), 0    );
    EXPECT_EQ( types::get<5>( ten_tpl ), 'b'  );
    EXPECT_EQ( types::get<6>( ten_tpl ), 54   );
    EXPECT_EQ( types::get<7>( ten_tpl ), 32   );
    EXPECT_EQ( types::get<8>( ten_tpl ), 2.71 );
    EXPECT_EQ( types::get<9>( ten_tpl ), 9    );
}

TEST(ToTuple, ContainsEnum)
{
    ThreeFieldsWithEnum three_fields = { 'a', first1, second2 };

    auto three_tpl = ToTuple( three_fields );

    EXPECT_EQ( types::get<0>( three_tpl ), 'a'     );
    EXPECT_EQ( types::get<1>( three_tpl ), first1  );
    EXPECT_EQ( types::get<2>( three_tpl ), second2 );
}


/************************************************************************************
 * Visual stream operators tests
 */

TEST(Operators, ostream)
{
    using operators::operator <<;
    
    std::cout << "It is a visual test.\n" << std::endl;

    TenFields ten_fields{ 'a', 25, 4, 3.14, 0, 'b', 54, 32, 2.71, 9 };

    std::cout << ten_fields << std::endl;
}

TEST(Operators, wostream)
{
    using operators::operator <<;
    
    std::wcout << L"It is a visual test.\n" << std::endl;

    TenFields ten_fields{ 'a', 25, 4, 3.14, 0, 'b', 54, 32, 2.71, 9 };

    std::wcout << ten_fields << std::endl;
}


/************************************************************************************
 * Serialization tests
 */

TEST(Serialization, Binary)
{
    TwoFields original{ 2, 4 };

    BinarySerializer<TwoFields> serializer;
    BinaryBuffer<TwoFields> buffer;

    EXPECT_TRUE( buffer.IsEmpty() );

    serializer.Serialize( original, buffer );

    EXPECT_FALSE( buffer.IsEmpty() );

    TwoFields loaded{ 0, 0 };

    EXPECT_NE( loaded.field1, original.field1 );
    EXPECT_NE( loaded.field2, original.field2 );

    serializer.Deserialize( loaded, buffer );

    EXPECT_EQ( loaded.field1, original.field1 );
    EXPECT_EQ( loaded.field2, original.field2 );
}

TEST(Serialization, BinaryContainsEnum)
{
    ThreeFieldsWithEnum original{ 'a', first1, second2 };

    BinarySerializer<ThreeFieldsWithEnum> serializer;
    BinaryBuffer<ThreeFieldsWithEnum> buffer;

    EXPECT_TRUE( buffer.IsEmpty() );

    serializer.Serialize( original, buffer );

    EXPECT_FALSE( buffer.IsEmpty() );

    ThreeFieldsWithEnum loaded{ 'b', second1, first2 };

    EXPECT_NE( loaded.field1, original.field1 );
    EXPECT_NE( loaded.field2, original.field2 );
    EXPECT_NE( loaded.field3, original.field3 );

    serializer.Deserialize( loaded, buffer );

    EXPECT_EQ( loaded.field1, original.field1 );
    EXPECT_EQ( loaded.field2, original.field2 );
    EXPECT_EQ( loaded.field3, original.field3 );
}

TEST(Serialization, StringStream)
{
    TwoFields original{ 2, 4 };

    StringStreamSerializer<TwoFields> serializer;
    StringStreamBuffer<TwoFields> buffer;

    EXPECT_TRUE( buffer.IsEmpty() );

    serializer.Serialize( original, buffer );

    EXPECT_FALSE( buffer.IsEmpty() );

    TwoFields loaded{ 0, 0 };

    EXPECT_NE( loaded.field1, original.field1 );
    EXPECT_NE( loaded.field2, original.field2 );

    serializer.Deserialize( loaded, buffer );

    EXPECT_EQ( loaded.field1, original.field1 );
    EXPECT_EQ( loaded.field2, original.field2 );
}

TEST(Serialization, StringStreamContainsEnum)
{
    ThreeFieldsWithEnum original{ 'a', first1, second2 };

    StringStreamSerializer<ThreeFieldsWithEnum> serializer;
    StringStreamBuffer<ThreeFieldsWithEnum> buffer;

    EXPECT_TRUE( buffer.IsEmpty() );

    serializer.Serialize( original, buffer );

    EXPECT_FALSE( buffer.IsEmpty() );

    ThreeFieldsWithEnum loaded{ 'b', second1, first2 };

    EXPECT_NE( loaded.field1, original.field1 );
    EXPECT_NE( loaded.field2, original.field2 );
    EXPECT_NE( loaded.field3, original.field3 );

    serializer.Deserialize( loaded, buffer );

    EXPECT_EQ( loaded.field1, original.field1 );
    EXPECT_EQ( loaded.field2, original.field2 );
    EXPECT_EQ( loaded.field3, original.field3 );
}