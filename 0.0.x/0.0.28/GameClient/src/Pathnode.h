#ifndef PathnodeH
#define PathnodeH

// One tile of the path search: the tile, the node it was reached from, and the pass.
struct PathNode
{
    bool expanded;
    int id;
    int parent_id;
    int x;
    int y;
    int depth;

    PathNode(int node_id, int from_node_id, int tile_x, int tile_y, int search_pass);
    ~PathNode();
};

#endif
