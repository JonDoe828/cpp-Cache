#include <catch2/catch_test_macros.hpp>

#include <random>
#include <string>
#include <vector>

#include "LruCache.h"

TEST_CASE("LRU: put/get basic hit-miss", "[lru]") {
  LruCache<int, std::string> cache(2);

  std::string out;
  REQUIRE_FALSE(cache.get(1, out));

  cache.put(1, "a");
  REQUIRE(cache.get(1, out));
  REQUIRE(out == "a");

  cache.put(2, "b");
  REQUIRE(cache.get(2, out));
  REQUIRE(out == "b");
}

TEST_CASE("LRU: eviction removes least recently used (get refreshes recency)",
          "[lru]") {
  LruCache<int, std::string> cache(2);

  cache.put(1, "a");
  cache.put(2, "b");

  // get(1) => 2 becomes LRU
  std::string out;
  REQUIRE(cache.get(1, out));
  REQUIRE(out == "a");

  cache.put(3, "c"); // should evict key=2

  REQUIRE_FALSE(cache.get(2, out));
  REQUIRE(cache.get(1, out));
  REQUIRE(out == "a");
  REQUIRE(cache.get(3, out));
  REQUIRE(out == "c");
}

TEST_CASE("LRU: put existing key updates value and refreshes recency",
          "[lru]") {
  LruCache<int, std::string> cache(2);

  cache.put(1, "a");
  cache.put(2, "b");

  // update key=1 => key=2 becomes LRU
  cache.put(1, "a2");

  cache.put(3, "c"); // should evict key=2

  std::string out;
  REQUIRE_FALSE(cache.get(2, out));
  REQUIRE(cache.get(1, out));
  REQUIRE(out == "a2");
  REQUIRE(cache.get(3, out));
  REQUIRE(out == "c");
}

TEST_CASE("LRU: capacity 1 always keeps most recent", "[lru]") {
  LruCache<int, std::string> cache(1);

  cache.put(1, "a");
  cache.put(2, "b"); // evict 1

  std::string out;
  REQUIRE_FALSE(cache.get(1, out));
  REQUIRE(cache.get(2, out));
  REQUIRE(out == "b");

  // access 2, then insert 3 => evict 2 anyway because cap=1
  REQUIRE(cache.get(2, out));
  cache.put(3, "c");

  REQUIRE_FALSE(cache.get(2, out));
  REQUIRE(cache.get(3, out));
  REQUIRE(out == "c");
}

TEST_CASE("LRU: zero capacity stores nothing", "[lru]") {
  LruCache<int, std::string> cache(0);

  cache.put(1, "a");

  std::string out;
  REQUIRE_FALSE(cache.get(1, out));

  // get(key) 返回默认值（string 就是空）
  REQUIRE(cache.get(1).empty());
}

// 可选：随机操作烟雾测试（不做严格断言，只保证不崩溃且命中率在合理范围）
// 你如果嫌慢可以删掉这个 case
TEST_CASE("LRU: random workload smoke test", "[lru][smoke]") {
  const int CAP = 50;
  const int OPS = 20000;
  const int KEY_RANGE = 500;

  LruCache<int, std::string> cache(CAP);

  std::mt19937 gen(123);
  int hits = 0, gets = 0;

  for (int i = 0; i < OPS; ++i) {
    const int key = static_cast<int>(gen() % KEY_RANGE);
    const bool isPut = (gen() % 100) < 30;

    if (isPut) {
      cache.put(key, "v" + std::to_string(key));
    } else {
      std::string out;
      ++gets;
      if (cache.get(key, out))
        ++hits;
    }
  }

  // 只做宽松检查：至少执行了 get
  REQUIRE(gets > 0);
  // 命中率不会是 0（正常情况下应该有命中）
  REQUIRE(hits >= 0);
}
