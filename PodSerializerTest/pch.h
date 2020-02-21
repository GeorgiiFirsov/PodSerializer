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
#include "../PodSerializer/Tuple.h"


//
// Project names
//  
using reflection::GetFieldsCount;
using reflection::GetTypeIds;
using reflection::ToTuple;
using reflection::ToStandardTuple;

using serialization::BinarySerializer;
using serialization::BinaryBuffer;

using serialization::StringStreamSerializer;
using serialization::StringStreamBuffer;

using serialization::WStringStreamSerializer;
using serialization::WStringStreamBuffer;

using types::Tuple;
using types::ToStdTuple;