#include "pch.h"

//
// Test settings
// 
#define TS_ENABLE_UNCOMPILABLE 0

#pragma region Structs for testing

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

    #if TS_ENABLE_UNCOMPILABLE

        //
        // Uncompilable tests.
        // 
        struct NotPod
        {
            int field1; int field2;
            NotPod() : field1( 0 ), field2( 0 ) {};
        };

    #endif // TS_ENABLE_UNCOMPILABLE

#pragma endregion

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

TEST(ToTuple, Correctness)
{
    TwoFields two_fields{ 2, 4 };

    auto two_tpl = ToTuple( two_fields );

    EXPECT_EQ( types::get<0>( two_tpl ), 2 );
    EXPECT_EQ( types::get<1>( two_tpl ), 4 );

    TenFields ten_fields{ 'a', 25, 4, 3.14, 0, 'b', 54, 32, 2.71, 9 };

    auto ten_tpl = ToTuple( ten_fields );

    EXPECT_EQ( types::get< 0>( ten_tpl ), 'a'  );
    EXPECT_EQ( types::get< 1>( ten_tpl ), 25   );
    EXPECT_EQ( types::get< 2>( ten_tpl ), 4    );
    EXPECT_EQ( types::get< 3>( ten_tpl ), 3.14 );
    EXPECT_EQ( types::get< 4>( ten_tpl ), 0    );
    EXPECT_EQ( types::get< 5>( ten_tpl ), 'b'  );
    EXPECT_EQ( types::get< 6>( ten_tpl ), 54   );
    EXPECT_EQ( types::get< 7>( ten_tpl ), 32   );
    EXPECT_EQ( types::get< 8>( ten_tpl ), 2.71 );
    EXPECT_EQ( types::get< 9>( ten_tpl ), 9    );
}

TEST(Serialization, DirectStreamCout)
{
    std::cout << "It is a visual test.\n" << std::endl;

    TenFields ten_fields{ 'a', 25, 4, 3.14, 0, 'b', 54, 32, 2.71, 9 };

    std::cout << ten_fields << std::endl;
}

TEST(Serialization, DirectStreamWcout)
{
    std::wcout << L"It is a visual test.\n" << std::endl;

    TenFields ten_fields{ 'a', 25, 4, 3.14, 0, 'b', 54, 32, 2.71, 9 };

    std::wcout << ten_fields << std::endl;
}

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

TEST(Serialization, WStringStream)
{
    TwoFields original{ 2, 4 };

    WStringStreamSerializer<TwoFields> serializer;
    WStringStreamBuffer<TwoFields> buffer;

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

#if TS_ENABLE_UNCOMPILABLE

    TEST(GetFieldsCount, Uncompilable_ExplicitTemplate)
    {
        GetFieldsCount<NotPod>();
        GetFieldsCount<int>();
    }

    TEST(GetFieldsCount, Uncompilable_ParameterPassing)
    {
        NotPod not_pod;
        int integer;

        GetFieldsCount( not_pod );
        GetFieldsCount( integer );
    }

#endif // TS_ENABLE_UNCOMPILABLE