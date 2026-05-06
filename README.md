Gamma Engine and Gamma's RAD Army Knife(GRAK) translated to C++

This was originally a 3D engine and a set of map compiling utilities which I made in Scratch, and decided to improve on in C++. It utilizes binary space partitioning in order to create BSP trees from lazy meshes. These trees are incorporated into the map file as models. These files can be read in by the engine, and rendered in real-time. 

It features radiosity-based global illumination. 

I am planning on making a more robust re-write of Gamma Engine with brush-based level editing capibilities. For now, I may conclude this repository.

The source can be built on linux. Run make to build the programs: GBSP, GRAD, and GAMMA