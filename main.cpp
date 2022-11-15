#include <iostream>
#include <vector>

#include "common.hpp"
#include "max-flow.hpp"


std::vector<ZoneInfo> load_zones()
{
    // Load zone amount
    node_idx_t n;
    std::cin >> n;

    // Load zones
    std::vector<ZoneInfo> zones(n);

    for (zone_idx_t i = 0; i < n; ++i)
    {
        ZoneInfo zone;
        
        std::cin 
            >> zone.x >> zone.y 
            >> zone.capacity[0] >> zone.capacity[1] 
            >> zone.capacity[2] >> zone.capacity[3];

        zones[i] = zone;
    }


    // Return vector of zones
    return zones;
}

int main()
{
    auto zones = load_zones();



    // FlowGraph graph(0, 1);

    // std::list<FlowEdge> edges;

    // for (node_idx_t i = 0; i < m; i++)
    // {
    //     node_idx_t start, end, capacity;
    //     std::cin >> start >> end >> capacity;

    //     edges.push_back(FlowEdge(start, end, capacity));
    // }

    // graph.add_edges(edges);


    // // Print total flow
    // std::cout << graph.maximize_flow() << std::endl;


    // Return success
    return 0;
}
