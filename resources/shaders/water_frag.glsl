#version 330
varying  vec4 color;
varying  vec3 norm;

bool depthcolor = false;



void
main()
{
    gl_FragColor = color;

    gl_FragColor *= dot(vec3(1,1,1), norm);
    gl_FragColor.a *= 0.2;



    //these are used to draw something along the lines of scanlines (per-pixel and dithering-style effects)
    int fcxmod2 = int(gl_FragCoord.x) % 2;
    int fcymod3 = int(gl_FragCoord.y) % 3;

    if((fcymod3 == 0) || (fcxmod2 == 0))
    {
      discard;
      // gl_FragColor.r = 0.1;
    }

}
