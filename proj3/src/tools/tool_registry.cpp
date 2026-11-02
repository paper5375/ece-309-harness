// src/tools/tool_registry.cpp — YOU IMPLEMENT (spec §4.3).
//
// A hand-written hash table. No std::unordered_map / std::map / std::set.
// Every function below is a stub so the starter compiles. Replace each body.

#include "tools/tool_registry.h"
#include <vector>

ToolRegistry::ToolRegistry() : tool_num(0), tool_table(initialBuckets) {};
ToolRegistry::~ToolRegistry() = default;   // unique_ptrs in your buckets free the tools

std::size_t ToolRegistry::hash(std::string_view key) noexcept {
    //FNV-1a (spec Appendix A) or another real string hash

    // From Appendix A:
    std::uint64_t h = 1469598103934665603ULL;                         // "offset basis"
    for (const unsigned char c : key) { h ^= c; h *= 1099511628211ULL; }     // "FNV prime"
    return static_cast<std::size_t>(h);
}

bool ToolRegistry::add(std::unique_ptr<Tool> tool) {
    if (!tool || find(tool->name()) != nullptr) {
        tool.reset(); // delete the tool
        return false;
    }

    // Rehash if needed
    if (load_factor() > load_factor_threshold) rehash(tool_table.size() * 2);

    // Compute index
    const std::size_t index = hash(tool->name()) & (tool_table.size()-1);

    // Add tool to bucket (move the pointer and not the tool)
    tool_table[index].push_back(std::move(tool));

    // Increment tool count
    tool_num++;

    return true;
}

Tool* ToolRegistry::find(std::string_view name) const {
    const std::size_t index = hash(name) & (tool_table.size()-1);
    const std::vector<std::unique_ptr<Tool>>& bucket = tool_table[index];

    // Search the bucket for the correct tool
    for (const auto& tool : bucket) {
        if (tool->name() == name) return tool.get();
    }

    return nullptr;
}

std::size_t ToolRegistry::size() const noexcept {
    return this->tool_num;
}

void ToolRegistry::for_each(const std::function<void(const Tool&)>& fn) const {
    for (const auto& bucket : tool_table) {
        for (const auto& tool : bucket) {
            fn(*tool); // dereference unique_ptr<Tool> -> Tool&
        }
    }
}

std::size_t ToolRegistry::bucket_count() const noexcept {
    return tool_table.size();
}

double ToolRegistry::load_factor() const noexcept {
    return static_cast<double>(tool_num) / static_cast<double>(tool_table.size());
}

std::size_t ToolRegistry::max_probe_length() const {
    std::size_t max_chain_length = 0;
    for (const auto& bucket : tool_table) {
        if (bucket.size() > max_chain_length) max_chain_length = bucket.size();
    }
    return max_chain_length;
}

// CUSTOM FUNCTIONS
void ToolRegistry::rehash(const std::size_t new_count) {
    // Declare a temporary tool table with the new size
    std::vector<std::vector<std::unique_ptr<Tool>>> new_table(new_count);

    // For each tool in the old table, rehash it in the new table
    for (auto& bucket : tool_table) {
        for (auto& tool : bucket) {
            const std::size_t index = hash(tool->name()) & (new_count - 1); // Compute index
            new_table[index].push_back(std::move(tool)); // Move pointer to bucket
        }
    }

    // Move the new table into tool_table
    // new_table (which now holds the old data) is destroyed after this
    tool_table.swap(new_table);
}
