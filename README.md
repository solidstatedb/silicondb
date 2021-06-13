# silicondb
A concurrent key value store, column oriented database and more.

<p align="center">
    <img src="./assets/logo.jpg" alt="silicondb" width="300" />   
</p>

<p align="center">
    <a href="https://github.com/arindas/silicondb/actions"><img src="https://github.com/arindas/silicondb/workflows/build-and-test/badge.svg" alt="Build Status"></a>
    <a href="https://github.com/arindas/silicondb/actions"><img src="https://github.com/arindas/silicondb/workflows/doxygen-docs/badge.svg" alt="Doxygen docs Publishing Status"></a> 
</p>

## Overview

silicondb was a project created to understand how modern NoSQL scalable distributed databases work. In particular
the author intends to learn implementing lock free data structures, log structured merge trees,
column-oriented data storage, merkle trees and data replication at scale.

This project doesn't aim to compete with existing NoSQL databases, neither does it provided any novel feature
addressing a particular pain point. It does however provide implementations of wait-free simulated lock free
data structures. (See references)

However, this project still aims to be a scalable, robust alternative to modern databases with production level code.

silicondb prioritizes efficiency on SSDs. Hence we focus on log structured merge trees as the driving data
storage data structure instead of b-trees.

## 💻 Storage Engine modules

- [x] A concurrent in memory map implemented as a hashtable.
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

## License

`silicondb` is licensed under the MIT License See [LICENSE](./LICENSE) for the full license text.

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Farindas%2Fsilicondb.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Farindas%2Fsilicondb?ref=badge_large)


## 📖 References
- [Book] C++ Concurrency in Action - Anthony Williams - Manning Publications. ISBN: 978-1-933-98877-1
- [Book] Designing Data Intensive Applications - Martin Kleppmann - Oreilly.  ISBN: 978-1-449-37332-0
- [Paper] A Practical Wait-Free Simulation for Lock-Free Data Structures http://dx.doi.org/10.1145/2555243.2555261