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
        TwoFieldsCorrectAnswer,
        GetFieldsCount<TwoFields>()
    );

    EXPECT_EQ(
        TenFieldsCorrectAnswer,
        GetFieldsCount<TenFields>()
    );
}

TEST(GetFieldsCount, Correctness_ParameterPassing)
{
    TwoFields two_fields;
    TenFields ten_fields;

    EXPECT_EQ(
        TwoFieldsCorrectAnswer,
        GetFieldsCount( two_fields )
    );

    EXPECT_EQ(
        TenFieldsCorrectAnswer,
        GetFieldsCount( ten_fields )
    );
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