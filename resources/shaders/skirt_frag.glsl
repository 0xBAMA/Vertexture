#version 330
varying  vec4 color;

in vec2 texcoord;

in vec4 vpos;

uniform sampler2D tex;


void
main()
{

    float height_offset = 0.2 * (color.z - 0.5);

    gl_FragColor  =  vec4(0.1, 0.45, 0.6, 0.5);  //the water's color

    if(vpos.z < 0.0 + height_offset)
    {
      gl_FragColor  = vec4((vpos.z + 0.2) * vec3(0.55, 0.45, 0.15), 1.0); //the ground's color

      //fcymod3 is used to draw something along the lines of scanlines
      int fcxmod2 = int(gl_FragCoord.x) % 2;
      int fcymod2 = int(gl_FragCoord.y) % 2;

      if((fcymod2 == 0) || (fcxmod2 == 0)) //add  '&& (fcxmod2 == 0)' for x based stuff
      {
        // discard;
        gl_FragColor.r *= 2*1.618;
        gl_FragColor.g *= 1.618;
      }
    }


    if((vpos.z > 0.0 + height_offset) && vpos.z > 0.0) discard;   //throw away everything above the water

    if(gl_FragColor == vec4(0.1, 0.45, 0.6, 0.5))
    {
      //fcymod3 is used to draw something along the lines of scanlines
      int fcxmod2 = int(gl_FragCoord.x) % 2;
      int fcymod2 = int(gl_FragCoord.y) % 2;

      if((fcymod2 == 1) || (fcxmod2 == 1)) //add  '&& (fcxmod2 == 0)' for x based stuff
      {
        // discard;
        gl_FragColor /= 1.618;
      }
    }

}
