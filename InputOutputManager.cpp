#include "InputOutputManager.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <cassert>
#include <sstream>
#include <regex>

#include <boost/algorithm/string.hpp>

std::pair<std::vector<Module*>, std::set<Module*> > readBlocks(std::string fileName)
{
    std::ifstream inFile;
    inFile.open(fileName.c_str());
    if (inFile.fail()) {
        throw std::runtime_error("Cannot read the file " + fileName);
    }

    std::vector<Module*> modules;
    std::regex numEx("([0-9]*\\.?[0-9]*) ([0-9]*\\.?[0-9]*) ([0-9]*\\.?[0-9]*) ([0-9]*\\.?[0-9]*)( [+-]{1})?");
    std::smatch match;
    std::string line;
    int counter = 0;
    std::set<Module*> netModules;
    while (std::getline(inFile, line)) {
        boost::trim(line);
        if (std::regex_match(line, match, numEx)) {
            assert(match.size() == 6);
            double x = (double)atof(match[1].str().c_str());
            double y = (double)atof(match[2].str().c_str());
            double width = (double)atof(match[3].str().c_str());
            double height = (double)atof(match[4].str().c_str());
            std::string name = static_cast<std::ostringstream*>( &(std::ostringstream() << (++counter)) )->str();
            Module* module = new Module(x, y, width, height, name);
            modules.push_back(module);
            std::string status = match[5].str();
            boost::trim(status);
            if (status == "+") {
                module->sign = Module::Pos;
                netModules.insert(module);
            } else if (status =="-") {
                module->sign = Module::Neg;
                netModules.insert(module);
            }
        } else {
            throw std::runtime_error("Input file is in a wrong format!");
        }
    }

    inFile.close();

    return std::make_pair(modules, netModules);
}

void writeFloorplan(std::string fileName, BaseFloorplan* floorplan, std::set<Module*> modules)
{
    std::ofstream outFile;
    outFile.open(fileName.c_str());
    if (outFile.fail()) {
        throw std::runtime_error("Cannot write to file " + fileName);
    }
    writeFloorplan(outFile, floorplan, modules);
    outFile.close();
}

void writeFloorplan(std::ofstream& outFile, BaseFloorplan* root, std::set<Module*> modules)
{
    LeafFloorplan* leaf = dynamic_cast<LeafFloorplan*>(root);
    if (0 != leaf) {
        outFile<<leaf->rect.x()<<" "<<leaf->rect.y()<<" "<<leaf->rect.width()<<" "<<leaf->rect.height();
        if (modules.find(leaf->module) != modules.end()) {
            outFile<<" +\n";
        } else {
            outFile<<"\n";
        }
        return;
    }

    Floorplan* floorplan = dynamic_cast<Floorplan*>(root);
    if (0 != floorplan) {
        writeFloorplan(outFile, floorplan->left, modules);
        writeFloorplan(outFile, floorplan->right, modules);
    }
}
