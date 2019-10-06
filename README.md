# Vertexture
experimenting with displacing verticies with a texture


todo -


pass that x,y, into the DudesAndTreesModel's function to handle that, drop the package


simulation -

input arguments define parameters of the simulation - # good, bad guys, # trees, # boxes initially available










initialization - called in main's initialization

  -population of the entities vector with good guys, bad guys, trees, according to input arguments

  -(add boxes, to show how many you have? - might be a good idea, along the edge of the board)

  -all the shader and texture stuff, uniform setup

  -player starts with zero points, and # of boxes to place based on the input argument






update_sim - called by the timer function

  -responsible for movement
    -if there are no boxes, move a small amount in a random direction
    -if there is one or more boxes, move towards the closest one by a small amount

  -scoring(box capture)
    -good guys score a point, remove the box
    -bad guys, no score change, remove the box

  -tree 'avoidance'
    -move in a random direction a small amount?

  -as a final check, see if it is in the range of (-phi,phi), and if not, probably put it at (0,0) (maybe the nearest corner?)






place_box - called by the click function

  -message and failure for each of the following (NO DECREMENT):
    -off the board - indicated by a black pixel
    -underweater - indicated by a blue value > 0
    -in a tree - indicate a failure

  -message and other behavior (DECREMENT BOX COUNT):
    -hit a good dude, he dies, you lose a point, box is placed near that location
    -hit a bad dude, he dies, you gain a point, box is placed near that location

  -just spawn a box, echo location, decrement box count
    -if there's an empty space, distance to all other entities > some threshold
