#include <algorithm>
#include <numeric>
#include <vector>
#include <tuple>

#include "common.hpp"


template<typename TData, typename TIndex>
std::tuple<std::vector<TIndex>, std::vector<TIndex>> 
argsort(std::vector<TData>& data, bool (comp)(const TIndex&, const TIndex&), const bool& inplace = false)
{

    // Get indexes to sort
    std::vector<int> idx(data.size());
    std::iota(idx.begin(), idx.end(), 0);

    // Get sorted indexes
    std::sort
    (
        idx.begin(),
        idx.end(),
        comp
        //[&](int i1, int i2) { return e[i1] < e[i2]; }
    );


    // Calculate inverse indexes
    std::vector<int> idx_inv(data.size());
    for

    if (inplace)
    {

    }

    
    // Return sorted indexes and inverse indexes
    return {idx, idx_inv};
}