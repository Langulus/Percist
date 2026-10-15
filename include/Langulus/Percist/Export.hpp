///                                                                           
/// Langulus::Percist                                                         
/// Copyright (c) 2025 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/Core.hpp>

#if not LANGULUS_FEATURE(PROFILING)
   #error This file shouldn't be included if LANGULUS_FEATURE_PROFILING is disabled
#endif

#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_PERCIST)
   #define LANGULUS_API_PERCIST() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_PERCIST() LANGULUS_IMPORT()
#endif

/// Make the rest of the code aware, that Langulus::Percist has been included 
#define LANGULUS_LIBRARY_PERCIST() 1
