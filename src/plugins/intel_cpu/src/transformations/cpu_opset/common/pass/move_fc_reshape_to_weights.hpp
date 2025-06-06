// Copyright (C) 2018-2025 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "openvino/pass/matcher_pass.hpp"

namespace ov::intel_cpu {

/**
 * This transformation is applied to the FC with compressed 3D u8 weights. It moves Reshape at the weights path to the
 * constants in order to constant fold the Reshape node. Example: Weights(3D)   Subtract_const(3D) Weights(2D)
 * Subtract_const(2D) |             /                                        |             / Convert
 * Subtract_convert(opt)                       Convert    Subtract_convert(opt) |      / |      / Subtract(opt)
 * Subtract(opt) |      Multiply_const(3D)        ====>                 |      Multiply_const(2D) |     / |     /
 *                    Multiply                                               Multiply
 *                       |                                                      |
 *                    Reshape(2D)                                               |
 *                       |                                                      |
 *         Data      Transpose(opt)                               Data      Transpose(opt)
 *             \     /                                                \     /
 *         FullyConnected                                         FullyConnected
 */
class MoveFCReshapeToWeights : public ov::pass::MatcherPass {
public:
    OPENVINO_MATCHER_PASS_RTTI("MoveFCReshapeToWeights");
    MoveFCReshapeToWeights();
};

}  // namespace ov::intel_cpu
