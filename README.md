# Vertexture
experimenting with displacing verticies with a texture


todo -





fix selection (zero offset - currently half the board is clipping at 0, also make sure to scale because the square's geometry is not unit either, unless you want to clip? the edges would be sort of a fuzzy border and a buffer around the edge would all translate to that edge value, which might be a good thing)

write code to get color and then translate that to an x,y in main.cc, in the click callback

pass that x,y, into the DudesAndTreesModel's function to handle that, drop the package
