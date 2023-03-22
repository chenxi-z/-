#include "xcorr.h"
#include "stdlib.h"
#include "arm_math.h"
#include "arm_const_structs.h"

// 定义xcorr函数的实现
void xcorr(float* x, float* y, float* r)
{
    // 定义相关变量
    uint32_t i;
    // arm_status status;
    uint32_t fftSize = SIGNAL_LENGTH * 2; // FFT的大小为信号长度的两倍
    uint32_t ifftFlag = 0; // 不进行反向FFT变换
    uint32_t doBitReverse = 1; // 需要进行位序反转
    uint32_t corrLength = fftSize - 1; // 互相关函数的长度为FFT长度减1
    float32_t* buffer = (float32_t*)malloc(fftSize * sizeof(float32_t)); // 分配缓冲区
    float32_t* output = (float32_t*)malloc(fftSize * sizeof(float32_t)); // 分配输出数组
    float32_t* temp = (float32_t*)malloc(corrLength * sizeof(float32_t)); // 分配临时数组
    float32_t norm = (float32_t)(1.0 / SIGNAL_LENGTH); // 缩放因子

    // 复制x和y到buffer中
    for (i = 0; i < SIGNAL_LENGTH; i++) {
        buffer[i] = x[i];
        buffer[i + SIGNAL_LENGTH] = y[i];
    }

    // 执行FFT计算
    arm_cfft_f32(&arm_cfft_sR_f32_len256, buffer, ifftFlag, doBitReverse);

    // 计算互相关函数
    arm_cmplx_mult_cmplx_f32(buffer, buffer + SIGNAL_LENGTH, output, SIGNAL_LENGTH); // 计算x和y的复数乘积
    arm_cmplx_conj_f32(buffer + SIGNAL_LENGTH, buffer + SIGNAL_LENGTH, SIGNAL_LENGTH); // 计算y的共轭
    arm_cmplx_mult_cmplx_f32(output, buffer + SIGNAL_LENGTH, output, SIGNAL_LENGTH); // 计算复数乘积和共轭的复数乘积
    arm_cfft_f32(&arm_cfft_sR_f32_len256, output, ifftFlag, doBitReverse); // 执行IFFT
    arm_cmplx_mag_f32(output, temp, corrLength); // 计算复数的模
    arm_scale_f32(temp, norm, temp, corrLength); // 缩放

    // 将结果复制到输出数组r中
    for (i = 0; i < corrLength; i++) {
        r[i] = temp[i];
    }

    // 释放内存
    free(buffer);
    free(output);
    free(temp);
}

/* 
可以使用arm_max_f32函数直接提取互相关函数的输出结果中的最大值与其索引
计算数组的最大值和最大值的索引
arm_status status = arm_max_f32(myArray, 10, &maxValue, &maxIndex);
*/
