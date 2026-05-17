// Logging state singleton — shared between the header-only inline log API in
// <collab/core.hpp> and the module impl unit (lib/collab.core/src/log.cpp).
//
// `state()` is an inline function returning a function-local static. Across
// all TUs that include this header, there is exactly one log_state instance —
// the inline rule merges them at link time. That gives both consumption modes
// (header-only #include and modular import) a single source of truth for the
// current level and the sink list.

#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "collab/detail/types.hpp"

namespace collab::log::detail {

struct log_state {
    level                              current_level = level::info;
    std::vector<std::unique_ptr<sink>> sinks;
    std::mutex                         mtx;
};

inline log_state& state() {
    static log_state s;
    return s;
}

}  // namespace collab::log::detail
