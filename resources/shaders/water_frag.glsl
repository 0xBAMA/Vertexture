#version 330
varying  vec4 color;
in float abort;

bool depthcolor = false;

void
main()
{
    // gl_FragColor = color;

    if(abort >= 1.0f)
      discard;

    // gl_FragColor = vec4(color.rgb * (0.8f-gl_FragCoord.z), 1.0f);


    //fcymod3 is used to draw something along the lines of scanlines
    int fcxmod2 = int(gl_FragCoord.x) % 2;
    int fcymod3 = int(gl_FragCoord.y) % 3;

    if((fcymod3 == 0) || (fcxmod2 == 0)) //add  '&& (fcxmod2 == 0)' for x based stuff
    {
      // discard;
      gl_FragColor.r = 0.1;
    }



    float dist = distance(gl_PointCoord, vec2(0.5f, 0.5f));
    float orig = gl_FragCoord.z;


    if(depthcolor)
      gl_FragColor = color * (1.0f - 0.3f * orig);
    else
      gl_FragColor = color;


    if(dist < 0.5f)
    {
      gl_FragColor *= dot(gl_PointCoord+vec2(0.5), vec2(1.0, 1.0));

      gl_FragDepth = orig;

      if(dist < 0.4)
        gl_FragDepth = orig + 0.01;
      else if(dist < 0.3)
        gl_FragDepth = orig + 0.02;
      else if(dist < 0.2)
        gl_FragDepth = orig + 0.03;
    }
    else
    {
      discard;
    }



}
