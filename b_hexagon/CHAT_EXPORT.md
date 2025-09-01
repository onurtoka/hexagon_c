# Chat Export Summary

## 1. Project Goal
C++17 hexagonal (ports & adapters) application that ingests raw track data, computes delay metrics, and emits enriched results while adhering to SOLID principles and clear domain separation.

## 2. Architectural Overview
- Hexagonal layers:
  - Domain: models (TrackData, DelayCalculatedTrackData), logic (TrackCalculations), ports (IDataReceiver, IDataSender)
  - Application: CalculatorService orchestrating receive → compute → send loop
  - Adapters (infrastructure/UI): Console (manual), ZeroMQ (PUB/SUB)
- Dependency Inversion: Domain depends only on abstractions (ports), adapters implement them.
- DTO Split: Input vs output to avoid accidental mutation and clarify lifecycle.

## 3. Domain Models & Units (Final)
TrackData:
- int trackId
- double velocityX, velocityY, positionX, positionY
- long updateTime (nanoseconds)
- long originalUpdateTime (milliseconds)
- long firstHopSentTime (nanoseconds)

DelayCalculatedTrackData (adds derived):
- long firstHopDelayTime (nanoseconds)
- long secondHopSentTime (nanoseconds)

Rationale: Preserve original millisecond timestamp while normalizing derived/processing metrics to nanoseconds.

## 4. Core Logic
TrackCalculations provides pure static helpers:
- firstHopDelay = updateTime - firstHopSentTime (nanoseconds)
- secondHopSentTime captured at send stage (nanoseconds, monotonic or system time depending on implementation)

CalculatorService loop:
1. Receive TrackData via IDataReceiver
2. Compute derived delay metrics
3. Emit DelayCalculatedTrackData via IDataSender
4. Repeat (currently infinite, graceful stop TBD)

## 5. Messaging & Serialization Evolution
- Prototype serialization: CSV (quick bootstrap)
- Migrated to JSON (line-oriented, numeric values only) for extensibility and clarity
- Minimal handcrafted serializer/parser (no external JSON library)
- Topics: incoming "track", outgoing "calculated"
- Pattern change: PUSH/PULL → PUB/SUB (requirement update)
  - Sender (publisher) binds
  - Receiver (subscriber) connects and filters topic

## 6. Ports & Adapters
- IDataReceiver: abstraction for source of raw track data
- IDataSender: abstraction for enriched data egress
- Console adapters: manual input/output use case / testing
- ZeroMQDataReceiver / ZeroMQDataSender: production-style transport using PUB/SUB

## 7. Build & Dependency Strategy
- Toolchain: g++ 8.5 (libstdc++ lacking newer GLIBCXX symbols)
- Issue: System libzmq referenced symbols GLIBCXX_3.4.29/30 → link failure
- Resolution: Vendor libzmq (v4.3.5) under third_party/libzmq and build from source with existing compiler
- CMake option: USE_VENDORED_ZMQ (ON) adds subdirectory and links internal target (resolved ABI mismatch)

## 8. Testing
- Simple assert-based tests for TrackCalculations and CalculatorService
- Updated to reflect long nanosecond / millisecond field types

## 9. Documentation
- Doxygen-style inline comments across ports, models, logic, adapters
- README enhanced: architecture rationale, units, vendoring explanation

## 10. Current Warning
- Ignored return value of zmq::socket_t::recv in ZeroMQDataReceiver (can be addressed by checking result, handling timeouts, or static_cast<void>(...))

## 11. Potential Improvements
1. Graceful shutdown (atomic flag + signal handler)
2. Robust recv error/timeout and malformed JSON handling
3. Unit tests for parsing edge cases (missing fields, invalid numbers)
4. Logging abstraction (structured logs) instead of std::cout
5. Optional: replace handwritten JSON with lightweight vetted library
6. CI pipeline including vendored lib build + basic integration test
7. Time source abstraction (monotonic vs wall clock) for deterministic tests
8. Metrics/telemetry hooks

## 12. Security / Reliability Notes
- Handwritten parser trusts numeric-only, assumes well-formed JSON; hardening recommended before untrusted input
- Blocking receive: potential for unresponsive shutdown; introduce non-blocking poll with timeout

## 13. Summary of State (Final at Export)
- Executable builds successfully linking against vendored libzmq
- Domain model solidified with explicit units
- PUB/SUB JSON messaging operational design in code
- Architecture adheres to hexagonal principles with clean ports
- Documentation comprehensive; improvement backlog identified

## 14. Files Central to Design
- domain/model/TrackData.hpp
- domain/model/DelayCalculatedTrackData.hpp
- domain/logic/TrackCalculations.hpp
- domain/logic/CalculatorService.*
- domain/ports/incoming/IDataReceiver.hpp
- domain/ports/outgoing/IDataSender.hpp
- adapters/incoming/ZeroMQDataReceiver.hpp
- adapters/outgoing/ZeroMQDataSender.hpp
- application/main.cpp
- CMakeLists.txt (vendoring toggle)

## 15. Next Optional Actions
If continuing:
- Implement shutdown & recv result handling
- Add parser validation tests
- Introduce logging + metrics
- Harden JSON parsing (bounds, type validation)

---
Generated: (UTC) <replace-with-build-time-if-needed>
