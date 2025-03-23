/**
 *
 *  Copyright 2016-2020 Netflix, Inc.
 *
 *     Licensed under the BSD+Patent License (the "License");
 *     you may not use this file except in compliance with the License.
 *     You may obtain a copy of the License at
 *
 *         https://opensource.org/licenses/BSDplusPatent
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 *
 */

#include <stdint.h>
#include <math.h>
#include <libvmaf/picture.h>
#include <stdlib.h>
#include <stdio.h>

// void picture_copy_hbd(float *dst, ptrdiff_t dst_stride,
//                       VmafPicture *src, int offset, float scaler)
// {
//     float *float_data = dst;
//     uint16_t *data = src->data[0];

//     for (unsigned i = 0; i < src->h[0]; i++) {
//         for (unsigned j = 0; j < src->w[0]; j++) {
//             // scales high-bit-depth images down to the same 8-bit range.
//             float_data[j] = (float) data[j] / scaler + offset;
//         }
//         float_data += dst_stride / sizeof(float);
//         data += src->stride[0] / 2;
//     }
//     return;
// }

void picture_copy_hbd(float *dst, ptrdiff_t dst_stride,
                      VmafPicture *src, int offset, float max_value)
{
    float beta = 4.0f;
    // printf("Debug: max_value=%.6f\n", max_value);
    // printf("Debug: beta=%.6f\n", beta);
    // if (src == NULL || src->data[0] == NULL)
    // {
    //     printf("Error: src or src->data[0] is NULL\n");
    //     exit(1);
    // }
    // if (dst == NULL)
    // {
    //     printf("Error: dst is NULL\n");
    //     exit(1);
    // }
    // if (max_value >= 0)
    // {
    //     printf("Error: max_value is invalid (%.6f)\n", max_value);
    //     exit(1);
    // }

    float *float_data = dst;
    uint16_t *data = src->data[0];

    for (unsigned i = 0; i < src->h[0]; i++)
    {
        for (unsigned j = 0; j < src->w[0]; j++)
        {
            // if (j >= src->w[0])
            // {
            //     printf("Error: j (%d) exceeds width (%d)\n", j, src->w[0]);
            //     exit(1);
            // }

            // Normalize to [-1,1]
            float Y_norm = 2.0f * ((float)data[j] / max_value) - 1.0f;
            // if (isnan(Y_norm) || isinf(Y_norm))
            // {
            //     printf("Error: Y_norm is NaN/Inf at (%d, %d), data=%u\n", i, j, data[j]);
            //     exit(1);
            // }

            // Piecewise Expansive Non-Linearity
            float Y_transformed;
            if (Y_norm >= 0)
            {
                Y_transformed = exp(beta * Y_norm) - 1.0f;
            }
            else
            {
                Y_transformed = 1.0f - exp(-beta * Y_norm);
            }

            // if (isnan(Y_transformed) || isinf(Y_transformed))
            // {
            //     printf("Error: Y_transformed is NaN/Inf at (%d, %d)\n", i, j);
            //     exit(1);
            // }

            // Normalize to [0,255]
            float_data[j] = Y_transformed * 256.0f + offset;
            // if (isnan(float_data[j]) || isinf(float_data[j]))
            // {
            //     printf("Error: float_data[%d][%d] is NaN/Inf\n", i, j);
            //     exit(1);
            // }

            // // ✅ 分步打印，防止崩溃
            // printf("(%d, %d): data=%u, ", i, j, data[j]);
            // printf("Y_norm=%.6f, Y_transformed=%.6f, ", Y_norm, Y_transformed);
            // printf("float_data=%.6f\n", float_data[j]);
        }

        float_data += dst_stride / sizeof(float);
        data += src->stride[0] / 2;
    }

    return;
}

void picture_copy(float *dst, ptrdiff_t dst_stride,
                  VmafPicture *src, int offset, unsigned bpc)
{
    if (bpc == 10)
    {
        picture_copy_hbd(dst, dst_stride, src, offset, 1024.0f);
        return;
    }
    else if (bpc == 12)
    {
        picture_copy_hbd(dst, dst_stride, src, offset, 1024.0f);
        return;
    }
    else if (bpc == 16)
    {
        picture_copy_hbd(dst, dst_stride, src, offset, 1024.0f);
        return;
    }

    float *float_data = dst;
    uint8_t *data = src->data[0];

    for (unsigned i = 0; i < src->h[0]; i++)
    {
        for (unsigned j = 0; j < src->w[0]; j++)
        {
            float_data[j] = (float)data[j] + offset;
        }
        float_data += dst_stride / sizeof(float);
        data += src->stride[0];
    }

    return;
}
