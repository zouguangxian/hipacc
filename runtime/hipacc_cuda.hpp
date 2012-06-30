//
// Copyright (c) 2012, University of Erlangen-Nuremberg
// Copyright (c) 2012, Siemens AG
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
// 
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef __HIPACC_CUDA_HPP__
#define __HIPACC_CUDA_HPP__

#include <cuda.h>

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#define HIPACC_NUM_ITERATIONS 10

static float total_time = 0.0f;
static float last_gpu_timing = 0.0f;

enum hipaccBoundaryMode {
    BOUNDARY_UNDEFINED,
    BOUNDARY_CLAMP,
    BOUNDARY_REPEAT,
    BOUNDARY_MIRROR,
    BOUNDARY_CONSTANT
};

typedef struct hipacc_const_info {
    hipacc_const_info(std::string name, void *memory, int size) :
        name(name), memory(memory), size(size) {}
    std::string name;
    void *memory;
    int size;
} hipacc_const_info;

typedef struct hipacc_tex_info {
    hipacc_tex_info(std::string name, CUarray_format type, void *image) :
        name(name), type(type), image(image) {}
    std::string name;
    CUarray_format type;
    void *image;
} hipacc_tex_info;

typedef struct hipacc_smem_info {
    hipacc_smem_info(int size_x, int size_y, int pixel_size) :
        size_x(size_x), size_y(size_y), pixel_size(pixel_size) {}
    int size_x, size_y;
    int pixel_size;
} hipacc_smem_info;


const char *getCUDAErrorCodeStr(int errorCode) {
    switch (errorCode) {
        case CUDA_SUCCESS:
            return "CUDA_SUCCESS";
        case CUDA_ERROR_INVALID_VALUE:
            return "CUDA_ERROR_INVALID_VALUE";
        case CUDA_ERROR_OUT_OF_MEMORY:
            return "CUDA_ERROR_OUT_OF_MEMORY";
        case CUDA_ERROR_NOT_INITIALIZED:
            return "CUDA_ERROR_NOT_INITIALIZED";
        case CUDA_ERROR_DEINITIALIZED:
            return "CUDA_ERROR_DEINITIALIZED";
        case CUDA_ERROR_PROFILER_DISABLED:
            return "CUDA_ERROR_PROFILER_DISABLED";
        case CUDA_ERROR_PROFILER_NOT_INITIALIZED:
            return "CUDA_ERROR_PROFILER_NOT_INITIALIZED";
        case CUDA_ERROR_PROFILER_ALREADY_STARTED:
            return "CUDA_ERROR_PROFILER_ALREADY_STARTED";
        case CUDA_ERROR_PROFILER_ALREADY_STOPPED:
            return "CUDA_ERROR_PROFILER_ALREADY_STOPPED";
        case CUDA_ERROR_NO_DEVICE:
            return "CUDA_ERROR_NO_DEVICE";
        case CUDA_ERROR_INVALID_DEVICE:
            return "CUDA_ERROR_INVALID_DEVICE";
        case CUDA_ERROR_INVALID_IMAGE:
            return "CUDA_ERROR_INVALID_IMAGE";
        case CUDA_ERROR_INVALID_CONTEXT:
            return "CUDA_ERROR_INVALID_CONTEXT";
        case CUDA_ERROR_CONTEXT_ALREADY_CURRENT:
            return "CUDA_ERROR_CONTEXT_ALREADY_CURRENT";
        case CUDA_ERROR_MAP_FAILED:
            return "CUDA_ERROR_MAP_FAILED";
        case CUDA_ERROR_UNMAP_FAILED:
            return "CUDA_ERROR_UNMAP_FAILED";
        case CUDA_ERROR_ARRAY_IS_MAPPED:
            return "CUDA_ERROR_ARRAY_IS_MAPPED";
        case CUDA_ERROR_ALREADY_MAPPED:
            return "CUDA_ERROR_ALREADY_MAPPED";
        case CUDA_ERROR_NO_BINARY_FOR_GPU:
            return "CUDA_ERROR_NO_BINARY_FOR_GPU";
        case CUDA_ERROR_ALREADY_ACQUIRED:
            return "CUDA_ERROR_ALREADY_ACQUIRED";
        case CUDA_ERROR_NOT_MAPPED:
            return "CUDA_ERROR_NOT_MAPPED";
        case CUDA_ERROR_NOT_MAPPED_AS_ARRAY:
            return "CUDA_ERROR_NOT_MAPPED_AS_ARRAY";
        case CUDA_ERROR_NOT_MAPPED_AS_POINTER:
            return "CUDA_ERROR_NOT_MAPPED_AS_POINTER";
        case CUDA_ERROR_ECC_UNCORRECTABLE:
            return "CUDA_ERROR_ECC_UNCORRECTABLE";
        case CUDA_ERROR_UNSUPPORTED_LIMIT:
            return "CUDA_ERROR_UNSUPPORTED_LIMIT";
        case CUDA_ERROR_CONTEXT_ALREADY_IN_USE:
            return "CUDA_ERROR_CONTEXT_ALREADY_IN_USE";
        case CUDA_ERROR_INVALID_SOURCE:
            return "CUDA_ERROR_INVALID_SOURCE";
        case CUDA_ERROR_FILE_NOT_FOUND:
            return "CUDA_ERROR_FILE_NOT_FOUND";
        case CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND:
            return "CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND";
        case CUDA_ERROR_SHARED_OBJECT_INIT_FAILED:
            return "CUDA_ERROR_SHARED_OBJECT_INIT_FAILED";
        case CUDA_ERROR_OPERATING_SYSTEM:
            return "CUDA_ERROR_OPERATING_SYSTEM";
        case CUDA_ERROR_INVALID_HANDLE:
            return "CUDA_ERROR_INVALID_HANDLE";
        case CUDA_ERROR_NOT_FOUND:
            return "CUDA_ERROR_NOT_FOUND";
        case CUDA_ERROR_NOT_READY:
            return "CUDA_ERROR_NOT_READY";
        case CUDA_ERROR_LAUNCH_FAILED:
            return "CUDA_ERROR_LAUNCH_FAILED";
        case CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES:
            return "CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES";
        case CUDA_ERROR_LAUNCH_TIMEOUT:
            return "CUDA_ERROR_LAUNCH_TIMEOUT";
        case CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING:
            return "CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING";
        case CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED:
            return "CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED";
        case CUDA_ERROR_PEER_ACCESS_NOT_ENABLED:
            return "CUDA_ERROR_PEER_ACCESS_NOT_ENABLED";
        case CUDA_ERROR_PRIMARY_CONTEXT_ACTIVE:
            return "CUDA_ERROR_PRIMARY_CONTEXT_ACTIVE";
        case CUDA_ERROR_CONTEXT_IS_DESTROYED:
            return "CUDA_ERROR_CONTEXT_IS_DESTROYED";
        case CUDA_ERROR_ASSERT:
            return "CUDA_ERROR_ASSERT";
        case CUDA_ERROR_TOO_MANY_PEERS:
            return "CUDA_ERROR_TOO_MANY_PEERS";
        case CUDA_ERROR_HOST_MEMORY_ALREADY_REGISTERED:
            return "CUDA_ERROR_HOST_MEMORY_ALREADY_REGISTERED";
        case CUDA_ERROR_HOST_MEMORY_NOT_REGISTERED:
            return "CUDA_ERROR_HOST_MEMORY_NOT_REGISTERED";
        case CUDA_ERROR_UNKNOWN:
            return "CUDA_ERROR_UNKNOWN";
        default:
            return "unknown error code";
    }
}
// Macro for error checking device driver
#if 1
#define checkErrDrv(err, name) \
    if (err != CUDA_SUCCESS) { \
        std::cerr << "ERROR: " << name << " (" << (err) << ")" << " [file " << __FILE__ << ", line " << __LINE__ << "]: "; \
        std::cerr << getCUDAErrorCodeStr(err) << std::endl; \
        exit(EXIT_FAILURE); \
    }
#else
inline void checkErrDrv(CUresult err, const char *name) {
    if (err != CUDA_SUCCESS) { \
        std::cerr << "ERROR: " << name << " (" << (err) << "): "; \
        std::cerr << getCUDAErrorCodeStr(err) << std::endl; \
        exit(EXIT_FAILURE); \
    }
}
#endif
// Macro for error checking
#if 1
#define checkErr(err, name) \
    if (err != cudaSuccess) { \
        std::cerr << "ERROR: " << name << " (" << (err) << ")" << " [file " << __FILE__ << ", line " << __LINE__ << "]: "; \
        std::cerr << cudaGetErrorString(err) << std::endl; \
        exit(EXIT_FAILURE); \
    }
#else
inline void checkErr(cudaError_t err, const char *name) {
    if (err != cudaSuccess) {
        std::cerr << "ERROR: " << name << " (" << err << "): ";
        std::cerr << cudaGetErrorString(err) << std::endl;
        exit(EXIT_FAILURE);
    }
}
#endif


class HipaccContext {
    public:
        typedef struct {
            int width;
            int height;
            int stride;
            int alignment;
            int pixel_size;
        } cl_dims;

    private:
        std::vector<std::pair<void *, cl_dims> > mems;

        HipaccContext() {};
        HipaccContext(HipaccContext const &);
        void operator=(HipaccContext const &);

    public:
        static HipaccContext &getInstance() {
            static HipaccContext instance;

            return instance;
        }
        void add_memory(void *id, cl_dims dim) { mems.push_back(std::make_pair(id, dim)); }
        void del_memory(void *id) {
            unsigned int num=0;
            std::vector<std::pair<void *, cl_dims> >::const_iterator i;
            for (i=mems.begin(); i!=mems.end(); ++i, ++num) {
                if (i->first == id) {
                    mems.erase(mems.begin() + num);
                    return;
                }
            }

            std::cerr << "ERROR: Unknown GPU memory requested: " << id << std::endl;
            exit(EXIT_FAILURE);
        }
        cl_dims get_mem_dims(void *id) {
            std::vector<std::pair<void *, cl_dims> >::const_iterator i;
            for (i=mems.begin(); i!=mems.end(); ++i) {
                if (i->first == id) return i->second;
            }

            std::cerr << "ERROR: Unknown GPU memory object requested: " << id << std::endl;
            exit(EXIT_FAILURE);
        }
};


// Get GPU timing of last executed Kernel in ms
float hipaccGetLastKernelTiming() {
    return last_gpu_timing;
}


// Initialize CUDA devices
void hipaccInitCUDA() {
    setenv("CUDA_CACHE_DISABLE", "1", 1);
    HipaccContext &Ctx = HipaccContext::getInstance();
}


// Allocate memory with alignment specified
template<typename T>
T *hipaccCreateMemory(T *host_mem, int width, int height, int *stride, int alignment) {
    cudaError_t err = cudaSuccess;
    T *mem;
    HipaccContext &Ctx = HipaccContext::getInstance();

    *stride = (int)ceil((float)(width)/(alignment/sizeof(T))) * (alignment/sizeof(T));
    err = cudaMalloc((void **) &mem, sizeof(T)*(*stride)*height);
    //err = cudaMallocPitch((void **) &mem, stride, *stride*sizeof(float), height);
    checkErr(err, "cudaMalloc()");

    HipaccContext::cl_dims dim = { width, height, *stride, alignment, sizeof(T) };
    Ctx.add_memory(mem, dim);

    return mem;
}


// Allocate memory without any alignment considerations
template<typename T>
T *hipaccCreateMemory(T *host_mem, int width, int height, int *stride) {
    cudaError_t err = cudaSuccess;
    T *mem;
    HipaccContext &Ctx = HipaccContext::getInstance();

    *stride = width;
    err = cudaMalloc((void **) &mem, sizeof(T)*width*height);
    checkErr(err, "cudaMalloc()");

    HipaccContext::cl_dims dim = { width, height, width, 0, sizeof(T) };
    Ctx.add_memory(mem, dim);

    return mem;
}


// Release memory
void hipaccReleaseMemory(void *mem) {
    cudaError_t err = cudaSuccess;
    HipaccContext &Ctx = HipaccContext::getInstance();

    err = cudaFree(mem);
    checkErr(err, "cudaFree()");

    Ctx.del_memory(mem);
}


// Write to memory
template<typename T>
void hipaccWriteMemory(T *mem, T *host_mem) {
    cudaError_t err = cudaSuccess;
    HipaccContext &Ctx = HipaccContext::getInstance();
    HipaccContext::cl_dims dim = Ctx.get_mem_dims(mem);

    int width = dim.width;
    int height = dim.height;
    int stride = dim.stride;

    if (stride > width) {
        err = cudaMemcpy2D(mem, stride*sizeof(T), host_mem, width*sizeof(T), width*sizeof(T), height, cudaMemcpyHostToDevice);
        checkErr(err, "cudaMemcpy2D()");
    } else {
        err = cudaMemcpy(mem, host_mem, sizeof(T)*width*height, cudaMemcpyHostToDevice);
        checkErr(err, "cudaMemcpy()");
    }
}


// Read from memory
template<typename T>
void hipaccReadMemory(T *host_mem, T *mem) {
    cudaError_t err = cudaSuccess;
    HipaccContext &Ctx = HipaccContext::getInstance();
    HipaccContext::cl_dims dim = Ctx.get_mem_dims(mem);

    int width = dim.width;
    int height = dim.height;
    int stride = dim.stride;

    if (stride > width) {
        err = cudaMemcpy2D(host_mem, width*sizeof(T), mem, stride*sizeof(T), width*sizeof(T), height, cudaMemcpyDeviceToHost);
        checkErr(err, "cudaMemcpy2D()");
    } else {
        err = cudaMemcpy(host_mem, mem, sizeof(T)*width*height, cudaMemcpyDeviceToHost);
        checkErr(err, "cudaMemcpy()");
    }
}


// Bind linear memory to texture
template<typename T>
void hipaccBindTexture(const struct texture<T, cudaTextureType1D, cudaReadModeElementType> &texture, void *mem) {
    cudaError_t err = cudaSuccess;
    HipaccContext &Ctx = HipaccContext::getInstance();
    HipaccContext::cl_dims dim = Ctx.get_mem_dims(mem);

    err = cudaBindTexture(NULL, texture, mem, sizeof(T)*dim.stride*dim.height);
    checkErr(err, "cudaBindTexture()");
}


// Bind linear memory to 2D texture
template<typename T>
void hipaccBindTexture2D(const struct texture<T, cudaTextureType2D, cudaReadModeElementType> &texture, void *mem) {
    cudaError_t err = cudaSuccess;
    HipaccContext &Ctx = HipaccContext::getInstance();
    HipaccContext::cl_dims dim = Ctx.get_mem_dims(mem);

    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<T>();

    err = cudaBindTexture2D(NULL, texture, mem, channelDesc, dim.width, dim.height, dim.stride*sizeof(T));
    checkErr(err, "cudaBindTexture2D()");
}


// Unbind texture
template<typename T, int ND>
void hipaccUnbindTexture(const struct texture<T, ND, cudaReadModeElementType>texture) {
    cudaError_t err = cudaSuccess;

    err = cudaUnbindTexture(texture);
    checkErr(err, "cudaUnbindTexture()");
}


// Write to symbol
template<typename T>
void hipaccWriteSymbol(const char *symbol, T *host_mem, int width, int height) {
    cudaError_t err = cudaSuccess;
    HipaccContext &Ctx = HipaccContext::getInstance();

    err = cudaMemcpyToSymbol(symbol, host_mem, sizeof(T)*width*height);
    checkErr(err, "cudaMemcpyToSymbol()");
}


// Read from symbol
template<typename T>
void hipaccReadSymbol(T *host_mem, const char *symbol, int width, int height) {
    cudaError_t err = cudaSuccess;
    HipaccContext &Ctx = HipaccContext::getInstance();

    err = cudaMemcpyFromSymbol(host_mem, symbol, sizeof(T)*width*height);
    checkErr(err, "cudaMemcpyFromSymbol()");
}


// Set the configuration for a kernel
void hipaccConfigureCall(dim3 grid, dim3 block) {
    cudaError_t err = cudaSuccess;

    err = cudaConfigureCall(grid, block, 0, 0);
    checkErr(err, "cudaConfigureCall()");
}


// Set a single argument of a kernel
void hipaccSetupArgument(const void *arg, size_t size, size_t &offset) {
    cudaError_t err = cudaSuccess;

    // GPU data has to be accessed aligned
    if (offset % size) {
        offset += offset % size;
    }
    err = cudaSetupArgument(arg, size, offset);
    offset += size;
    checkErr(err, "clSetKernelArg()");
}


// Launch kernel
void hipaccLaunchKernel(const char *kernel, dim3 grid, dim3 block, bool print_timing=true) {
    cudaError_t err = cudaSuccess;
    cudaEvent_t start, end;
    HipaccContext &Ctx = HipaccContext::getInstance();
    float time;
    std::string error_string = "cudaLaunch(";
    error_string += kernel;
    error_string += ")";

    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaEventRecord(start, 0);

    err = cudaLaunch(kernel);
    checkErr(err, error_string);

    cudaThreadSynchronize();
    err = cudaGetLastError();
    checkErr(err, error_string);

    cudaEventRecord(end, 0);
    cudaEventSynchronize(end);
    cudaEventElapsedTime(&time, start, end);

    last_gpu_timing = time;
    if (print_timing) {
        std::cerr << "<HIPACC:> Kernel timing ("<< block.x*block.y << ": " << block.x << "x" << block.y << "): " << time << "(ms)" << std::endl;
    }
}


// Benchmark timing for a kernel call
void hipaccLaunchKernelBenchmark(const char *kernel, std::vector<std::pair<size_t, void *> > args, dim3 grid, dim3 block, bool print_timing=true) {
    float min_dt=FLT_MAX;

    for (int i=0; i<HIPACC_NUM_ITERATIONS; i++) {
        // setup call
        hipaccConfigureCall(grid, block);

        // set kernel arguments
        size_t offset = 0;
        for (unsigned int i=0; i<args.size(); i++) {
            hipaccSetupArgument(args.data()[i].second, args.data()[i].first, offset);
        }

        // launch kernel
        hipaccLaunchKernel(kernel, grid, block, print_timing);
        if (last_gpu_timing < min_dt) min_dt = last_gpu_timing;
    }

    last_gpu_timing = min_dt;
    if (print_timing) {
        std::cerr << "<HIPACC:> Kernel timing benchmark ("<< block.x*block.y << ": " << block.x << "x" << block.y << "): " << min_dt << "(ms)" << std::endl;
    }
}


//
// DRIVER API
//

// Compile CUDA source file to ptx assembly using nvcc compiler
void hipaccCompileCUDAToPTX(std::string file_name, int cc, const char *build_options=(const char *)"") {
    char line[FILENAME_MAX];
    FILE *fpipe;

    std::string command = "nvcc -ptx ";

    switch (cc) {
        default:
            std::cerr << "ERROR: Specified compute capability '" << cc << "' is not supported!" << std::endl;
            exit(EXIT_FAILURE);
            break;
        case 10:
            command += "-gencode=arch=compute_10,code=\\\"sm_10,compute_10\\\" ";
            break;
        case 11:
            command += "-gencode=arch=compute_11,code=\\\"sm_11,compute_11\\\" ";
            break;
        case 12:
            command += "-gencode=arch=compute_12,code=\\\"sm_12,compute_12\\\" ";
            break;
        case 13:
            command += "-gencode=arch=compute_13,code=\\\"sm_13,compute_13\\\" ";
            break;
        case 20:
            command += "-gencode=arch=compute_20,code=\\\"sm_20,compute_20\\\" ";
            break;
        case 21:
            command += "-gencode=arch=compute_21,code=\\\"sm_21,compute_21\\\" ";
            break;
    }
    command += "-ftz=true -prec-sqrt=false -prec-div=false ";
    command += build_options;
    command += " " + file_name;
    command += " -o " + file_name + ".ptx 2>&1";

    if (!(fpipe = (FILE *)popen(command.c_str(), "r"))) {
        perror("Problems with pipe");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(char) * FILENAME_MAX, fpipe)) {
        std::cerr << line;
    }
    pclose(fpipe);
}


// Load ptx assembly, create a module and kernel
void hipaccCreateModuleKernel(CUfunction *result_function, CUmodule *result_module, std::string file_name, std::string kernel_name, int cc) {
    CUresult err = CUDA_SUCCESS;
    HipaccContext &Ctx = HipaccContext::getInstance();
    CUmodule module;
    CUfunction function;
    CUjit_target_enum target_cc;

    switch (cc) {
        default:
            std::cerr << "ERROR: Specified compute capability '" << cc << "' is not supported!" << std::endl;
            exit(EXIT_FAILURE);
            break;
        case 10:
            target_cc = CU_TARGET_COMPUTE_10;
            break;
        case 11:
            target_cc = CU_TARGET_COMPUTE_11;
            break;
        case 12:
            target_cc = CU_TARGET_COMPUTE_12;
            break;
        case 13:
            target_cc = CU_TARGET_COMPUTE_13;
            break;
        case 20:
            target_cc = CU_TARGET_COMPUTE_20;
            break;
        case 21:
            target_cc = CU_TARGET_COMPUTE_21;
            break;
    }


    std::ifstream srcFile(file_name.c_str());
    if (!srcFile.is_open()) {
        std::cerr << "ERROR: Can't open PTX source file '" << file_name.c_str() << "'!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string ptxString(std::istreambuf_iterator<char>(srcFile),
            (std::istreambuf_iterator<char>()));

    const size_t length = ptxString.length();
    const char *c_str = ptxString.c_str();

    const int errorLogSize = 10240;
    char errorLogBuffer[errorLogSize] = {0};

    CUjit_option options[] = { CU_JIT_ERROR_LOG_BUFFER, CU_JIT_ERROR_LOG_BUFFER_SIZE_BYTES, CU_JIT_TARGET };
    void *optionValues[] = { (void *)errorLogBuffer, (void *)errorLogSize, (void *)target_cc };

    // Load ptx source
    err = cuModuleLoadDataEx(&module, c_str, 2, options, optionValues);
    if (err != CUDA_SUCCESS) {
        std::cerr << "Error log: " << errorLogBuffer << std::endl;
    }
    checkErrDrv(err, "cuModuleLoadDataEx()");

    // Get function entry point
    err = cuModuleGetFunction(&function, module, kernel_name.c_str());
    checkErrDrv(err, "cuModuleGetFunction()");

    *result_function = function;
    *result_module = module;
}


// Launch kernel
void hipaccLaunchKernel(CUfunction &kernel, const char *kernel_name, dim3 grid, dim3 block, void **args, bool print_timing=true) {
    CUresult err = CUDA_SUCCESS;
    cudaEvent_t start, end;
    HipaccContext &Ctx = HipaccContext::getInstance();
    float time;
    std::string error_string = "cuLaunchKernel(";
    error_string += kernel_name;
    error_string += ")";

    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaEventRecord(start, 0);

    // Launch the kernel
    err = cuLaunchKernel(kernel, grid.x, grid.y, grid.z, block.x, block.y, block.z, 0, NULL, args, NULL);
    checkErrDrv(err, error_string);
    err = cuCtxSynchronize();
    checkErrDrv(err, error_string);

    cudaEventRecord(end, 0);
    cudaEventSynchronize(end);
    cudaEventElapsedTime(&time, start, end);
    total_time += time;

    last_gpu_timing = time;
    if (print_timing) {
        std::cerr << "<HIPACC:> Kernel timing (" << block.x*block.y << ": " << block.x << "x" << block.y << "): " << time << "(ms)" << std::endl;
    }
}
void hipaccLaunchKernelBenchmark(CUfunction &kernel, const char *kernel_name, dim3 grid, dim3 block, void **args, bool print_timing=true) {
    float min_dt=FLT_MAX;

    for (int i=0; i<HIPACC_NUM_ITERATIONS; i++) {
        hipaccLaunchKernel(kernel, kernel_name, grid, block, args, print_timing);
        if (last_gpu_timing < min_dt) min_dt = last_gpu_timing;
    }

    last_gpu_timing = min_dt;
    if (print_timing) {
        std::cerr << "<HIPACC:> Kernel timing benchmark ("<< block.x*block.y << ": " << block.x << "x" << block.y << "): " << min_dt << "(ms)" << std::endl;
    }
}


// Get global reference from module
void hipaccGetGlobal(CUdeviceptr *result_global, CUmodule &module, std::string global_name) {
    CUresult err = CUDA_SUCCESS;
    CUdeviceptr global;
    HipaccContext &Ctx = HipaccContext::getInstance();
    size_t size;

    err = cuModuleGetGlobal(&global, &size, module, global_name.c_str());
    checkErrDrv(err, "cuModuleGetGlobal()");

    *result_global = global;
}


// Get texture reference from module
void hipaccGetTexRef(CUtexref *result_texture, CUmodule &module, std::string texture_name) {
    CUresult err = CUDA_SUCCESS;
    CUtexref tex;
    HipaccContext &Ctx = HipaccContext::getInstance();

    err = cuModuleGetTexRef(&tex, module, texture_name.c_str());
    checkErrDrv(err, "cuModuleGetTexRef()");

    *result_texture = tex;
}


// Bind texture to linear memory
void hipaccBindTextureDrv(CUtexref &texture, void *mem, CUarray_format format) {
    HipaccContext &Ctx = HipaccContext::getInstance();
    HipaccContext::cl_dims dim = Ctx.get_mem_dims(mem);

    checkErrDrv(cuTexRefSetFormat(texture, format, 1), "cuTexRefSetFormat()");
    checkErrDrv(cuTexRefSetFlags(texture, CU_TRSF_READ_AS_INTEGER), "cuTexRefSetFlags()");
    checkErrDrv(cuTexRefSetAddress(0, texture, (CUdeviceptr)mem, dim.pixel_size*dim.stride*dim.height), "cuTexRefSetAddress()");
    // not necessary?
    //checkErrDrv(cuParamSetTexRef(BF, CU_PARAM_TR_DEFAULT, texture), "cuParamSetTexRef()");
}


unsigned int nextPow2(unsigned int x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    ++x;

    // get at least the warp size
    if (x < 32) x = 32;

    return x;
}


// Perform global reduction and return result
template<typename T>
T hipaccApplyReduction(const char *kernel2D, const char *kernel1D, T *image, T
        neutral, unsigned int width, unsigned int height, unsigned int stride,
        unsigned int offset_x, unsigned int offset_y, unsigned int is_width,
        unsigned int is_height, unsigned int max_threads, unsigned int
        pixels_per_thread) {
    cudaError_t err = cudaSuccess;
    T *output;  // GPU memory for reduction
    T result;   // host result

    // first step: reduce image (region) into linear memory
    dim3 block(max_threads, 1);
    dim3 grid((int)ceil((float)(width)/(block.x*2)), (int)ceil((float)(is_height)/pixels_per_thread));
    unsigned int num_blocks = grid.x*grid.y;

    err = cudaMalloc((void **) &output, sizeof(T)*num_blocks);
    checkErr(err, "cudaMalloc()");

    size_t offset = 0;
    hipaccConfigureCall(grid, block);

    hipaccSetupArgument(&image, sizeof(T *), offset);
    hipaccSetupArgument(&output, sizeof(T *), offset);
    hipaccSetupArgument(&neutral, sizeof(T), offset);
    hipaccSetupArgument(&width, sizeof(unsigned int), offset);
    hipaccSetupArgument(&height, sizeof(unsigned int), offset);
    hipaccSetupArgument(&stride, sizeof(unsigned int), offset);
    // check if the reduction is applied to the whole image
    if ((offset_x || offset_y) && (is_width!=width || is_height!=height)) {
        hipaccSetupArgument(&offset_x, sizeof(unsigned int), offset);
        hipaccSetupArgument(&offset_y, sizeof(unsigned int), offset);
        hipaccSetupArgument(&is_width, sizeof(unsigned int), offset);
        hipaccSetupArgument(&is_height, sizeof(unsigned int), offset);
    }

    hipaccLaunchKernel(kernel2D, grid, block);


    // second step: reduce partial blocks on GPU
    // this is done in one shot, so no additional memory is required, i.e. the
    // same array can be used for the input and output array
    // block.x is fixed, either max_threads or power of two
    block.x = (num_blocks < max_threads) ? nextPow2((num_blocks+1)/2) :
        max_threads;
    grid.x = 1;
    grid.y = 1;
    // calculate the number of pixels reduced per thread
    int num_steps = (num_blocks + (block.x - 1)) / (block.x);

    offset = 0;
    hipaccConfigureCall(grid, block);

    hipaccSetupArgument(&output, sizeof(T *), offset);
    hipaccSetupArgument(&output, sizeof(T *), offset);
    hipaccSetupArgument(&neutral, sizeof(T), offset);
    hipaccSetupArgument(&num_blocks, sizeof(unsigned int), offset);
    hipaccSetupArgument(&num_steps, sizeof(unsigned int), offset);

    hipaccLaunchKernel(kernel1D, grid, block);

    // get reduced value
    err = cudaMemcpy(&result, output, sizeof(T), cudaMemcpyDeviceToHost);
    checkErr(err, "cudaMemcpy()");

    err = cudaFree(output);
    checkErr(err, "cudaFree()");

    return result;
}
// Perform global reduction and return result
template<typename T>
T hipaccApplyReduction(const char *kernel2D, const char *kernel1D, T *image, T
        neutral, unsigned int width, unsigned int height, unsigned int stride,
        unsigned int max_threads, unsigned int pixels_per_thread) {
    return hipaccApplyReduction<T>(kernel2D, kernel1D, image, neutral, width,
            height, stride, 0, 0, width, height, max_threads,
            pixels_per_thread);
}


// Perform global reduction using memory fence operations and return result
template<typename T>
T hipaccApplyReductionThreadFence(const char *kernel2D, T *image, T neutral,
        unsigned int width, unsigned int height, unsigned int stride, unsigned
        int offset_x, unsigned int offset_y, unsigned int is_width, unsigned int
        is_height, unsigned int max_threads, unsigned int pixels_per_thread) {
    cudaError_t err = cudaSuccess;
    T *output;  // GPU memory for reduction
    T result;   // host result

    // single step reduction: reduce image (region) into linear memory and
    // reduce the linear memory using memory fence operations
    dim3 block(max_threads, 1);
    dim3 grid((int)ceil((float)(width)/(block.x*2)), (int)ceil((float)(is_height)/pixels_per_thread));
    unsigned int num_blocks = grid.x*grid.y;

    err = cudaMalloc((void **) &output, sizeof(T)*num_blocks);
    checkErr(err, "cudaMalloc()");

    size_t offset = 0;
    hipaccConfigureCall(grid, block);

    hipaccSetupArgument(&image, sizeof(T *), offset);
    hipaccSetupArgument(&output, sizeof(T *), offset);
    hipaccSetupArgument(&neutral, sizeof(T), offset);
    hipaccSetupArgument(&width, sizeof(unsigned int), offset);
    hipaccSetupArgument(&height, sizeof(unsigned int), offset);
    hipaccSetupArgument(&stride, sizeof(unsigned int), offset);
    // check if the reduction is applied to the whole image
    if ((offset_x || offset_y) && (is_width!=width || is_height!=height)) {
        hipaccSetupArgument(&offset_x, sizeof(unsigned int), offset);
        hipaccSetupArgument(&offset_y, sizeof(unsigned int), offset);
        hipaccSetupArgument(&is_width, sizeof(unsigned int), offset);
        hipaccSetupArgument(&is_height, sizeof(unsigned int), offset);
    }

    hipaccLaunchKernel(kernel2D, grid, block);

    err = cudaMemcpy(&result, output, sizeof(T), cudaMemcpyDeviceToHost);
    checkErr(err, "cudaMemcpy()");

    err = cudaFree(output);
    checkErr(err, "cudaFree()");

    return result;
}
// Perform global reduction using memory fence operations and return result
template<typename T>
T hipaccApplyReductionThreadFence(const char *kernel2D, T *image, T neutral,
        unsigned int width, unsigned int height, unsigned int stride, unsigned
        int max_threads, unsigned int pixels_per_thread) {
    return hipaccApplyReductionThreadFence<T>(kernel2D, image, neutral, width,
            height, stride, 0, 0, width, height, max_threads,
            pixels_per_thread);
}


// Perform global reduction and return result
template<typename T>
T hipaccApplyReductionExploration(const char *filename, const char *kernel2D,
        const char *kernel1D, T *image, T neutral, unsigned int width, unsigned
        int height, unsigned int stride, unsigned int offset_x, unsigned int
        offset_y, unsigned int is_width, unsigned int is_height, unsigned int
        max_threads, unsigned int pixels_per_thread, int cc) {
    cudaError_t err = cudaSuccess;
    T *output;  // GPU memory for reduction
    T result;   // host result

    unsigned int num_blocks = (int)ceil((float)(width)/(max_threads*2))*is_height;
    err = cudaMalloc((void **) &output, sizeof(T)*num_blocks);
    checkErr(err, "cudaMalloc()");

    void *argsReduction2D[] = {
        (void *)&image,
        (void *)&output,
        (void *)&neutral,
        (void *)&width,
        (void *)&height,
        (void *)&stride,
        (void *)&offset_x,
        (void *)&offset_y,
        (void *)&is_width,
        (void *)&is_height
    };

    std::cerr << "<HIPACC:> Exploring pixels per thread for '" << kernel2D << ", " << kernel1D << "'" << std::endl;

    for (unsigned int ppt=1; ppt<=is_height; ppt++) {
        std::stringstream num_ppt_ss;
        std::stringstream num_bs_ss;
        num_ppt_ss << ppt;
        num_bs_ss << max_threads;

        std::string compile_options = "-D PPT=" + num_ppt_ss.str() + " -D BS=" + num_bs_ss.str() + " -I./include ";
        hipaccCompileCUDAToPTX(filename, cc, compile_options.c_str());

        std::string ptx_filename = filename;
        ptx_filename += ".ptx";
        CUmodule modReduction;
        CUfunction exploreReduction2D;
        CUfunction exploreReduction1D;
        hipaccCreateModuleKernel(&exploreReduction2D, &modReduction, ptx_filename, kernel2D, cc);
        hipaccCreateModuleKernel(&exploreReduction1D, &modReduction, ptx_filename, kernel1D, cc);

        float min_dt=FLT_MAX;
        for (int i=0; i<HIPACC_NUM_ITERATIONS; i++) {
            dim3 block(max_threads, 1);
            dim3 grid((int)ceil((float)(width)/(block.x*2)), (int)ceil((float)(is_height)/ppt));
            num_blocks = grid.x*grid.y;

            // start timing
            total_time = 0.0f;

            hipaccLaunchKernel(exploreReduction2D, kernel2D, grid, block, argsReduction2D, false);


            // second step: reduce partial blocks on GPU
            grid.y = 1;
            while (num_blocks > 1) {
                block.x = (num_blocks < max_threads) ? nextPow2((num_blocks+1)/2) :
                    max_threads;
                grid.x = (int)ceil((float)(num_blocks)/(block.x*ppt));

                void *argsReduction1D[] = {
                    (void *)&output,
                    (void *)&output,
                    (void *)&neutral,
                    (void *)&num_blocks,
                    (void *)&ppt
                };

                hipaccLaunchKernel(exploreReduction1D, kernel1D, grid, block, argsReduction1D, false);

                num_blocks = grid.x;
            }
            // stop timing
            if (total_time < min_dt) min_dt = total_time;
        }

        // print timing
        std::cerr.precision(5);
        std::cerr << "<HIPACC:> PPT: " << ppt << ",    " << min_dt << " ms" << std::endl;
    }

    // get reduced value
    err = cudaMemcpy(&result, output, sizeof(T), cudaMemcpyDeviceToHost);
    checkErr(err, "cudaMemcpy()");

    err = cudaFree(output);
    checkErr(err, "cudaFree()");

    return result;
}
template<typename T>
T hipaccApplyReductionExploration(const char *filename, const char *kernel2D,
        const char *kernel1D, T *image, T neutral, unsigned int width, unsigned
        int height, unsigned int stride, unsigned int max_threads, unsigned int
        pixels_per_thread, int cc) {
    return hipaccApplyReductionExploration<T>(filename, kernel2D, kernel1D, image,
            neutral, width, height, stride, 0, 0, width, height, max_threads,
            pixels_per_thread, cc);
}


// Perform configuration exploration for a kernel call
void hipaccKernelExploration(const char *filename, const char *kernel,
        std::vector<void *> args, std::vector<hipacc_smem_info> smems,
        std::vector<hipacc_const_info> consts, std::vector<hipacc_tex_info>
        texs, int is_width, int is_height, int warp_size, int
        max_threads_per_block, int max_threads_for_kernel, int
        max_smem_per_block, int pixels_per_thread, int max_size_x, int
        max_size_y, int opt_tx, int opt_ty, int cc) {
    CUresult err = CUDA_SUCCESS;
    std::string ptx_filename = filename;
    ptx_filename += ".ptx";

    std::cerr << "<HIPACC:> Exploring configurations for kernel '" << kernel << "': optimal configuration ";
    std::cerr << opt_tx*opt_ty << "(" << opt_tx << "x" << opt_ty << "). " << std::endl;

    for (int tile_size_x=warp_size; tile_size_x<=max_threads_per_block; tile_size_x+=warp_size) {
        for (int tile_size_y=1; tile_size_y<=max_threads_per_block; tile_size_y++) {
            // check if we exceed maximum number of threads
            if (tile_size_x*tile_size_y > max_threads_for_kernel) continue;

            // check if we exceed size of shared memory
            int used_smem = 0;
            for (unsigned int i=0; i<smems.size(); i++) {
                used_smem += (tile_size_x + smems.data()[i].size_x)*(tile_size_y + smems.data()[i].size_y - 1) * smems.data()[i].pixel_size;
            }
            if (used_smem >= max_smem_per_block) continue;

            int num_blocks_bh_x = (int)ceil((float)(max_size_x) / (float)tile_size_x);
            int num_blocks_bh_y = (int)ceil((float)(max_size_y) / (float)(tile_size_y*pixels_per_thread));
            std::stringstream num_blocks_bh_x_ss, num_blocks_bh_y_ss;
            num_blocks_bh_x_ss << num_blocks_bh_x;
            num_blocks_bh_y_ss << num_blocks_bh_y;
            std::stringstream num_threads_x_ss, num_threads_y_ss;
            num_threads_x_ss << tile_size_x;
            num_threads_y_ss << tile_size_y;

            // compile kernel
            std::string compile_options = "-D BSX_EXPLORE=" + num_threads_x_ss.str() + " -D BSY_EXPLORE=" + num_threads_y_ss.str();
            compile_options += " -D BHX_EXPLORE=" + num_blocks_bh_x_ss.str() + " -D BHY_EXPLORE=" + num_blocks_bh_y_ss.str() + " ";
            hipaccCompileCUDAToPTX(filename, cc, compile_options.c_str());

            CUmodule modKernel;
            CUfunction exploreKernel;
            hipaccCreateModuleKernel(&exploreKernel, &modKernel, ptx_filename, kernel, cc);

            // load constant memory
            CUdeviceptr constMem;
            for (unsigned int i=0; i<consts.size(); i++) {
                hipaccGetGlobal(&constMem, modKernel, consts.data()[i].name);
                cuMemcpyHtoD(constMem, consts.data()[i].memory, consts.data()[i].size);
                checkErrDrv(err, "cuMemcpyHtoD()");
            }

            // bind texture memory
            CUtexref texImage;
            for (unsigned int i=0; i<texs.size(); i++) {
                hipaccGetTexRef(&texImage, modKernel, texs.data()[i].name);
                hipaccBindTextureDrv(texImage, texs.data()[i].image, texs.data()[i].type);
            }

            dim3 block(tile_size_x, tile_size_y);
            dim3 grid((int)ceil((float)(is_width)/tile_size_x), ceil((float)(is_height)/(tile_size_y*pixels_per_thread)));

            float min_dt=FLT_MAX;
            for (int i=0; i<HIPACC_NUM_ITERATIONS; i++) {

                // start timing
                total_time = 0.0f;

                hipaccLaunchKernel(exploreKernel, kernel, grid, block, args.data(), false);

                // stop timing
                if (total_time < min_dt) min_dt = total_time;
            }

            // print timing
            std::cerr.precision(5);
            std::cerr << "<HIPACC:> Kernel config: " << tile_size_x << "x" << tile_size_y << " (" << tile_size_x*tile_size_y << "): " << min_dt << " ms" << std::endl;
        }
    }
}

#endif  // __HIPACC_CUDA_HPP__
