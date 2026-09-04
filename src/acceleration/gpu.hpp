#pragma once

#include "../main.hpp"
#if defined(__APPLE__)
/**
 * I can use the gpu for
 * 
 *      - ray tracing for a porion of the screen
 *      - debth buffer (mat_multiply) for a portion of the screen
 * 
 */

// Matrix dimension (N x N)
const uint32_t N = 4; 

// Raw MSL code embedded into C++ string literal
const char* shaderSource = R"(
#include <metal_stdlib>
using namespace metal;
kernel void matrix_multiply(device const float* matrixA [[buffer(0)]],
                            device const float* matrixB [[buffer(1)]],
                            device float*       matrixC [[buffer(2)]],
                            constant uint&      N       [[buffer(3)]]) {
    if (pos.x < N && pos.y < N) {
        float sum = 0.0f;
        for (uint k = 0; k < N; ++k) {
            sum += matrixA[pos.y * N + k] * matrixB[k * N + pos.x];
        }
        matrixC[pos.y * N + pos.x] = sum;
    }
}
)";


void r() {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    // 1. Initialize Device and Command Queue
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    MTL::CommandQueue* commandQueue = device->newCommandQueue();

    // 2. Compile MSL Shader Program
    NS::Error* error = nullptr;
    NS::String* sourceStr = NS::String::string(shaderSource, NS::UTF8StringEncoding);
    MTL::CompileOptions* options = nullptr;
    MTL::Library* library = device->newLibrary(sourceStr, options, &error);
    
    if (!library) {
        std::cerr << "Shader compile error: " << error->localizedDescription()->utf8String() << std::endl;
        return;
    }

    NS::String* funcName = NS::String::string("matrix_multiply", NS::UTF8StringEncoding);
    MTL::Function* kernelFunc = library->newFunction(funcName);
    MTL::ComputePipelineState* pipelineState = device->newComputePipelineState(kernelFunc, &error);

    // 3. Prepare Host Data (Matrices A and B)
    size_t matrixSize = N * N * sizeof(float);
    std::vector<float> hostA(N * N, 2.0f); // Filled with 2.0
    std::vector<float> hostB(N * N, 3.0f); // Filled with 3.0

    // 4. Create Shared GPU Buffers
    MTL::Buffer* bufferA = device->newBuffer(hostA.data(), matrixSize, MTL::ResourceStorageModeShared);
    MTL::Buffer* bufferB = device->newBuffer(hostB.data(), matrixSize, MTL::ResourceStorageModeShared);
    MTL::Buffer* bufferC = device->newBuffer(matrixSize, MTL::ResourceStorageModeShared);
    MTL::Buffer* bufferN = device->newBuffer(&N, sizeof(uint32_t), MTL::ResourceStorageModeShared);

    // 5. Create Command Buffer and Compute Encoder
    MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
    MTL::ComputeCommandEncoder* encoder = commandBuffer->computeCommandEncoder();

    encoder->setComputePipelineState(pipelineState);
    encoder->setBuffer(bufferA, 0, 0);
    encoder->setBuffer(bufferB, 0, 1);
    encoder->setBuffer(bufferC, 0, 2);
    encoder->setBuffer(bufferN, 0, 3);

    // 6. Define Thread Execution Grid Dimensions
    // Threadgroup size configuration optimizing for GPU SIMD widths (typically multiples of 16 or 32)
    MTL::Size threadGroupSize = MTL::Size(16, 16, 1);
    MTL::Size gridSize = MTL::Size(N, N, 1);

    encoder->dispatchThreads(gridSize, threadGroupSize);
    encoder->endEncoding();

    // 7. Execute Tasks and Wait for GPU
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    // 8. Read and Print Output Matrix Results
    float* result = static_cast<float*>(bufferC->contents());
    std::cout << "Result Matrix C (" << N << "x" << N << "):\n";
    for (uint32_t i = 0; i < N; ++i) {
        for (uint32_t j = 0; j < N; ++j) {
            std::cout << result[i * N + j] << " ";
        }
        std::cout << "\n";
    }

    // 9. Explicit Manual Reference Clean Up
    bufferA->release();
    bufferB->release();
    bufferC->release();
    bufferN->release();
    pipelineState->release();
    kernelFunc->release();
    library->release();
    commandQueue->release();
    device->release();
    pool->release();
}
#endif