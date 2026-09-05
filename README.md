# LOBSim: Limit Order Book Reconstruction Engine

A C++20 engine that replays historical Nasdaq market data message by message and rebuilds the full limit order book at every event. The book is validated against an independent reference (LOBSTER) at each step. This is the first layer of a larger project: a backtesting engine with realistic fill simulation (queue position, latency, adverse selection) on top of a provably correct book.

Current state: the reconstruction engine is complete and validated against LOBSTER to the extent the LOBSTER sample data allows. That extent turned out to be bounded, for reasons described below. The next phase replaces the CSV front-end with a raw Nasdaq ITCH 5.0 binary parser, which removes the bound.

## Results

Results from tag `v0.1-lobster`. INTC, 2012-06-21, 09:30:00 to 16:00:00, LOBSTER level-10 sample.

| Metric | Value |
|---|---|
| Messages processed | 624,040 |
| Events replayed (after seed) | 624,038 |
| Replay time, book only | 172 ms |
| Throughput, book only | ~3.6M events/sec, single thread |
| Load time (two CSVs, ~29M fields) | 1,441 ms |
| Distinct order IDs added | 304,790 |
| Price range | $26.52 to $27.68 |
| Divergences from reference | 118 (price, side) pairs, all traced to the dataset, none to engine defects |
| Unit tests | all passing (doctest) |

Hardware and compiler for the throughput number: Apple M5 Max, Apple clang 21.0.0 (arm64-apple-darwin25.6.0), macOS 26.6.2, release flags `-O2 -g`. Throughput is measured with `--no-diff`, so it covers message dispatch and book mutation only, not the reference comparison.

Message type distribution for the day:

| LOBSTER type | Meaning | Count | Share |
|---|---|---|---|
| 1 | New limit order | 304,790 | 48.84% |
| 2 | Partial cancel | 8,028 | 1.29% |
| 3 | Full delete | 278,739 | 44.67% |
| 4 | Visible execution | 28,924 | 4.63% |
| 5 | Hidden execution | 3,559 | 0.57% |
| 6 | Cross / auction | 0 | 0.00% |
| 7 | Trading halt | 0 | 0.00% |

Cancels and deletes outnumber executions by 9.9 to 1. Most orders that enter this book leave it without trading.

## The validation problem

The original plan was simple: replay LOBSTER's message file, rebuild the book, and assert that the top 5 levels match LOBSTER's orderbook file at every one of the ~624k events. That claim is not achievable from a level-10 LOBSTER file, and the reason is worth understanding because it is a property of the data, not the code.

### What LOBSTER's message file contains

LOBSTER reconstructs the full book from the raw Nasdaq ITCH feed, then exports two CSVs for a requested number of levels (here 10). The orderbook file holds the top-N levels after every event. The message file holds the events that caused those changes.

The message file only contains events whose price falls inside the top-N levels at the time of the event. An order resting 30 levels deep never appears when it is added. If the market later moves toward that price and it enters the top 10, the order's depth shows up in the orderbook file, but there is no message that put it there. If that order is then cancelled while still visible, the cancel does appear.

### Why the seed does not fix it

The sample window starts at 09:30:00, but the book already contains thousands of shares from pre-market orders whose Add messages are not in the window. The engine handles this by seeding: it reads reference row 1 and populates each visible price level with the correct aggregate size, then replays from message 2 onward.

The seed gives 10 levels of depth on each side. On a penny-spread, tick-constrained stock like INTC, 10 levels is about 10 cents of price coverage. On 2012-06-21 the seed band was $27.42 to $27.51 on the bid side and $27.52 to $27.61 on the ask side. INTC ranged from $26.52 to $27.68 that day, about 116 ticks. Every tick the price moved past the edge of the seed band brought in depth whose Adds were never in the message stream.

The contamination is fixed in price space, not rank space. It sits at prices the engine has never seen an Add for, and it becomes visible to a top-5 comparison when the levels above it drain. From seed row 1, a contaminated price first reached level 5 at event 647. From seed row 10,000, at event 10,639. In both cases roughly 640 events, which is about how long it takes the market to eat through five levels of INTC depth.

### What this means for the claim

Exact top-5 match across a full trading day cannot be produced from a level-10 LOBSTER file by any implementation. The information is not in the file.

What can be claimed, and what this project claims: every divergence between the engine's book and the reference has been audited and attributed to this artifact. None is attributable to a defect in the engine. Details in the audit section below.

The fix is to reconstruct from the full ITCH feed starting at the beginning of the trading day. There is no window and no seed; every Add is present. See Roadmap.

## Divergence audit
## IMPORTANT

With seed row 1 and strict top-5 comparison, 118 distinct (price, side) pairs diverged from the reference at some point during the day. Each was classified by where it sat relative to the seed band and by the sign of the delta (engine size minus reference size).

**Class 1: outside the seed band, engine has zero.** About 90 of the 118. Prices below $27.42 on the bid and above $27.61 on the ask. The engine never saw an Add at these prices because they were outside LOBSTER's window when the orders arrived. Not recoverable from this file.

**Class 2: inside the seed band, positive delta.** A removal (cancel, delete, or execution) arrived for an order ID the engine had never seen, at a price where the engine's anonymous seeded depth was already exhausted. The engine refused to take the level negative. It is overstated by exactly the amount it refused. Correct behavior producing a wrong number.

**Class 3: inside the seed band, negative delta.** Eight rows. These were the only candidates for an engine bug, since inside the seed band the engine started with correct depth and clamping can only make it too high.

| Side | Price | First divergence (event) | Delta |
|---|---|---|---|
| ASK | $27.57 | 742 | -100 |
| ASK | $27.59 | 680 | -400 |
| ASK | $27.60 | 682 | -1,000 |
| BID | $27.42 | 371 | -100 |
| BID | $27.43 | 10,460 | -900 |
| BID | $27.44 | 10,382 | -2,900 |
| BID | $27.45 | 6,367 | -500 |
| BID | $27.46 | 9,396 | -100 |

Each was traced by logging every removal at that price and side: order ID, whether the ID was known, size removed, resulting total, and remaining anonymous depth. In all eight, anonymous removals only consumed anonymous seeded depth, and the anonymous pool never went negative. The arithmetic balanced.

The mechanism is the window sliding sideways. A price inside the seed band can drop out of LOBSTER's visible top 10 when the market moves away from it. An order added at that price during the gap generates no message. When the market returns and the order is cancelled, the cancel appears. The engine sees a removal with no matching Add, has legitimate seeded depth to take it from, and ends up short by that order's size. No clamp fires because nothing went negative.

## Data

LOBSTER free sample files: https://lobsterdata.com/info/DataSamples.php

Download `LOBSTER_SampleFile_INTC_2012-06-21_10.zip` and extract into `data/`:

```
data/INTC_2012-06-21_34200000_57600000_message_10.csv
data/INTC_2012-06-21_34200000_57600000_orderbook_10.csv
```

The numbers in the filename are milliseconds after midnight: 34200000 is 09:30:00, 57600000 is 16:00:00. The `data/` directory is gitignored.

INTC was chosen over AAPL because in 2012 INTC was a low-priced, tick-constrained stock with a one-cent spread and deep queues at every level, which is the regime where queue-position effects matter. That same property is what makes 10 levels a narrow price band, and is the reason the validation problem above shows up within seconds.

### LOBSTER format notes

Message file, 6 columns: Time, Type, OrderID, Size, Price, Direction.

- Time is seconds after midnight with fractional nanoseconds. The reader parses it as two integers and combines them; it never goes through `double`.
- Price is in units of 1/10000 dollar. 265200 is $26.52. The engine stores prices as integer ticks (price / 100 for a one-cent tick) and asserts `price % 100 == 0` on load.
- Direction is 1 for the bid side, -1 for the ask side. On an execution (type 4 or 5), Direction refers to the side of the resting order that was hit, not the aggressor. A type-4 with Direction -1 means a resting sell was executed, so a buyer crossed the spread.
- Type 2 (partial cancel): Size is the amount removed, not the new remaining size.
- Types 5, 6, 7 do not change visible depth and are handled as explicit no-ops.

Orderbook file, 40 columns for level 10: ask price 1, ask size 1, bid price 1, bid size 1, ask price 2, and so on. Row k of the orderbook file is the book state after message k. Unoccupied levels are padded with a large sentinel price and size 0; the reader treats these as "no level," never as a price.

## Building

Requires CMake 3.20+ and a C++20 compiler. On macOS, `xcode-select --install` provides clang, lldb, and make.

Two out-of-source build directories, because the build type is baked in at configure time:

```bash
# Debug: no optimization, full debug info, AddressSanitizer + UndefinedBehaviorSanitizer
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j

# Release: optimized, debug info retained for stack traces
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j
```

Rerun the `cmake -S` step only when `CMakeLists.txt` changes or a source file is added.

### Compiler flags

| Build | Flags | Purpose |
|---|---|---|
| Debug | `-g -O0 -fsanitize=address,undefined -Wall -Wextra` | Correctness. Sanitizers report memory errors and undefined behavior at the moment they happen, with a stack trace. Roughly 2x slower and 3x more memory than an unsanitized build. |
| Release | `-O2 -g -Wall -Wextra` | Measurement. All throughput numbers come from this build. `-g` is retained so crashes in the optimized build still produce readable stack traces. |

Flag by flag:

- `-g`: emit debug info (source line mapping, symbol names). No runtime cost.
- `-O0`: no optimization. Code matches source line for line, so single-stepping works and no variable is optimized away. Also 10x to 50x slower on tight loops. Never benchmark this build.
- `-O2`: standard optimization level. Inlining, loop transforms, register allocation.
- `-Wall -Wextra`: two curated warning sets. Signed/unsigned comparison warnings are treated as bugs, not noise.
- `-fsanitize=address`: heap and stack buffer overflow, use-after-free, double-free, use-after-return. Leak detection is not available on macOS.
- `-fsanitize=undefined`: signed overflow, misaligned access, out-of-range shifts, invalid enum values, null dereference. Directly relevant for the upcoming binary ITCH parser, where unaligned reads from a byte buffer are undefined behavior.

The sanitizer flags are applied to both the compiler and the linker in the debug build.

## Running

```bash
./build/release/replay \
  --messages data/INTC_2012-06-21_34200000_57600000_message_10.csv \
  --book     data/INTC_2012-06-21_34200000_57600000_orderbook_10.csv \
  --seed 1 --levels 5
```

### Flags

| Flag | Argument | Default | Meaning |
|---|---|---|---|
| `--messages` | path | required | LOBSTER message CSV |
| `--book` | path | required | LOBSTER orderbook CSV |
| `--seed` | row index | 1 | Reference orderbook row to seed the book from before replay begins. Replay starts at the next message. |
| `--levels` | N | 5 | Number of levels per side to compare against the reference. Levels beyond the top 5 are known to diverge (see the validation problem). |
| `--limit` | N | none | Stop after N messages. Used with `--seed` to isolate a window for debugging. |
| `--no-diff` | none | off | Skip the reference comparison. For timing runs only. Output states that no comparison was performed. |

Exit codes: 0 for a clean match over the compared range, 1 for a mismatch, 2 for a usage or file error.

### Output

The load summary prints message count, reference row count and whether they match, first and last timestamp, price range, distinct order IDs, and the message type histogram. Replay then either completes or stops at the first mismatch, printing:

- the event index, decoded message (type, order ID, side, price in dollars and ticks, size, time)
- the failing field (side, level, price or size), expected and actual
- the engine's top-N and the reference top-N side by side

Example mismatch output from a run seeded at row 10,000:

```
*** MISMATCH at event 10639 ***
  message   Delete  id 18746127  B  $27.49 (274900 ticks)  x400  @ 09:31:20
  field     BID PRICE level 5
  expected  274400
  actual    274300
                                 MINE   |                      REFERENCE
  lvl           ask      sz         bid      sz  |          ask      sz         bid      sz
    1         27.51    3365       27.48    1073  |        27.51    3365       27.48    1073
    2         27.52    1100       27.47     700  |        27.52    1100       27.47     700
    3         27.53    1700       27.46    1537  |        27.53    1700       27.46    1537
    4         27.54    2100       27.45    1500  |        27.54    2100       27.45    1500
    5         27.55    2765       27.43     500  |        27.55    2765       27.44    3900
```

This delete emptied the $27.49 level. Every bid level shifted up one rank, and $27.44, which the engine has no depth at, moved from level 6 into level 5. The divergence existed before this event; this event made it visible to a top-5 comparison. This is a class-1 divergence.

## Validation methodology

The reference (LOBSTER's orderbook file) was produced by a different team, from the raw exchange feed, with a different implementation. Agreement between two independent reconstructions is the strongest correctness evidence available without access to the exchange itself.

The engine compares after every event. Both the engine's book and the reference row are normalized into the same `TopOfBook` struct (prices in ticks, occupancy counts instead of sentinel values) before comparison. The diff checks occupancy first, then walks levels in rank order checking price then size, and reports the first field that differs. Reporting only the first difference is deliberate: a single missing level shifts everything below it, so one root cause produces many differing fields.

Before each diff, the engine's own `TopOfBook` is checked for internal sanity: bid prices strictly descending, ask prices strictly ascending, no occupied level with zero size, best bid below best ask. This separates "the book state is wrong" from "the book state is fine but the extraction is wrong."

Invariants asserted during replay:

- an order found by ID has the price the message says it does
- a level's total size is at least the amount being removed, before every subtraction
- a level that reaches zero size is erased, not left at zero
- the book is never crossed

Removals for unknown order IDs are expected (they reference pre-window depth) and are handled by decrementing the aggregate level directly, clamped at zero. They are counted; the count plateaus once the seeded depth has churned out.

## Architecture

```
LOBSTER CSV reader ──┐
                     ├──▶ NormalizedMessage ──▶ apply() ──▶ OrderBook ──▶ TopOfBook ──▶ BookDiff
ITCH 5.0 parser ─────┘   (planned)                                            ▲
                                                                              │
                                                    ReferenceBookReader ──────┘
```

`NormalizedMessage` is a flat struct (`uint64_t timestamp_ns`, type, `uint64_t order_id`, `uint32_t size`, `int64_t price_ticks`, `int8_t side`) and is the contract between every layer. `apply()` is a free function taking one message; it does not know where the message came from, which is what lets the ITCH front-end drop in without touching the book.

The current `OrderBook` is deliberately the simple version:

```cpp
std::unordered_map<uint64_t, OrderRef> orders_;              // id -> side, price, size
std::map<int64_t, uint64_t, std::greater<int64_t>> bids_;    // price -> aggregate size
std::map<int64_t, uint64_t> asks_;
```

Two sorted maps holding aggregate size per price, one hash map so removals can find their order by ID. This was built first because it is easy to get right, and the validation harness was needed before any optimization. The performance rewrite (arena-allocated intrusive lists, tick-indexed flat price array) comes after the ITCH validation, so that the rewrite is measured against a correct baseline.

### Layout

```
CMakeLists.txt
include/            headers
src/                lobster_reader, reference_book, book_diff, order_book, main
tests/              doctest cases (test_main.cpp holds the doctest main)
third_party/        doctest.h (vendored, single header, MIT)
data/               LOBSTER CSVs (gitignored)
docs/               notebook, decisions, benchmarks
build/              out-of-source builds (gitignored)
```

## Tests

```bash
cmake --build build/debug -j
./build/debug/tests
```

Run under the debug build so the sanitizers are active. `./build/debug/tests -ltc` lists cases; `-tc="*pattern*"` runs a subset.

Coverage includes: identical rows compare equal; size and price differences caught at the correct level and side; engine book thinner than reference (occupancy reported, no out-of-bounds read); reference thinner than engine; sanity checks reject crossed books, zero-size levels, and out-of-order prices; add/reduce/remove/execute against known and unknown IDs; a full delete emptying a level; a partial reduce leaving size behind.

## Known limitations

- **Top-5 exact match across the full day is not achievable from this dataset.** See the validation problem. The engine is exactly correct from any seed until the reference dataset's window artifact reaches the compared levels, roughly 640 events on this day.
- **Levels beyond 5 diverge earlier.** The window artifact reaches deeper levels first.
- **Class-3 divergences (silent absorption) are not self-detectable.** The engine can flag clamps and out-of-band prices on its own. It cannot know that a removal it honored cleanly corresponded to an Add it never saw. Those are found only by diffing.
- **No replace message.** LOBSTER decomposes exchange-level replaces into delete-then-add before export, so the engine has never processed one. The ITCH parser will have to do that decomposition itself, since ITCH carries replace as a single message.
- **No per-order FIFO within a level.** The book holds aggregate size per price. This is sufficient for the LOBSTER comparison (which is also aggregate) and insufficient for queue-position tracking, which is a later layer.
- **The reader loads everything into memory.** Fine for 624k rows; not viable for a full ITCH day (~100M messages). The ITCH front-end will stream.

## Benchmarks

| Tag | Data | Events | Load | Replay (book only) | Throughput | Hardware | Compiler | OS | Flags |
|---|---|---|---|---|---|---|---|---|---|
| `v0.1-lobster` | INTC 2012-06-21 L10 | 624,038 | 1,441 ms | 172 ms | ~3.6M ev/s | Apple M5 Max | Apple clang 21.0.0 arm64 | macOS 26.6.2 | `-O2 -g` |

Replay time excludes the reference diff (`--no-diff`). Debug-build timings are not recorded; that build runs under sanitizers at `-O0` and is roughly 8x slower.

## Roadmap

1. **ITCH 5.0 binary parser.** Nasdaq's public sample files (BX and PSX venues, 2019-2020) in BinaryFILE format: 2-byte big-endian length prefix, then the message. Messages A, F, E, C, X, D, U, plus system events and stock directory. Field-by-field `memcpy` and byteswap; no pointer casts, no packed structs. Streaming, not loaded.
2. **Full-day validation from ITCH.** Reconstruct from the start of the trading day with no seed. Diff against an independent open-source reconstructor (meatpy or martinobdl/ITCH) run on the same file. Unknown order IDs become a hard assertion failure, since the full feed contains every Add.
3. **Performance rewrite.** Arena-allocated order pool with `uint32_t` indices, intrusive FIFO per level, tick-indexed price array, `mmap` input. Measured against the `std::map` baseline above.
4. **Matching engine and fill simulation.** Shadow book for simulated orders (never touching the historical book), queue-position tracking distinguishing cancel-driven from execution-driven advancement, configurable latency, markout measurement.
5. **Strategy API.** `on_book_update` / `on_fill` callbacks; naive two-sided market maker, queue-aware market maker, liquidity-taking baseline. PnL, fill rate, markout at 100ms and 1s, inventory, slippage vs arrival.

## References

- LOBSTER data description: https://lobsterdata.com/info/DataStructure.php
- Nasdaq TotalView-ITCH 5.0 specification: https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf
- Nasdaq sample ITCH files: https://emi.nasdaq.com/ITCH/
- doctest: https://github.com/doctest/doctest
