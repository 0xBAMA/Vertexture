Jon Baker 10/8/2019
Project 2

Packed the same way as last time, with 'tar czvf project2.gz ./*'

This project has quite a bit going on. I am having some trouble getting it to
include the freeglut library on the lab machines, but I have included code that
I have had running on my laptop and I have seen it work, and if there's trouble
I am ready to run it on my own laptop in class. The shaders that I am
using to do my selection are not allowing me to use them, so I have not been
able to get it running in the lab, because that is fundamental to the rest of
how the program works. On my own laptop, I have had no issues at all.

I have included code directly from the repository that I have been working from,
with the final code in it. I tried to get things working but I ended up just
reverting to what I had working.

The main visual element of the program is the heightmap which sits in the center
of the screen and acts as the 'board' on which everything else takes place. It
Consists of a large quad subdivided a number of times. In  the GroundModel class,
there is a texture loaded, which represents the height of the terrain at all points,
a heightmap. Once this is loaded, the Sampler2d object in the vertex shader
displaces the verticies to the heights indicated. There is also some stuff to do
with normal maps, but I didn't have huge success with those except for using them
to animate the good and bad guys, making them bounce along the normals of the
ground below where they were (<-maybe cool points, see what you think). A big thing
I have achieved in this project is the ability to include multiple textures
going into any given shader, with the use of texture units and separate bindings
for each of them. (putting an integer value into each of the uniform samplers,
which maps it to the same texture unit in each display function - this process
is manual right now, so I have to keep track of those numbers). The heightmap
part of the project was something I was working on the weekend before we got the
assignment because I noticed that samplers were specified as uniform, so I wanted
to see if I could use them in the vertex shader to displace geometry. This was
successful, and the rest I did once we got the project, including the water and
the skirts around the bottom which keep you from seeing the back side of the
triangles which are used to represent the heightmap. The base vertical positions
of each of the good and bad guys is also set by the ground's heightmap (another
consequence of this multiple texture thing).

The water is a major application of this ability to use multiple textures, which
I got working last Monday, because it uses a heightmap to displace the geometry
in a sort of subtle way, it uses a matching normal map, and a third texture for
coloration (<-cool points). In the water's fragment shader there's a simple dot
product calculation that gives you some glitteryness on the water's surface.

With the command line arguments, you can set the number of bad guys, the number
of good guys, the number of trees, and the number of boxes which you are initially
able to place. If these fields are left blank, the default values for all of them
will be used, and they will all be set to 3. The representation for the good and
bad guys is a simple sphere, which is achieved by using a small texture and mapping
it to the extents of the point by using gl_PointCoord. It works very similarly to
the heightmap, but is used to discard fragments around the round extents of it,
and the greyscale values set the depth of each fragment, so that they can be
accurately considered when depth testing against all the other objects in the
scene (<-cool points). These are each drawn with the same point, the first
that is in the vertex buffer, e.g. glDrawArrays(GL_POINTS,0,1). There is a
uniform offset and color which I use to draw each of these based on the information
kept on the CPU.

Selection is done by using a separate shader, as I briefly mentioned earlier,
which uses the red channel of the output to represent the x coordinate of the
model, the green channel to represent the y coordinate of the model, and the
blue channel to tell whether or not you clicked in water. This is much more
easily communicated by showing a picture of the colored representation, but
by getting these values back to the cpu, I can get information that represents
where I clicked on the translated and rotated model. By using this information,
I can spawn a box at the locaiton specified by the user, accurately positioning
the object in the location that they specify. As a consequence of this accuracy
being made available to the user, there are some consequences built into the
'game', such as it is. Great power great responsibility and all that. Anyways,
if you click too close to a tree, you will lose that box, as it was lost in the
tree. If you click in the water, you will also lose that box, as it was lost in
the water. If you click too close to one of your own or the enemy guys,
there are consequences as well...

Once a box is placed, all the guys on the board begin to move towards it - the
first to get to it will capture it for their team - that is, good guys score a
point, and bad guys prevent a point from being scored. once the supply of boxes
is exhausted, and there are no boxes on the field, the game exits, and reports
your score on the command line.

Trees act as an obstacle by doing a distance check with every guy that moves around
and if they are within a certain distance, the guy is translated back by an amount
that is based on the vector from the tree to that guy, so it is directly pushed away.
In practice is is not very apparent unless you specifically try to exhibit that
behavior but it is in there.
