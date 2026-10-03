#pragma once

#include "../main.hpp"
#if defined(__APPLE__)


// Matrix dimension (N x N)
const uint32_t N = 4; 

// Raw MSL code embedded into C++ string literal
inline const char* matrix_multiply_kernel = R"(
#include <metal_stdlib>
using namespace metal;
kernel void matrix_multiply_kernel(device const float* matrixC [[buffer(0)]],
                            device const float* matrixA [[buffer(1)]],
                            device float*       matrixB [[buffer(2)]],
                            constant uint&      N       [[buffer(3)]],
                            uint2               pos     [[ thread_position_in_grid ]]) 
    {
        if (pos.x < N && pos.y < N) {
            float sum = 0.0f;
            for (uint k = 0; k < N; ++k) {
                sum += matrixA[pos.y * N + k] * matrixB[k * N + pos.x];
            }
            matrixC[pos.y * N + pos.x] = sum;
        }
    }
)";

inline const char* ray_trace_kernel = R"(
#include <metal_stdlib>
using namespace metal;

// colors
typedef float3 color;
typedef int3 rgb;

// locations/vectors
typedef float4 point;

// Intervals
typedef float2 interval

// matricies
typedef float4x4 mat;

// Ray Type

struct Ray {
    float4 pos;
    float4 dir;
    float time;
};

// Geometric Primitives

struct Triangle {
    float4 v_coords[3];
    float4 t_coords[3];
    // Surface?
};

struct Sphere {
    float center;
    float radius;
    // Surface?
};

template <int N, int M>
struct PrimitiveCollection {
    Triangle t_arr[N];
    Sphere t_arr[M];
};

// I NEED CAMERA CONTROLS 

color color_for(int p_x, int p_y) {

}

kernel void ray_trace_kernel (   
    device const    rgb*                            img             [[ buffer(0) ]],
    constant        uint&                           s_flat          [[ buffer(1) ]], 
    device const    PrimitiveCollection&            prim_list       [[ buffer(2) ]], 
    device const    interval                        shutter_event   [[ buffer(3) ]],
                    uint2                           pos             [[ thread_position_in_grid  ]]
) {
    // TODO                     
}

)";

class ComputeKernel {

    public:

    MTL::Library* library;
    MTL::Function* kernel;
    MTL::ComputePipelineState* pipelineState;
    MTL::CommandQueue* command_queue;

    ComputeKernel(MTL::Device* device, const char* kernel_src, const char* kernel_name) {
        NS::Error* error = nullptr;
        NS::String* sourceStr = NS::String::string(matrix_multiply_kernel, NS::UTF8StringEncoding);
        MTL::CompileOptions* options = nullptr;
        library = device->newLibrary(sourceStr, options, &error);
        
        if (!library) {
            std::cerr << "Shader compile error: " << error->localizedDescription()->utf8String() << std::endl;
            return;
        }

        // Create the pipeline for the computing
        NS::String* funcName = NS::String::string(kernel_name, NS::UTF8StringEncoding);
        kernel = library->newFunction(funcName);
        pipelineState = device->newComputePipelineState(kernel, &error);
    }

    ~ComputeKernel() {
        if (command_queue) command_queue->release();
        if (pipelineState) pipelineState->release();
        if (kernel) kernel->release();
        if (library) library->release();
    }


    void call(int num_i, MTL::Buffer* inputs[]) {
        MTL::CommandBuffer* commandBuffer = command_queue->commandBuffer();
        MTL::ComputeCommandEncoder* encoder = commandBuffer->computeCommandEncoder();

        encoder->setComputePipelineState(pipelineState);
        for (int i = 0; i < num_i; i++) {
            encoder->setBuffer(inputs[i], 0, i);
        }

        // Define Thread Execution Grid Dimensions
        // Threadgroup size configuration optimizing for GPU SIMD widths (typically multiples of 16 or 32)
        MTL::Size threadGroupSize = MTL::Size(16, 16, 1);
        MTL::Size gridSize = MTL::Size(N, N, 1);

        // Assign threads
        encoder->dispatchThreads(gridSize, threadGroupSize);
        encoder->endEncoding();

        // Execute and wait
        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();
    }


};


class MetalInstance {

    public:

    NS::AutoreleasePool* pool;
    MTL::Device* device;

    MetalInstance() {
        NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
        MTL::Device* device = MTL::CreateSystemDefaultDevice();
    }

    ~MetalInstance() {
        if (device) device->release();
        if (pool) pool->release();
    }

};


inline void gpu_test() {
    MetalInstance m = MetalInstance();
    ComputeKernel mm_kernel = ComputeKernel(m.device, matrix_multiply_kernel, "matrix multiply");
    ComputeKernel rt_kernel = ComputeKernel(m.device, ray_trace_kernel, "ray_trace");


    // 3. Prepare Host Data (Matrices A and B)
    size_t matrixSize = N * N * sizeof(float);
    std::vector<float> hostA(N * N, 2.0f); // Filled with 2.0
    std::vector<float> hostB(N * N, 3.0f); // Filled with 3.0

    // Create input buffers
    MTL::Buffer* arr[4] = {
        m.device->newBuffer(matrixSize, MTL::ResourceStorageModeShared), 
        m.device->newBuffer(hostA.data(), matrixSize, MTL::ResourceStorageModeShared), 
        m.device->newBuffer(hostB.data(), matrixSize, MTL::ResourceStorageModeShared), 
        m.device->newBuffer(&N, sizeof(uint32_t), MTL::ResourceStorageModeShared)
    };
    mm_kernel.call(4, arr);

    // Read and Print Output Matrix Results
    float* result = static_cast<float*>(arr[0]->contents());
    std::cout << "Result Matrix C (" << N << "x" << N << "):\n";
    for (uint32_t i = 0; i < N; ++i) {
        for (uint32_t j = 0; j < N; ++j) {
            std::cout << result[i * N + j] << " ";
        }
        std::cout << "\n";
    }

    // Release input buffers
    for (int i = 0; i < 4; i++) {
        arr[i]->release();
    }
}
#endif