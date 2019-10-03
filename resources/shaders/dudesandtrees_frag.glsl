#version 330
varying  vec4 color;
varying  vec4 norm;

uniform sampler2D point_sprite;

void
main()
{

    // gl_FragColor = vec4(color.rgb * (0.8f-gl_FragCoord.z), 1.0f);
    gl_FragColor = color;
    // gl_FragColor=vec4(1.0);


    gl_FragColor.a = texture(point_sprite,gl_PointCoord.xy).r;

    if(gl_FragColor.a == 0.0)
      discard;




    gl_FragDepth = gl_FragCoord.z - 0.01*(gl_FragColor.a-0.5);


    //fcymod3 is used to draw something along the lines of scanlines
    int fcxmod2 = int(gl_FragCoord.x) % 2;
    int fcymod2 = int(gl_FragCoord.y) % 2;

    if((fcymod2 == 1) || (fcxmod2 == 1))
    {
      // discard;
      gl_FragColor.r = 0;
    }

}
