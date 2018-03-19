#ifndef GRAPH_H
#define GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
#include "gurobi_c++.h"
//#include <scip/scip.h>

using namespace boost;

/**
 * Struct representing a pixel in the imput image
 */
struct Pixel
{
    uint32_t x; ///< x coordinate
    uint32_t y; ///< y coordinate

    Pixel(uint32_t x_, uint32_t y_) : x(x_), y(y_)
    {}
};

/**
 * Struct representing a superpixel
 */
struct Superpixel
{
    double color; ///< color of the superpixel, i.e. the average color of all pixels contained in it
    std::vector<Pixel> pixels; ///< vector of all pixels contained in the superpixel
    std::vector< std::vector<GRBVar>> var;
    double merge_label; // for the l0-gradient-algorithm
};


struct edge_property
{
};


/**
 * The graph type using the [Boost Graph Library](http://www.boost.org/doc/libs/1_64_0/libs/graph/doc/index.html)
 * This type forbids parallel edges and allows the creation of subgraphs.
 * Each node is a `Superpixel`, i.e. it has the proberties `color` and `pixels`.
 */
typedef subgraph<adjacency_list<
        setS, // setS disallows parallel edges
        vecS, undirectedS,
        Superpixel,
        property<edge_index_t, size_t,
        property<edge_weight_t, size_t>> // edge weight is the number of neighbouring pixels,
                                         // i.e. the number of edges in the pixel graph
                                         // connecting both superpixels
    >>
    Graph;
 
#endif
