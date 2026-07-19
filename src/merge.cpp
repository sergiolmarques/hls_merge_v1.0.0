/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

/*******************************************************************************
Description:

    This example uses the load/compute/store coding style which is generally
    the most efficient for implementing kernels using HLS. The load and store
    functions are responsible for moving data in and out of the kernel as
    efficiently as possible. The core functionality is decomposed across one
    of more compute functions. Whenever possible, the compute function should
    pass data through HLS streams and should contain a single set of nested loops.

    HLS stream objects are used to pass data between producer and consumer
    functions. Stream read and write operations have a blocking behavior which
    allows consumers and producers to synchronize with each other automatically.

    The dataflow pragma instructs the compiler to enable task-level pipelining.
    This is required for to load/compute/store functions to execute in a parallel
    and pipelined manner.

    The kernel operates on vectors of NUM_WORDS integers modeled using the hls::vector
    data type. This datatype provides intuitive support for parallelism and
    fits well the vector-add computation. The vector length is set to NUM_WORDS
    since NUM_WORDS integers amount to a total of 64 bytes, which is the maximum size of
    a kernel port. It is a good practice to match the compute bandwidth to the I/O
    bandwidth. Here the kernel loads, computes and stores NUM_WORDS integer values per
    clock cycle and is implemented as below:
                                       _____________
                                      |             |<----- Input Vector 1 from Global Memory
                                      |  load_input |       __
                                      |_____________|----->|  |
                                       _____________       |  | in1_stream
Input Vector 2 from Global Memory --->|             |      |__|
                               __     |  load_input |        |
                              |  |<---|_____________|        |
                   in2_stream |  |     _____________         |
                              |__|--->|             |<--------
                                      | compute_add |      __
                                      |_____________|---->|  |
                                       ______________     |  | out_stream
                                      |              |<---|__|
                                      | store_result |
                                      |______________|-----> Output result to Global Memory

*******************************************************************************/

// Includes
#include <hls_vector.h>
#include <hls_stream.h>
#include "assert.h"

#define MEMORY_DWIDTH 512
#define SIZEOF_WORD 4
#define NUM_WORDS ((MEMORY_DWIDTH) / (8 * SIZEOF_WORD))

#define DATA_SIZE 4096

// TRIPCOUNT identifier
const int c_size = DATA_SIZE;

static void load_input(hls::vector<uint32_t, NUM_WORDS>* in,
                       hls::stream<hls::vector<uint32_t, NUM_WORDS> >& inStream,
                       int vSize) {
mem_rd:
    for (int i = 0; i < vSize; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        inStream << in[i];
    }
}

static void compute_merge(
    hls::stream<hls::vector<uint32_t, NUM_WORDS> >& in1_stream,
    hls::stream<hls::vector<uint32_t, NUM_WORDS> >& in2_stream,
    hls::stream<hls::vector<uint32_t, NUM_WORDS> >& out_stream,
    int vSize, 
    int vSize2)
{
    const int total1 = vSize  * NUM_WORDS;   // total scalar elements in stream 1
    const int total2 = vSize2 * NUM_WORDS;   // total scalar elements in stream 2
					     
    hls::vector<uint32_t, NUM_WORDS> inVec1, inVec2;
    hls::vector<uint32_t, NUM_WORDS> outVec(0);

    int idx1 = 0, idx2 = 0;     // position within currently-loaded input vector
    int outIdx = 0;             // position within the output vector being built
    int count1 = 0, count2 = 0; // scalar elements consumed so far from each stream

    // Preload first beat from each stream (only if that stream actually has data)
    if (total1 > 0) inVec1 = in1_stream.read();
    if (total2 > 0) inVec2 = in2_stream.read();

merge_loop:
    while (count1 < total1 && count2 < total2) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=1 max=4096

        uint32_t val1 = inVec1[idx1];
        uint32_t val2 = inVec2[idx2];

        if (val1 <= val2) {
            outVec[outIdx++] = val1;
            idx1++; count1++;
            if (idx1 == NUM_WORDS && count1 < total1) {
                inVec1 = in1_stream.read();
                idx1 = 0;
            }
        } else {
            outVec[outIdx++] = val2;
            idx2++; count2++;
            if (idx2 == NUM_WORDS && count2 < total2) {
                inVec2 = in2_stream.read();
                idx2 = 0;
            }
        }

        if (outIdx == NUM_WORDS) {
            out_stream.write(outVec);
            outIdx = 0;
        }
    }

    // Drain any remainder from stream 1
drain1_loop:
    while (count1 < total1) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=0 max=4096
        outVec[outIdx++] = inVec1[idx1++];
        count1++;
        if (outIdx == NUM_WORDS) { out_stream.write(outVec); outIdx = 0; }
        if (idx1 == NUM_WORDS && count1 < total1) { inVec1 = in1_stream.read(); idx1 = 0; }
    }

    // Drain any remainder from stream 2
drain2_loop:
    while (count2 < total2) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=0 max=4096
        outVec[outIdx++] = inVec2[idx2++];
        count2++;
        if (outIdx == NUM_WORDS) { out_stream.write(outVec); outIdx = 0; }
        if (idx2 == NUM_WORDS && count2 < total2) { inVec2 = in2_stream.read(); idx2 = 0; }
    }

    // Flush a final partial output vector, padding with zeros
    if (outIdx != 0) {
        for (int k = outIdx; k < NUM_WORDS; k++) {
#pragma HLS UNROLL
            outVec[k] = 0;
        }
        out_stream.write(outVec);
    }

//#ifndef __SYNTHESIS__
    // Debug aid: leave this in during co-sim. For your test case this MUST
    // print outIdx=0 — if it doesn't, vSize/vSize2 don't match what the
    // testbench actually sent into the streams.
//    printf("compute_merge done: total1=%d total2=%d final_outIdx=%d\n",
//           total1, total2, outIdx);
//#endif
}

static void store_result(hls::vector<uint32_t, NUM_WORDS>* out,
                         hls::stream<hls::vector<uint32_t, NUM_WORDS> >& out_stream,
                         int vSize) {
mem_wr:
    for (int i = 0; i < vSize; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        out[i] = out_stream.read();
    }
}

extern "C" {

void merge(hls::vector<uint32_t, NUM_WORDS>* in1,
          hls::vector<uint32_t, NUM_WORDS>* in2,
          hls::vector<uint32_t, NUM_WORDS>* out,
          int size,
          int size2) {
#pragma HLS INTERFACE m_axi port = in1 bundle = gmem0
#pragma HLS INTERFACE m_axi port = in2 bundle = gmem1
#pragma HLS INTERFACE m_axi port = out bundle = gmem0

    static hls::stream<hls::vector<uint32_t, NUM_WORDS> > in1_stream("input_stream_1");
    static hls::stream<hls::vector<uint32_t, NUM_WORDS> > in2_stream("input_stream_2");
    static hls::stream<hls::vector<uint32_t, NUM_WORDS> > out_stream("output_stream");

    // Since NUM_WORDS values are processed
    // in parallel per loop iteration, the for loop only needs to iterate 'size / NUM_WORDS' times.
    //assert(size % NUM_WORDS == 0);
    //assert(size2 % NUM_WORDS == 0);
    int vSize = size / NUM_WORDS;
    int vSize2 = size2 / NUM_WORDS;
    
    int vSizeOut = (vSize + vSize2);
#pragma HLS dataflow

    load_input(in1, in1_stream, vSize);
    load_input(in2, in2_stream, vSize2);
    compute_merge(in1_stream, in2_stream, out_stream, vSize, vSize2);
    store_result(out, out_stream, vSizeOut);
}
}
