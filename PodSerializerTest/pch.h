//
// pch.h
// Header for standard system include files.
//

#pragma once

//
// Google Tests headers
// 
#include "gtest/gtest.h"


//
// Project headers
// 
#include "../PodSerializer/Reflection.h"
#include "../PodSerializer/StreamOperators.h"
#include "../PodSerializer/Serialization.h"
#include "../PodSerializer/TypeList.h"
#include "../PodSerializer/Tuple.h"
#include "../PodSerializer/GetTypeList.h"


//
// Project names
//  

// ../PodSerializer/Reflection.h
using reflection::GetFieldsCount;
using reflection::GetTypeIds;
using reflection::ToTuple;
using reflection::ToStandardTuple;
using reflection::GetTypeList;
using reflection::ToTuplePrecise;
using reflection::ToStandardTuplePrecise;

// ../PodSerializer/Serialization.h
using serialization::BinarySerializer;
using serialization::BinaryBuffer;
using serialization::StringStreamSerializer;
using serialization::StringStreamBuffer;
using serialization::WStringStreamSerializer;
using serialization::WStringStreamBuffer;

// ../PodSerializer/TypeList.h
using type_list::TypeList;
using type_list::EmptyTypeList;
using type_list::Size;
using type_list::TupleType;
using type_list::Apply;

// ../PodSerializer/Tuple.h
using types::Tuple;
using types::ToStdTuple;

//../PodSerializer/GetTypeList.h
using reflection::GetTypeList;