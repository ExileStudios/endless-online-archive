#include <vcl.h>
#pragma hdrstop

#include "Pathnode.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

PathNode::PathNode(int node_id, int from_node_id, int tile_x, int tile_y, int search_pass)
{
    expanded = false;
    id = node_id;
    parent_id = from_node_id;
    x = tile_x;
    y = tile_y;
    depth = search_pass;
}

PathNode::~PathNode()
{
}
