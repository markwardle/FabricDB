/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 23, 2015
 */

#ifndef _FABRIC_H__
#define _FABRIC_H__
#include "Internal.h"

void Fabric_create_graph(FILE *file, Graph *new_graph);
void Fabric_load_graph(FILE *graph_file, Graph *graph);
void Fabric_dump_graph_header (Graph *graph);

#endif