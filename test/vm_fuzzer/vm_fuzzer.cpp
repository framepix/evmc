// EVMC: Ethereum Client-VM Connector API
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

#include <evmc/loader.h>

#include <cstdlib>

namespace
{
struct host : evmc_context
{
    host();
};

bool account_exists(evmc_context* context, const evmc_address* address)
{
    return false;
}

bool get_storage(evmc_bytes32* result,
                        evmc_context* context,
                        const evmc_address* address,
                        const evmc_bytes32* key)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    auto it = host->accounts.find(*address);
    if (it != host->accounts.end())
    {
        *result = it->second.storage[*key];
        return true;
    }
    return false;
}

static enum evmc_storage_status set_storage(evmc_context* context,
                                            const evmc_address* address,
                                            const evmc_bytes32* key,
                                            const evmc_bytes32* value)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    auto accountIt = host->accounts.find(*address);
    if (accountIt == host->accounts.end())
        return EVMC_STORAGE_NON_EXISTING_ACCOUNT;

    auto storageIt = accountIt->second.storage.find(*key);
    if (storageIt == accountIt->second.storage.end())
    {
        accountIt->second.storage.emplace(std::make_pair(*key, *value));
        return EVMC_STORAGE_ADDED;
    }
    else if (storageIt->second == *value)
    {
        return EVMC_STORAGE_UNCHANGED;
    }
    else
    {
        storageIt->second = *value;
        return EVMC_STORAGE_MODIFIED;
    }
}

static bool get_balance(evmc_uint256be* result, evmc_context* context, const evmc_address* address)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    auto it = host->accounts.find(*address);
    if (it != host->accounts.end())
    {
        *result = it->second.balance;
        return true;
    }
    return false;
}

static bool get_code_size(size_t* result, evmc_context* context, const evmc_address* address)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    auto it = host->accounts.find(*address);
    if (it != host->accounts.end())
    {
        *result = it->second.code_size;
        return true;
    }
    return false;
}

static bool get_code_hash(evmc_bytes32* result, evmc_context* context, const evmc_address* address)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    auto it = host->accounts.find(*address);
    if (it != host->accounts.end())
    {
        *result = it->second.code_hash;
        return true;
    }
    return false;
}

static size_t copy_code(evmc_context* context,
                        const evmc_address* address,
                        size_t code_offset,
                        uint8_t* buffer_data,
                        size_t buffer_size)
{
    (void)context;
    (void)address;
    (void)code_offset;
    (void)buffer_data;
    (void)buffer_size;
    return 0;
}

static void selfdestruct(evmc_context* context,
                         const evmc_address* address,
                         const evmc_address* beneficiary)
{
    (void)context;
    (void)address;
    (void)beneficiary;
}

static evmc_result call(evmc_context* context, const evmc_message* msg)
{
    (void)context;
    (void)msg;
    evmc_result result{};
    result.status_code = EVMC_FAILURE;
    return result;
}

static evmc_tx_context get_tx_context(evmc_context* context)
{
    (void)context;
    evmc_tx_context result{};
    return result;
}

static evmc_bytes32 get_block_hash(evmc_context* context, int64_t number)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    int64_t current_block_number = host->tx_context.block_number;

    evmc_bytes32 example_block_hash;
    if (number < current_block_number && number >= current_block_number - 256)
        example_block_hash = {{1, 1, 1, 1}};
    else
        example_block_hash = {};
    return example_block_hash;
}

static void emit_log(evmc_context* context,
                     const evmc_address* address,
                     const uint8_t* data,
                     size_t data_size,
                     const evmc_bytes32 topics[],
                     size_t topics_count)
{
    (void)context;
    (void)address;
    (void)data;
    (void)data_size;
    (void)topics;
    (void)topics_count;
}

static const evmc_host_interface interface = {
    account_exists, get_storage,  set_storage, get_balance,    get_code_size,  get_code_hash,
    copy_code,      selfdestruct, call,        get_tx_context, get_block_hash, emit_log,
};

evmc_instance* load()
{
    const char* path = std::getenv("EVMC_VM");
    if (!path)
        std::exit(2);

    auto* vm = evmc_load_and_create(path, nullptr);
    if (!vm)
        std::exit(1);

    return vm;
}

auto* vm = load();

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    evmc_message msg{};
    msg.gas = 50000;


    return 0;
}