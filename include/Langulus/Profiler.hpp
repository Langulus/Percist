///                                                                           
/// Langulus::Percist                                                         
/// Copyright (c) 2025 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/Logger.hpp>

#if LANGULUS_FEATURE(PROFILING)
#include "../../source/Build.hpp"
#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>


#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_PERCIST)
   #define LANGULUS_API_PERCIST() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_PERCIST() LANGULUS_IMPORT()
#endif

/// Make the rest of the code aware, that Langulus::Percist has been included
#define LANGULUS_LIBRARY_PROFILER() 1


namespace Langulus::Percist
{

   using Clock = ::std::chrono::steady_clock;
   using TimePoint = Clock::time_point;
   using Time = Clock::duration;
   using String = ::std::string;
   using Nano = ::std::chrono::duration<long double, ::std::nano>;
   using namespace ::std::chrono_literals;

   LANGULUS(ALWAYS_INLINED)
   long double RealMs(Time t) noexcept {
      return ::std::chrono::duration_cast<Nano>(t).count() / 1'000'000.0;
   }
   

   ///                                                                        
   /// The profiler state object, keeping track of running measurements       
   ///                                                                        
   struct State {
      struct Measurement;
      struct Result;
      struct Stopper;

      using ResultPtr = ::std::unique_ptr<Result>;
      using Database = ::std::unordered_map<String, ::std::unordered_map<Build, ResultPtr>>;

   private:
      Measurement* main = nullptr;
      Database results;
      ::std::unordered_set<Build> active_builds;

      String output_file = "profiling.htm";
      Time output_interval = 1s;
      TimePoint last_output_timestamp = Clock::now();

      LANGULUS_API(PERCIST) void Compile(Measurement*);
      LANGULUS_API(PERCIST) void DumpProfilerResults() const;

   public:
      LANGULUS_API(PERCIST) void Configure(String&&, Time interval) noexcept;
      LANGULUS_API(PERCIST) auto Start(String&&, Build&&) -> Stopper;
      LANGULUS_API(PERCIST) void End();
   };


   /// Global profiler instance                                               
   LANGULUS_API(PERCIST) extern State Instance;


   ///                                                                        
   /// A single measurement                                                   
   ///                                                                        
   struct State::Measurement {
   protected:
      friend struct State;
      String       name;
      Build        build;
      bool         ended = false;
      TimePoint    start;
      TimePoint    end;
      Measurement* parent = nullptr;
      Measurement* child = nullptr;
      Result*      compiled = nullptr;

   public:
      Measurement() = delete;

      LANGULUS_API(PERCIST) Measurement(String&&, Build&&, Measurement*) noexcept;
      LANGULUS_API(PERCIST) void Stop() noexcept;
   };


   ///                                                                        
   /// Auto measurement stopper on scope end                                  
   ///                                                                        
   struct State::Stopper {
   private:
      Measurement* measurement = nullptr;

   public:
      Stopper(const Stopper&) = delete;

      LANGULUS(ALWAYS_INLINED)
      Stopper() = default;

      LANGULUS(ALWAYS_INLINED)
      Stopper(Measurement* m) noexcept
         : measurement {m} {}

      LANGULUS(ALWAYS_INLINED)
      Stopper(Stopper&& rhs) noexcept
         : measurement {rhs.measurement} {
         rhs.measurement = nullptr;
      }

      LANGULUS(ALWAYS_INLINED)
      ~Stopper() {
         if (measurement) {
            measurement->Stop();
            delete measurement;
         }
      }
   };


   ///                                                                        
   /// A compiled result                                                      
   ///                                                                        
   struct State::Result {
      String name;
      Build build;
      Time min = Time::max();
      Time max = Time::min();
      Time average = 0ms;
      mutable Time total = 0ms;
      long long samples = 0;
      Database children;

      Result() = delete;
      LANGULUS_API(PERCIST) Result(const Measurement&);
      LANGULUS_API(PERCIST) void Integrate(const Measurement&);
      LANGULUS_API(PERCIST) void Dump(::std::ofstream&, const Result* parent) const;
   };


   /// Start doing a measurement                                              
   ///   @param n - name of the measurement, usually the function name        
   ///   @param build - the build identifier (should be inline-generated)     
   ///   @return the auto-stopper                                             
   LANGULUS(ALWAYS_INLINED)
   State::Stopper Start(String&& n, Build&& build) {
      return Instance.Start(
         ::std::forward<String>(n),
         ::std::forward<Build>(build)
      );
   }
}

#undef LANGULUS_PROFILE

/// Start scoped profiling                                                    
/// Add one of these in the beginning of all functions you want to profile    
#define LANGULUS_PROFILE() \
   const auto scoped_profiler____________ = ::Langulus::Percist::Start(LANGULUS_FUNCTION(), ::Langulus::Percist::Build {})

#endif