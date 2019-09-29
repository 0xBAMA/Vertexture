#version 330
varying  vec4 color;


void
main()
{
    gl_FragColor = color;

    // gl_FragColor = vec4(color.rgb * (0.8f-gl_FragCoord.z), 1.0f);


    //fcymod3 is used to draw something along the lines of scanlines
    int fcxmod2 = int(gl_FragCoord.x) % 2;
    int fcymod3 = int(gl_FragCoord.y) % 3;

    if((fcymod3 == 0) || (fcxmod2 == 0)) //add  '&& (fcxmod2 == 0)' for x based stuff
    {
      discard;
      // gl_FragColor.r = 0;
    }



    if(distance(gl_PointCoord, vec2(0.5f, 0.5f)) < 0.5f)
    {
      gl_FragColor *= dot(gl_PointCoord+vec2(0.5), vec2(1.0, 1.0));
    }
    else
    {
      discard;
    }



}
