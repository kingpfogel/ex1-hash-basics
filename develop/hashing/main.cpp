#include <algorithm>
#include <cassert>
#include <cstring>
#include <chrono>
#include <iostream>
#include <optional>
#include <random>
#include <unordered_map>
#include <vector>
#include <cmath>

constexpr int M = 1 << 26;
// bitshift example: 1 = 0001 and 1 << 2 = 0100
// This function turns a hash into an index in [0, M).
// This computes h % M but since M is a power of 2,
// we can compute the modulo by using a bitwise AND to cut off the leading binary digits.
// As hash, we just use the integer key directly.
int hash_to_index(int h) {
	return h & (M - 1);
}

// This implements a hash table that uses chaining.
struct chaining_table {
	static constexpr const char *name = "chaining";

	struct chain {
		chain *next;
		int key;
		int value;
	};

	chaining_table()
	: heads{new chain *[M]{}} { }

	// Note: to simply the implementation, destructors and copy/move constructors are missing.

	void put(int k, int v) {
		auto idx = hash_to_index(k);
		auto p = heads[idx];

		if(!p) {
			heads[idx] = new chain{nullptr, k, v};
			return;
		}

		while(true) {
			assert(p);

			if(p->key == k) {
				p->value = v;
				return;
			}

			if(!p->next) {
				p->next = new chain{nullptr, k, v};
				return;
			}

			p = p->next;
		}
	}

	std::optional<int> get(int k) {
		auto idx = hash_to_index(k);
		auto p = heads[idx];

		while(p) {
			if(p->key == k)
				return p->value;

			p = p->next;
		}

		return std::nullopt;
	}

	chain **heads = nullptr;
};

// This hash table uses linear probing.
struct linear_table {
	static constexpr const char *name = "linear";

	struct cell {
		int key;
		int value;
		bool valid = false;
	};

	linear_table()
	: cells{new cell[M]{}} { }

	// Note: to simply the implementation, destructors and copy/move constructors are missing.

	void put(int k, int v) {
		int i = 0;

		while(true) {
			assert(i < M);

			auto idx = hash_to_index(k + i);
			auto &c = cells[idx];

			if(!c.valid) {
				c.key = k;
				c.value = v;
				c.valid = true;
				return;
			}

			if(c.key == k) {
				c.value = v;
				return;
			}

			++i;
		}
	}

	std::optional<int> get(int k) {
		int i = 0;

		while(true) {
			assert(i < M);

			auto idx = hash_to_index(k + i);
			auto &c = cells[idx];

			if(!c.valid)
				return std::nullopt;

			if(c.key == k)
				return c.value;

			++i;
		}
	}

	cell *cells = nullptr;
};

// For comparsion, an implementation that uses std::unordered_map.
// This is apples-to-oranges since std::unordered_map does not respect our fill factor.
struct stl_table {
	static constexpr const char *name = "stl";

	void put(int k, int v) {
		map[k] = v;
	}

	std::optional<int> get(int k) {
		auto it = map.find(k);
		if(it == map.end())
			return std::nullopt;
		return it->second;
	}

	std::unordered_map<int, int> map;
};

struct quadratic_table {
    static constexpr const char *name = "quadratic";

    struct cell {
        int key;
        int value;
        bool valid = false;
    };

    quadratic_table()
            : cells{new cell[M]{}} { }

    // Note: to simply the implementation, destructors and copy/move constructors are missing.

    void put(int k, int v) {
        int i = 0;
        //h(k)+i mod m = linear probing
        //h(k)+i/2+(i^2)/2 mod m quadratic probing
        while(true) {
            assert((i/2) + ((i*i)/2) < M);

            auto idx = hash_to_index(k + (i/2) + ((i*i)/2));
            auto &c = cells[idx];

            if(!c.valid) {
                c.key = k;
                c.value = v;
                c.valid = true;
                return;
            }

            if(c.key == k) {
                c.value = v;
                return;
            }

            ++i;
        }
    }

    std::optional<int> get(int k) {
        int i = 0;

        while(true) {
            assert((i/2) + ((i*i)/2) < M);

            auto idx = hash_to_index(k + (i/2) + ((i*i)/2));
            auto &c = cells[idx];

            if(!c.valid)
                return std::nullopt;

            if(c.key == k)
                return c.value;

            ++i;
        }
    }

    cell *cells = nullptr;
};



struct bucket_cuckoo_table {
    static constexpr const char *name = "bucket_cuckoo";
    const int d = 2;
    const int B = 2;
    const int log2B = log2(B);

    std::mt19937 prng{42};
    std::uniform_int_distribution<int> chooseHash{1, (2<<(32-26))};
    std::uniform_int_distribution<int> chooseFnc{0, d-1};
    int max_eviction_length = 2;

    struct cell {
        int key;
        int value;
        bool valid = false;
    };

    bucket_cuckoo_table()
        : cells{new cell[(M)]{}},
        as{new int[(d)]{}},
        bs{new int[(d)]{}}
        {
            rerollHashFunctions();
        }


    void rerollHashFunctions() {
        for (int i = 0; i < d; ++i) {
            as[i] = chooseHash(prng);
            bs[i] = chooseHash(prng);
        }
    }

    int hash_to_bucket_index(int h) {
        return (h & ((M >> log2B) - 1)) * B;
    }

    //d = hash functions
    //B = buckets => linear probe through buckets
    //hash table consists of m/B buckets - each containing B cells => linear probe B times per Bucket
    //Cuckoo hash functions h1, ..., hd select d bucket indices.

    bool rehash(int currentKey, int currentValue){
        rerollHashFunctions();
        cell *tmp = new cell[M];
        std::copy(cells, cells + M, tmp);
        delete[] cells;
        cells = nullptr;
        cells = new cell[M];
        //reinsert all other key-value-pairs
        bool succeeded;
        for(int i = 0; i<M; ++i){
            auto &c = cells[i];
            if(c.valid){
                succeeded = putHelper(c.key, c.value, 0, false);
                if(!succeeded){
                    delete[] cells;
                    cells = nullptr;
                    cells = new cell[M];
                    std::copy(tmp, tmp + M, cells);
                    delete[] tmp;
                    tmp = nullptr;
                    return true;
                }
            } else {
                //nothing to insert
            }
        }
        //reinsert current key-value-pair
        if(succeeded) {
            if(!putHelper(currentKey, currentValue, 0, false)){
                return true;
            };
        }

        return false;
    }
    bool putHelper(int k, int v, int chain = 0, bool hashAgain = true) {
        if(chain > max_eviction_length) {
            bool rehashSuccess = true;
            while(hashAgain && rehashSuccess) {
                rehashSuccess = rehash(k, v);
            }
            return false;
        }
        for(int i = 0; i < d; i++){
            auto idx = hash_to_bucket_index((as[i] * (unsigned)k + bs[i]) >> (32 - 26));

            for(int i2 = 0; i2 < B; i2++){
                auto &c = cells[idx+i2];
                if(!c.valid) {
                    c.key = k;
                    c.value = v;
                    c.valid = true;
                    return true;
                }

                if(c.key == k) {
                    c.value = v;
                    return true;
                }
            }
        }
        //wenn ich hier ankomme, dann sind wohl alle Zellen mit etwas anderem besetzt
        //waehle random h insert element dort und put(old.key, old.value)
        int f = chooseFnc(prng);
        int replacementIdx = hash_to_bucket_index((as[f] * (unsigned)k + bs[f]) >> (32 - 26));
        auto &c = cells[replacementIdx];
        int key = c.key, value = c.value;
        c.key = k;
        c.value = v;

        return putHelper(key, value, ++chain, hashAgain);
    }
    void put(int k, int v) {
        putHelper(k, v, 0);
    }

    std::optional<int> get(int k) {
        for(int i = 0; i < d; i++){
            auto idx = hash_to_bucket_index((as[i] * (unsigned)k + bs[i]) >> (32 - 26));

            for(int i2 = 0; i2 < B; i2++){
                auto &c = cells[idx+i2];
                if(!c.valid) {
                    continue;
                }

                if(c.key == k) {
                    return c.value;
                }
            }
        }

        return std::nullopt;
    }

    cell *cells = nullptr;
    int *as = nullptr;
    int *bs = nullptr;
};

// Helper function to evaluate a hash table algorithm.
// You should not need to touch this.
template<typename Algo>
void evaluate(float fill_factor) {
	Algo table;

	std::mt19937 prng{42};

	int n = M * fill_factor;

	std::cerr << "Generating random data..." << std::endl;

	// First roll a pool of keys that we will use.
	std::vector<int> pool;
	{
		std::uniform_int_distribution<int> distrib;
		for(int i = 0; i < n; ++i)
			pool.push_back(distrib(prng));
	}

	// Now generate a sequence of insertions that only use keys in our pool.
	std::vector<std::pair<int, int>> inserts;
	std::vector<std::pair<int, int>> truth; // The final values for each key, for sanity checking.

	int nv = 1;
	{
		std::uniform_int_distribution<int> distrib{0, n};
		std::unordered_map<int, int> temp;
		for(int i = 0; i < n; ++i) {
			int k = pool[distrib(prng)];
			inserts.push_back({k, nv});
			temp[k] = nv;
			++nv;
		}

		for(auto [k, v] : temp)
			truth.push_back({k, v});
	}

	std::cerr << "Performing insertions..." << std::endl;

	auto insert_start = std::chrono::high_resolution_clock::now();
	for(auto [k, v] : inserts)
		table.put(k, v);
	auto t_insert = std::chrono::high_resolution_clock::now() - insert_start;

	std::cerr << "Performing lookups..." << std::endl;

	// We want iterate through the keys in random order to avoid any potential bias.
	std::shuffle(truth.begin(), truth.end(), prng);

	int errors = 0;
	auto lookup_start = std::chrono::high_resolution_clock::now();
	for(auto [k, v] : truth) {
		auto r = table.get(k);
		if(!r) {
			++errors;
			continue;
		}

		if(*r != v) {
			++errors;
			continue;
		}
	}
	auto t_lookup = std::chrono::high_resolution_clock::now() - lookup_start;

	std::cerr << "There were " << errors << " errors" << std::endl;
	assert(!errors);

	std::cout << "algo: " << Algo::name << std::endl;
	std::cout << "m: " << M << std::endl;
	std::cout << "fill_factor: " << fill_factor << std::endl;
	std::cout << "time_insert: "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(t_insert).count()
			<< " # ms" << std::endl;
	std::cout << "time_lookup: "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(t_lookup).count()
			<< " # ms" << std::endl;
}

// Helper function to perform a microbenchmark.
// You should not need to touch this.
template<typename Algo>
void microbenchmark(float fill_factor) {
	Algo table;

	std::mt19937 prng{42};
	std::uniform_int_distribution<int> distrib;

	int n = M * fill_factor;

	std::cerr << "Running microbenchmark..." << std::endl;

	auto start = std::chrono::high_resolution_clock::now();
	int nv = 1;
	for(int i = 0; i < n; ++i)
		table.put(distrib(prng), nv++);
	auto t = std::chrono::high_resolution_clock::now() - start;

	std::cout << "algo: " << Algo::name << std::endl;
	std::cout << "m: " << M << std::endl;
	std::cout << "fill_factor: " << fill_factor << std::endl;
	std::cout << "time: "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(t).count()
			<< " # ms" << std::endl;
}

static const char *usage_text =
	"Usage: hashing [OPTIONS]\n"
	"Possible OPTIONS are:\n"
	"    --microbenchmark\n"
	"        Perform microbenchmarking.\n"
	"    --algo ALGORITHM\n"
	"        Select an algorithm {chaining,linear,stl,quadratic}.\n"
	"    --fill FACTOR\n"
	"        Set the fill factor.\n";

int main(int argc, char **argv) {
    bool do_microbenchmark = false;
    std::string_view algorithm;
    float fill_factor = 0.5;

    auto error = [] (const char *text) {
        std::cerr << usage_text << "Usage error: " << text << std::endl;
        exit(2);
    };

    // Argument for unary options.
    const char *arg;

    // Parse all options here.

    char **p = argv + 1;

    auto handle_nullary_option = [&] (const char *name) -> bool {
        assert(*p);
        if(std::strcmp(*p, name))
            return false;
        ++p;
        return true;
    };

    auto handle_unary_option = [&] (const char *name) -> bool {
        assert(*p);
        if(std::strcmp(*p, name))
            return false;
        ++p;
        if(!(*p))
            error("expected argument for unary option");
        arg = *p;
        ++p;
        return true;
    };

    while(*p && !std::strncmp(*p, "--", 2)) {
        if(handle_nullary_option("--microbenchmark")) {
            do_microbenchmark = true;
        }else if(handle_unary_option("--algo")) {
            algorithm = arg;
        }else if(handle_unary_option("--fill")) {
            fill_factor = std::atof(arg);
        }else{
            error("unknown command line option");
        }
    }

    if(*p)
        error("unexpected arguments");

    // Verify that options are correct and run the algorithm.

    if(algorithm.empty())
        error("no algorithm specified");

    if(do_microbenchmark) {
        if(algorithm == "chaining") {
            microbenchmark<chaining_table>(fill_factor);
        }else if(algorithm == "linear") {
            microbenchmark<linear_table>(fill_factor);
        }else if(algorithm == "stl") {
            microbenchmark<stl_table>(fill_factor);
        }else if(algorithm == "quadratic") {
            microbenchmark<quadratic_table>(fill_factor);
        }else if(algorithm == "bucket_cuckoo") {
            microbenchmark<bucket_cuckoo_table>(fill_factor);
        }else{
            error("unknown algorithm");
        }
    }else{
        if(algorithm == "chaining") {
            evaluate<chaining_table>(fill_factor);
        }else if(algorithm == "linear") {
            evaluate<linear_table>(fill_factor);
        }else if(algorithm == "stl") {
            evaluate<stl_table>(fill_factor);
        }else if(algorithm == "quadratic") {
            evaluate<quadratic_table>(fill_factor);
        }else if(algorithm == "bucket_cuckoo") {
            evaluate<bucket_cuckoo_table>(fill_factor);
        }
        else{
            error("unknown algorithm");
        }
    }
}
