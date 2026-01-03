#ifndef PLUSMINUSONERMQ_HPP
#define PLUSMINUSONERMQ_HPP

#include <vector>
#include <cstddef>
#include <bitset>

class PlusMinusOneRMQ {
    public:
        PlusMinusOneRMQ() {}
        PlusMinusOneRMQ(const std::vector<size_t> _arr) : arr(_arr) {
            const size_t n = arr.size();

            const size_t log_2 = 63 - __builtin_clzll(n);
            s = std::max<size_t>(1, log_2 >> 1);

            const size_t block_size = (n + s - 1) / s;
            blocks = std::vector<size_t>(block_size);

            for(size_t b = 0; b < block_size; b++){
                size_t start = b * s;
                size_t end = std::min(start + s, n);

                size_t mask = 0;
                for(size_t i = start + 1; i < end; i++){
                    if(arr[i] < arr[i - 1]) {
                        mask |= (1ULL << (i - start - 1));
                    }
                }
                blocks[b] = mask;
            }

            const size_t count_classes_of_equivalence = 1ULL << (s - 1);
            normalized_block_RMQ_table = std::vector<std::vector<std::vector<size_t>>>(
                count_classes_of_equivalence, std::vector<std::vector<size_t>>(s, std::vector<size_t>(s))
            );

            std::vector<int> depth(s);
            for(size_t t = 0; t < count_classes_of_equivalence; t++){
                for(size_t i = 1; i < s; i++){
                    if(t & (1ULL << (i - 1))) {
                        depth[i] = depth[i - 1] - 1;
                    } else {
                        depth[i] = depth[i - 1] + 1;
                    }
                }

                for(size_t i = 0; i < s; i++){
                    normalized_block_RMQ_table[t][i][i] = i;
                    int minDepth = depth[i];
                    size_t minIdx = i;

                    for(size_t j = i + 1; j < s; j++){
                        if(depth[j] < minDepth){
                            minDepth = depth[j];
                            minIdx = j;
                        }
                        normalized_block_RMQ_table[t][i][j] = minIdx;
                    }
                }
            }

            std::vector<size_t> blocks_minimums(block_size);

            for(size_t b = 0; b < block_size; b++){
                size_t start = b * s;
                size_t end = std::min(start + s, n);

                size_t minIdx = start;
                for(size_t i = start + 1; i < end; i++){
                    if(arr[i] < arr[minIdx])
                        minIdx = i;
                }
                blocks_minimums[b] = minIdx;
            }

            const size_t log_2_block_size = 63 - __builtin_clzll(block_size);
            block_min_sparse_table = std::vector<std::vector<size_t>>(block_size, std::vector<size_t>(log_2_block_size + 1));

            for(size_t i = 0; i < block_size; i++) {
                block_min_sparse_table[i][0] = blocks_minimums[i];
            
            }

            for(size_t j = 1; j <= log_2_block_size; j++){
                for(size_t i = 0; i + (1ULL << j) <= block_size; i++){
                    if(arr[block_min_sparse_table[i][j - 1]] <= arr[block_min_sparse_table[i + (1ULL << (j - 1))][j - 1]]) {
                        block_min_sparse_table[i][j] = block_min_sparse_table[i][j - 1];
                    } else {
                        block_min_sparse_table[i][j] = block_min_sparse_table[i + (1ULL << (j - 1))][j - 1];
                    }
                }
            }
        }

        size_t getRMQ(size_t i, size_t j)  const {
            if(i > j){
                i ^= j;
                j ^= i;
                i ^= j;
            }

            size_t b1 = i / s;
            size_t b2 = j / s;

            if(b1 == b2){
                size_t t = blocks[b1];
                size_t i = i % s;
                size_t j = j % s;
                return b1 * s + normalized_block_RMQ_table[t][i][j];
            }

            size_t t1 = blocks[b1];
            size_t i1 = i % s;
            size_t block_end_index = std::min(s - 1, arr.size() - b1 * s - 1);
            size_t min_index = b1 * s + normalized_block_RMQ_table[t1][i1][block_end_index];

            size_t t2 = blocks[b2];
            size_t j2 = j % s;
            size_t prefix_min_index = b2 * s + normalized_block_RMQ_table[t2][0][j2];
            if(arr[prefix_min_index] < arr[min_index]) {
                min_index = prefix_min_index;
            }

            if(b1 + 1 <= b2 - 1){
                const size_t interval_length =  b2 - b1 - 1;
                const size_t k = 63 - __builtin_clzll(interval_length);

                const size_t x = block_min_sparse_table[b1+1][k];
                const size_t y = block_min_sparse_table[b2-1 - (1ULL << k) + 1][k];
                size_t mid = (arr[x] <= arr[y]) ? x : y;

                if(arr[mid] < arr[min_index]) {
                    min_index = mid;
                }
            }

            return min_index;
        }

    private:
        size_t s;
        std::vector<size_t> arr;
        std::vector<size_t> blocks;
        std::vector<std::vector<size_t>> block_min_sparse_table;
        std::vector<std::vector<std::vector<size_t>>> normalized_block_RMQ_table;
};

#endif