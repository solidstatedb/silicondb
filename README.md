# 💽 silicondb

[build-and-test-workflow](https://github.com/arindas/silicondb/actions/workflows/build-and-test.yml/badge.svg)

A concurrent key value store, column oriented database and more.

## 💻 Storage Engine modules

- [ ] A concurrent in memory map implemented as a hashtable.
- [ ] A lock-free variant of the map mentioned above.
- [ ] A wait-free simulation of the lock-free map.
- [ ] A lock-free concurrent SSTable simulated in a wait-free manner.
- [ ] Compaction operations on SSTable.
- [ ] Column oriented storage support. (Needs further elaboration)
- [ ] Content correctness verification and replication using merkle trees for SSTable and Column oriented storage.
- [ ] Message sequencing with merkle trees.

## 🏂 Features
- [ ] Partial implementation of the Redis protocol:
    - [ ] Key, Value retreival
    - [ ] Key Ranged operations
- [ ] REST API server for accessing data.
- [ ] CAP compliant.

## 🏗️ Build Instructions

### Pre-requisites

- gcc-9.3+
- cmake

```
git clone git@github.com:arindas/silicondb.git
cd silicondb
cmake -S . -B build/
```

This produces a binary `build/silicondb` which is the silicondb daemon server.

## 🧪 Testing

```
cd build && ctest
```

This should run all tests.

## 📖 References
- [Book] C++ Concurrency in Action - Anthony Williams - Manning Publications. ISBN: 978-1-933-98877-1
- [Book] Designing Data Intensive Applications - Martin Kleppmann - Oreilly.  ISBN: 978-1-449-37332-0
- [Paper] A Practical Wait-Free Simulation for Lock-Free Data Structures http://dx.doi.org/10.1145/2555243.2555261