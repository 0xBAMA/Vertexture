#version 330
varying  vec4 color;
varying  vec3 norm;


void
main()
{

    // gl_FragColor = vec4(color.rgb * (0.8f-gl_FragCoord.z), 1.0f);
    gl_FragColor = color;
    gl_FragColor *= dot(vec3(0.0,0.0,1.0),norm);
    // gl_FragColor.xyz = norm;


    //fcymod3 is used to draw something along the lines of scanlines
    int fcxmod2 = int(gl_FragCoord.x) % 2;
    int fcymod3 = int(gl_FragCoord.y) % 3;

    if((fcymod3 == 0) || (fcxmod2 == 0)) //add  '&& (fcxmod2 == 0)' for x based stuff
    {
      // discard;
      gl_FragColor.r *= 1.618;
    }

}
