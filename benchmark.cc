// Created by chengke on 2021/11/23

#include <asm-generic/int-ll64.h>
#include <cstdint>
#include <limits>
#include <random>
#include <sys/select.h>
#include <unordered_set>
#include <cassert>
#include "city.h"
#include "cuckoo_filter.h"
#include "yeah_filter.h"

constexpr size_t num_fp_test = 100000;
constexpr int randseed = 20211124;

size_t cuckoo_cityhash(uint32_t x) 
{
	return CityHash64((const char*) &x, sizeof(x));
}

#define BENCHMARK_FILTER CuckooFilter
/* #define BENCHMARK_FILTER YeahFilter */

/**
 * @param capacity     The number of element fingerprints.
 * @param bits_per_key The fingerprint length.
 * @param way          The number of fingerprints in a bucket. 
 * @param num_elem     The number of elements to be inserted in to the filter.
 * We can also calcuate (1) num_buckets = capcacity / way, and (2) load factor = num_elem / capacity.
 */
template<int capacity, int bits_per_key, int way, int num_elem>
void run_benchmark() 
{
	std::mt19937 gen(randseed);
	// closed interval
	std::uniform_int_distribution<uint32_t> distrib(0, std::numeric_limits<uint32_t>::max());
	auto generate_rand = [&]() {
		return distrib(gen);
	};

	std::unordered_set<uint32_t> element_set;
	std::vector<uint32_t> element_sequence;

	for (size_t loop = 0; loop < num_elem; loop++) {
		uint32_t x;
		do {
			x = generate_rand();
		} while (element_set.count(x));

		element_sequence.push_back(x);
		element_set.insert(x);
	}

	BENCHMARK_FILTER<bits_per_key, uint32_t, decltype(&cuckoo_cityhash)> 
		filter(cuckoo_cityhash, capacity);

	size_t insert_count = 0;
	for (auto x : element_sequence) {
		if (filter.insert(x)) {
			insert_count += 1;
		}
	}

	size_t fp_count = 0;
	for (size_t loop = 0; loop < num_fp_test; loop++) {
		uint32_t x;
		do {
			x = generate_rand();
		} while (element_set.count(x));

		if (filter.query(x)) {
			fp_count += 1;
		}
	}
	log_info("load_factor: %.5f\n", (float) insert_count / capacity);
	log_info("ideal_load_factor: %.5f\n", (float) num_elem / capacity);
	log_info("FP_rate: %.5f\n", (float) fp_count / num_fp_test);
}

int main()
{
/* template<int capacity, int bits_per_key, int way, int num_elem> */
/* run_benchmark< 1 << 10,      12,       4,        1 << 10 >(); */
	run_benchmark< 1 << 10,      12,       16,        1 << 10 >();
	return 0;
}
