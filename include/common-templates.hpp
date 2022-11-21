#pragma once
#include <algorithm>
#include <numeric>
#include <vector>
#include <tuple>
#include <functional>

#include "common.hpp"


template <typename TData, typename TIndex>
std::tuple<std::vector<TIndex>, std::vector<TIndex>>
argsort(const TIndex& size, const std::function<bool(const TIndex&, const TIndex&)>& comp, const bool& inverse_index)
{

    // Get indexes to sort
    std::vector<TIndex> idx(size);
    std::iota(idx.begin(), idx.end(), 0);

    // Get sorted indexes
    std::sort
    (
        idx.begin(),
        idx.end(),
        //NOTE: Should return false if data[i1].atribute < data[i2].attribute
        comp
    );

    // If inverse index should be returned, calculate it
    std::vector<TIndex> idx_inv;
    if (inverse_index)
    {
        idx_inv = std::vector<TIndex>(size);

        for (TIndex i = 0; i < size; i++)
            idx_inv[idx[i]] = i;
    }

    
    // Return sorted indexes and inverse indexes
    return {idx, idx_inv};
}


template <typename TData, typename TIndex>
std::vector<TIndex>
argsort(const TIndex& size, const std::function<bool(const TIndex&, const TIndex&)>& comp)
{
    return std::get<0>(argsort<TData, TIndex>(size, comp, false));
}


template <typename TData, typename TIndex>
std::tuple<std::vector<TIndex>, std::vector<TIndex>>
argsort(std::vector<TData>& data, const std::function<bool(const TIndex&, const TIndex&)>& comp, const bool& inverse_index)
{
    // Get sorted indexes
    auto [idx, idx_inv] = argsort<TData, TIndex>(data.size(), comp, inverse_index);

    // Sort data
    std::vector<TData> data_new(data.size());
    for (TIndex i = 0; i < data.size(); i++)
        data_new[i] = data[idx[i]];

    data = data_new;

    // Return indexes
    return {idx, idx_inv};
}


template <typename TData, typename TIndex>
std::tuple<std::vector<TIndex>, std::vector<TIndex>>
argsort(std::vector<TData>& data, const bool& inverse_index)
{
    return argsort<TData, TIndex>(
        data,
        [&](const TIndex& i1, const TIndex& i2) -> bool { return data[i1] < data[i2]; },
        inverse_index
    );
}


template <typename TData, typename TIndex>
std::vector<TIndex>
argsort(std::vector<TData>& data, const std::function<bool(const TIndex&, const TIndex&)>& comp)
{
    return std::get<0>(argsort(data, comp, false));
}


template <typename TData, typename TIndex>
std::vector<TIndex>
argsort(std::vector<TData>& data)
{
    return std::get<0>(argsort(data, false));
}



template <typename TData, typename TValue, typename TIndex>
void find_sequences
(
    const std::vector<TData>& data,
    const std::function<TValue
    (
        const TData& data
    )>& value_extractor,
    const std::function<void
    (
        const TIndex&, const TIndex&
    )>& process_sequence
)
{
    TIndex size = data.size();

    // Find all sequences of common values 
    //  by iterating over sorted values 
    //  and finding sequences of equal values
    TIndex start = 0;
    TIndex end = 0;
    TValue prev_val = value_extractor(data[0]);

    for (TIndex i = 1; i < size; i++)
    {
        TValue val = value_extractor(data[i]);

        if (prev_val != val)
        {
            if (start != end)
                process_sequence(start, end);

            start = i;
            prev_val = val;
        }

        end = i;
    }


    // If there is a final sequence, mutate it
    if (start != end)
        process_sequence(start, end);
}



// TODO: binary search for closest element
