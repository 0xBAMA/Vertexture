#version 330
varying  vec4 color;


void
main()
{

    // gl_FragColor = vec4(color.rgb * (0.8f-gl_FragCoord.z), 1.0f);
    gl_FragColor = color;


    //fcymod3 is used to draw something along the lines of scanlines
    int fcxmod2 = int(gl_FragCoord.x) % 2;
    int fcymod3 = int(gl_FragCoord.y) % 3;

    if((fcymod3 == 0) || (fcxmod2 == 0)) 
    {
      // discard;
      gl_FragColor.r *= 1.618;
    }

}
