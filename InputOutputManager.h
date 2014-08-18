#ifndef INPUTREADER_H
#define INPUTREADER_H

#include <vector>
#include <set>
#include <utility>

#include "Module.h"
#include "Floorplans.h"

std::pair<std::vector<Module*>, std::set<Module*> > readBlocks(std::string fileName);
void writeFloorplan(std::string fileName, BaseFloorplan* floorplan, std::set<Module*> modules);
void writeFloorplan(std::ofstream& outFile, BaseFloorplan* root, std::set<Module*> modules);

#endif // INPUTREADER_H
