# Project 3 Design Log
**Author:** Nate Robison · **P2 code used:** instructor reference
**Tools used:** CLion editor (including autocomplete)

## 1. Hash table
<!-- Chaining or open addressing, and why. Your hash function, load-factor
     threshold, and resize policy. Paste your 10,000-tool numbers
     (bucket_count, load_factor, max_probe_length) next to the alternative
     hash's longest chain. Explain briefly why add() is amortized O(1). -->
10,000 tool numbers:
- bucket_count: 16384
- load_factor: 0.610352
- max_probe_length: 6

Weak hash longest chain: 9000

## 2. Pointer stability
<!-- Why does a Tool* returned by find() survive a resize in your design? -->

## 3. Calculator trace
<!-- Trace the values and ops stacks for 2 * (3 + 4) - 5, one token at a
     time, in your own words. -->

## 4. Errors as values
<!-- Why do tools, resolve() and calc_evaluate() return errors instead of
     throwing? When IS an exception the right choice? -->

## 5. What I would design differently
