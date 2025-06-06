// Copyright (C) 2018-2025 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "utils.hpp"

#include <memory>
#include <unordered_set>

#include "openvino/core/model.hpp"
#include "openvino/core/node.hpp"
#include "openvino/core/type.hpp"
#include "openvino/op/matmul.hpp"
#include "openvino/op/multiply.hpp"
#include "ov_ops/fully_connected.hpp"
#include "transformations/rt_info/dequantization_node.hpp"
#include "transformations/utils/utils.hpp"

namespace ov::intel_cpu {

bool has_matmul_with_compressed_weights(const std::shared_ptr<const ov::Model>& model) {
    bool has_decompression_multiply = false;
    auto is_decompression_multiply = [&](ov::Node* node) {
        if (auto* multiply = ov::as_type<ov::op::v1::Multiply>(node)) {
            if (ov::is_dequantization_node(multiply->shared_from_this())) {
                has_decompression_multiply = true;
            }
        }
    };

    for (const auto& op : model->get_ops()) {
        if (!ov::is_type_any_of<ov::op::v0::MatMul, ov::op::internal::FullyConnected>(op)) {
            continue;
        }

        if (!op->get_input_element_type(0).is_real()) {
            continue;
        }

        auto weights = op->input_value(1);
        if (!ov::op::util::is_on_constant_path(weights)) {
            continue;
        }

        std::unordered_set<Node*> visited;
        ov::op::util::visit_constant_path(weights.get_node(), visited, is_decompression_multiply);

        if (has_decompression_multiply) {
            return true;
        }
    }
    return false;
}

}  // namespace ov::intel_cpu
