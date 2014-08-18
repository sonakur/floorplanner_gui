Floorplannner with gui.

To test this floorplanner you need to provide an input file, consisting of lines, each of which describes a block of the floorplan. Line format is the following:

<x coordinate> <y coordinate> <block width> <block height> <+>?

The symbol "+" is an optional parameter, for indicateing if the block must be consdered in net migration or reduce distance algorithms.

Reduce ditsance algorithm reduces distance between 2 indicated blocks.
Net migration algorithm reduces distance bwetween multiple blocks.
