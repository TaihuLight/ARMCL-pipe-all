/*
 * Copyright (c) 2021 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef ARM_COMPUTE_CPU_SOFTMAX_H
#define ARM_COMPUTE_CPU_SOFTMAX_H

#include "arm_compute/core/ITensorInfo.h"
#include "arm_compute/core/experimental/Types.h"
#include "src/core/cpu/ICpuKernel.h"
#include "src/runtime/cpu/ICpuOperator.h"
#include "src/runtime/cpu/operators/CpuPermute.h"
#include <memory>

namespace arm_compute
{
namespace cpu
{
class CpuLogits1DMaxKernel;
template <bool IS_LOG>
class CpuLogits1DSoftmaxKernel;

/** Basic function to compute a SoftmaxLayer and a Log SoftmaxLayer.
 *
 * Softmax is calculated by :
 * @f[ out = exp((x - max(x)) * beta) / sum(exp((x - max(x)) * beta)) @f]
 *
 * Log Softmax is calculated by :
 * @f[ out = (x - max(x) * beta) - log(\sum{e^{x - max(x) * beta}}) @f]
 *
 * This function runs the following function/kernels:
 * -# If axis is not 0:
 * -# @ref CpuPermute
 * -# @ref kernels::CpuLogits1DMaxKernel
 * -# @ref kernels::CpuLogits1DSoftmaxKernel
 */
template <bool IS_LOG = false>
class CpuSoftmaxGeneric : public ICpuOperator
{
public:
    /** Constructor */
    CpuSoftmaxGeneric();
    /** Set the input and output tensors.
     *
     * @param[in,out] src  Source tensor info. Data types supported: QASYMM8/QASYMM8_SIGNED/F16/F32.
     *                     last value of each row to the nearest multiple.
     * @param[out]    dst  Destination tensor ifo. Data types supported: same as @p input.
     * @param[in]     beta (Optional) A scaling factor for the exponent.
     * @param[in]     axis (Optional) The dimension in which to apply the function. E.g. for input of shape 4x5x6 and
     *                       axis=1, softmax will be applied to 4x6=24 vectors of size 5. Defaults to 0
     */
    void configure(const ITensorInfo *src, ITensorInfo *dst, float beta = 1.0f, int32_t axis = 0);

    /** Static function to check if given info will lead to a valid configuration of @ref CpuSoftmax
     *
     * @param[in] src  Source tensor info. Data types supported: QASYMM8/QASYMM8_SIGNED/F16/F32.
     * @param[in] dst  Destination tensor info. Data types supported: same as @p input
     * @param[in] beta (Optional) A scaling factor for the exponent.
     * @param[in] axis (Optional) The dimension in which to apply the function. E.g. for input of shape 4x5x6 and
     *                       axis=1, softmax will be applied to 4x6=24 vectors of size 5. Defaults to 0
     *
     * @return a status
     */
    static Status validate(const ITensorInfo *src, const ITensorInfo *dst, float beta = 1.0f, int32_t axis = 0);

    // Inherited methods overridden:
    void run(ITensorPack &tensors) override;
    experimental::MemoryRequirements workspace() const override;

private:
    CpuPermute                   _permute_input;
    CpuPermute                   _permute_output;
    std::unique_ptr<ICpuKernel>  _max_kernel;
    std::unique_ptr<ICpuKernel>  _softmax_kernel;
    std::unique_ptr<ITensorInfo> _max;
    std::unique_ptr<ITensorInfo> _tmp;
    std::unique_ptr<ITensorInfo> _input_permuted;
    std::unique_ptr<ITensorInfo> _output_permuted;
    bool                         _needs_permute;
};
using CpuSoftmax    = CpuSoftmaxGeneric<false>;
using CpuLogSoftmax = CpuSoftmaxGeneric<true>;

} // namespace cpu
} // namespace arm_compute
#endif /* ARM_COMPUTE_CPU_SOFTMAX_H */
