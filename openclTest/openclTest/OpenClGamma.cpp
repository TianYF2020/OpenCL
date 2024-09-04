#include "OpenClGamma.h"
#include <CL/cl.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

// OpenCL kernel for gamma correction
const char* kernelSource = R"(
__kernel void gammaCorrection(__global uchar* image, float gamma, int width, int height) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int idx = (y * width + x) * 3;

    for (int i = 0; i < 3; ++i) {
        float pixel = image[idx + i] / 255.0f;
        pixel = pow(pixel, gamma);
        image[idx + i] = (uchar)(pixel * 255.0f);
    }
}
)";

int testCLGamma() {
    // 读取图片
    cv::Mat img = cv::imread("color.png");
    if (img.empty()) {
        std::cerr << "无法打开图片" << std::endl;
        return -1;
    }

    int width = img.cols;
    int height = img.rows;
    size_t imgSize = width * height * 3 * sizeof(uchar);

    // 初始化OpenCL
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices.front();

    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    cl::Program::Sources sources(1, std::make_pair(kernelSource, strlen(kernelSource)));
    cl::Program program(context, sources);
    program.build();

    // 创建缓冲区
    cl::Buffer bufferImage(context, CL_MEM_READ_WRITE, imgSize);

    // 将图片数据拷贝到设备缓冲区
    queue.enqueueWriteBuffer(bufferImage, CL_TRUE, 0, imgSize, img.data);

    // 设置内核参数
    cl::Kernel kernel(program, "gammaCorrection");
    kernel.setArg(0, bufferImage);
    kernel.setArg(1, 10.2f);  // 设置gamma值为1.2
    kernel.setArg(2, width);
    kernel.setArg(3, height);

    // 执行内核
    cl::NDRange global(width, height);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange);
    queue.finish();

    // 读取处理后的图片数据
    queue.enqueueReadBuffer(bufferImage, CL_TRUE, 0, imgSize, img.data);

    // 显示处理后的图片
    cv::imwrite("output.jpg", img);
    cv::imshow("Gamma Corrected Image", img);
    cv::waitKey(0);

    return 0;
}
