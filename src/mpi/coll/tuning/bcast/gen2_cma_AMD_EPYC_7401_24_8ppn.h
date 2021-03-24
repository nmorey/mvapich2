#define GEN2_CMA__AMD_EPYC_7401_24__8PPN {	 \
    {    \
    8,    \
    8192,    \
    4,    \
    4,    \
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},    \
    21,    \
    {    \
    {1, &MPIR_Bcast_binomial_MV2},    \
    {2, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {4, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {8, &MPIR_Bcast_scatter_doubling_allgather_MV2},    \
    {16, &MPIR_Bcast_scatter_doubling_allgather_MV2},    \
    {32, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {64, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {128, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {256, &MPIR_Pipelined_Bcast_Zcpy_MV2, 4},    \
    {512, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {1024, &MPIR_Pipelined_Bcast_Zcpy_MV2, 2},    \
    {2048, &MPIR_Bcast_binomial_MV2},    \
    {4096, &MPIR_Bcast_binomial_MV2},    \
    {8192, &MPIR_Pipelined_Bcast_Zcpy_MV2, 2},    \
    {16384, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {32768, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {65536, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {131072, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {262144, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {524288, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    {1048576, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    },    \
    21,    \
    {    \
    {1, &MPIR_Shmem_Bcast_MV2},    \
    {2, &MPIR_Shmem_Bcast_MV2},    \
    {4, &MPIR_Shmem_Bcast_MV2},    \
    {8, &MPIR_Shmem_Bcast_MV2},    \
    {16, &MPIR_Shmem_Bcast_MV2},    \
    {32, &MPIR_Shmem_Bcast_MV2},    \
    {64, &MPIR_Shmem_Bcast_MV2},    \
    {128, &MPIR_Shmem_Bcast_MV2},    \
    {256, &MPIR_Knomial_Bcast_intra_node_MV2, 4},    \
    {512, &MPIR_Shmem_Bcast_MV2},    \
    {1024, &MPIR_Knomial_Bcast_intra_node_MV2, 2},    \
    {2048, &MPIR_Shmem_Bcast_MV2},    \
    {4096, &MPIR_Shmem_Bcast_MV2},    \
    {8192, &MPIR_Knomial_Bcast_intra_node_MV2, 2},    \
    {16384, &MPIR_Shmem_Bcast_MV2},    \
    {32768, &MPIR_Shmem_Bcast_MV2},    \
    {65536, &MPIR_Shmem_Bcast_MV2},    \
    {131072, &MPIR_Shmem_Bcast_MV2},    \
    {262144, &MPIR_Shmem_Bcast_MV2},    \
    {524288, &MPIR_Shmem_Bcast_MV2},    \
    {1048576, &MPIR_Shmem_Bcast_MV2},    \
    }    \
    },    \
    {    \
    16,    \
    8192,    \
    4,    \
    4,    \
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},    \
    21,    \
    {    \
    {1, &MPIR_Pipelined_Bcast_Zcpy_MV2, 4},    \
    {2, &MPIR_Bcast_binomial_MV2},    \
    {4, &MPIR_Bcast_binomial_MV2},    \
    {8, &MPIR_Bcast_binomial_MV2},    \
    {16, &MPIR_Bcast_binomial_MV2},    \
    {32, &MPIR_Bcast_binomial_MV2},    \
    {64, &MPIR_Bcast_binomial_MV2},    \
    {128, &MPIR_Pipelined_Bcast_Zcpy_MV2, 2},    \
    {256, &MPIR_Bcast_binomial_MV2},    \
    {512, &MPIR_Bcast_binomial_MV2},    \
    {1024, &MPIR_Bcast_binomial_MV2},    \
    {2048, &MPIR_Bcast_binomial_MV2},    \
    {4096, &MPIR_Bcast_binomial_MV2},    \
    {8192, &MPIR_Bcast_binomial_MV2},    \
    {16384, &MPIR_Pipelined_Bcast_MV2},    \
    {32768, &MPIR_Pipelined_Bcast_MV2},    \
    {65536, &MPIR_Bcast_binomial_MV2},    \
    {131072, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {262144, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {524288, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {1048576, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    },    \
    21,    \
    {    \
    {1, &MPIR_Knomial_Bcast_intra_node_MV2, 4},    \
    {2, &MPIR_Shmem_Bcast_MV2},    \
    {4, &MPIR_Shmem_Bcast_MV2},    \
    {8, &MPIR_Shmem_Bcast_MV2},    \
    {16, &MPIR_Shmem_Bcast_MV2},    \
    {32, &MPIR_Shmem_Bcast_MV2},    \
    {64, &MPIR_Shmem_Bcast_MV2},    \
    {128, &MPIR_Knomial_Bcast_intra_node_MV2, 2},    \
    {256, &MPIR_Shmem_Bcast_MV2},    \
    {512, &MPIR_Shmem_Bcast_MV2},    \
    {1024, &MPIR_Shmem_Bcast_MV2},    \
    {2048, &MPIR_Shmem_Bcast_MV2},    \
    {4096, &MPIR_Shmem_Bcast_MV2},    \
    {8192, &MPIR_Shmem_Bcast_MV2},    \
    {16384, &MPIR_Shmem_Bcast_MV2},    \
    {32768, &MPIR_Shmem_Bcast_MV2},    \
    {65536, &MPIR_Shmem_Bcast_MV2},    \
    {131072, &MPIR_Shmem_Bcast_MV2},    \
    {262144, &MPIR_Shmem_Bcast_MV2},    \
    {524288, &MPIR_Shmem_Bcast_MV2},    \
    {1048576, &MPIR_Shmem_Bcast_MV2},    \
    }    \
    },    \
    {    \
    32,    \
    8192,    \
    4,    \
    4,    \
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},    \
    21,    \
    {    \
    {1, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {2, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {4, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {8, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {16, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {32, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {64, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {128, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {256, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {512, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {1024, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {2048, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {4096, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {8192, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {16384, &MPIR_Pipelined_Bcast_MV2},    \
    {32768, &MPIR_Pipelined_Bcast_MV2},    \
    {65536, &MPIR_Pipelined_Bcast_MV2},    \
    {131072, &MPIR_Pipelined_Bcast_MV2},    \
    {262144, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {524288, &MPIR_Bcast_scatter_ring_allgather_shm_MV2},    \
    {1048576, &MPIR_Bcast_scatter_ring_allgather_MV2},    \
    },    \
    21,    \
    {    \
    {1, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {2, &MPIR_Shmem_Bcast_MV2},    \
    {4, &MPIR_Shmem_Bcast_MV2},    \
    {8, &MPIR_Shmem_Bcast_MV2},    \
    {16, &MPIR_Shmem_Bcast_MV2},    \
    {32, &MPIR_Shmem_Bcast_MV2},    \
    {64, &MPIR_Shmem_Bcast_MV2},    \
    {128, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {256, &MPIR_Shmem_Bcast_MV2},    \
    {512, &MPIR_Shmem_Bcast_MV2},    \
    {1024, &MPIR_Shmem_Bcast_MV2},    \
    {2048, &MPIR_Shmem_Bcast_MV2},    \
    {4096, &MPIR_Shmem_Bcast_MV2},    \
    {8192, &MPIR_Shmem_Bcast_MV2},    \
    {16384, &MPIR_Shmem_Bcast_MV2},    \
    {32768, &MPIR_Shmem_Bcast_MV2},    \
    {65536, &MPIR_Shmem_Bcast_MV2},    \
    {131072, &MPIR_Shmem_Bcast_MV2},    \
    {262144, &MPIR_Shmem_Bcast_MV2},    \
    {524288, &MPIR_Knomial_Bcast_intra_node_MV2},    \
    {1048576, &MPIR_Shmem_Bcast_MV2},    \
    }    \
    },    \
    {    \
    64,    \
    8192,    \
    4,    \
    4,    \
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},    \
    21,    \
    {    \
    {1, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {2, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {4, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {8, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {16, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {32, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {64, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {128, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {256, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {512, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {1024, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {2048, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {4096, &MPIR_Pipelined_Bcast_Zcpy_MV2, 8},    \
    {8192, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {16384, &MPIR_Pipelined_Bcast_MV2},    \
    {32768, &MPIR_Pipelined_Bcast_MV2},    \
    {65536, &MPIR_Pipelined_Bcast_MV2},    \
    {131072, &MPIR_Pipelined_Bcast_MV2},    \
    {262144, &MPIR_Pipelined_Bcast_MV2},    \
    {524288, &MPIR_Bcast_binomial_MV2},    \
    {1048576, &MPIR_Bcast_scatter_ring_allgather_shm_MV2},    \
    },    \
    21,    \
    {    \
    {1, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {2, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {4, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {8, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {16, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {32, &MPIR_Shmem_Bcast_MV2},    \
    {64, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {128, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {256, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {512, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {1024, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {2048, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {4096, &MPIR_Knomial_Bcast_intra_node_MV2, 8},    \
    {8192, &MPIR_Shmem_Bcast_MV2},    \
    {16384, &MPIR_Shmem_Bcast_MV2},    \
    {32768, &MPIR_Shmem_Bcast_MV2},    \
    {65536, &MPIR_Shmem_Bcast_MV2},    \
    {131072, &MPIR_Shmem_Bcast_MV2},    \
    {262144, &MPIR_Shmem_Bcast_MV2},    \
    {524288, &MPIR_Shmem_Bcast_MV2},    \
    {1048576, &MPIR_Knomial_Bcast_intra_node_MV2},    \
    }    \
    },    \
    {    \
    128,    \
    8192,    \
    4,    \
    4,    \
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},    \
    21,    \
    {    \
    {1, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {2, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {4, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {8, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {16, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {32, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {64, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {128, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {256, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {512, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {1024, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {2048, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {4096, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {8192, &MPIR_Knomial_Bcast_inter_node_wrapper_MV2},    \
    {16384, &MPIR_Pipelined_Bcast_MV2},    \
    {32768, &MPIR_Pipelined_Bcast_MV2},    \
    {65536, &MPIR_Pipelined_Bcast_MV2},    \
    {131072, &MPIR_Pipelined_Bcast_MV2},    \
    {262144, &MPIR_Pipelined_Bcast_MV2},    \
    {524288, &MPIR_Pipelined_Bcast_MV2},    \
    {1048576, &MPIR_Bcast_scatter_ring_allgather_shm_MV2},    \
    },    \
    21,    \
    {    \
    {1, &MPIR_Shmem_Bcast_MV2},    \
    {2, &MPIR_Shmem_Bcast_MV2},    \
    {4, &MPIR_Shmem_Bcast_MV2},    \
    {8, &MPIR_Shmem_Bcast_MV2},    \
    {16, &MPIR_Shmem_Bcast_MV2},    \
    {32, &MPIR_Shmem_Bcast_MV2},    \
    {64, &MPIR_Shmem_Bcast_MV2},    \
    {128, &MPIR_Shmem_Bcast_MV2},    \
    {256, &MPIR_Shmem_Bcast_MV2},    \
    {512, &MPIR_Shmem_Bcast_MV2},    \
    {1024, &MPIR_Shmem_Bcast_MV2},    \
    {2048, &MPIR_Shmem_Bcast_MV2},    \
    {4096, &MPIR_Shmem_Bcast_MV2},    \
    {8192, &MPIR_Shmem_Bcast_MV2},    \
    {16384, &MPIR_Shmem_Bcast_MV2},    \
    {32768, &MPIR_Shmem_Bcast_MV2},    \
    {65536, &MPIR_Shmem_Bcast_MV2},    \
    {131072, &MPIR_Shmem_Bcast_MV2},    \
    {262144, &MPIR_Shmem_Bcast_MV2},    \
    {524288, &MPIR_Shmem_Bcast_MV2},    \
    {1048576, &MPIR_Knomial_Bcast_intra_node_MV2},    \
    }    \
    }    \
}

