#version 330
varying  vec4 color;

bool depthcolor = false;

varying vec3 vpos;

uniform int t;


uniform sampler3D perlin_tex;


void
main()
{
    //these are used to draw something along the lines of scanlines (per-pixel and dithering-style effects)
    // int fcxmod2 = int(gl_FragCoord.x) % 2;
    // int fcymod3 = int(gl_FragCoord.y) % 3;
    //
    // if((fcymod3 != 0) || (fcxmod2 != 0))
    // {
    //   // discard;
    //   gl_FragColor.r = 0.1;
    // }

    gl_FragColor = texture(perlin_tex, vpos + vec3(0.0005 * t, 0.003 * t, 0.0));
    gl_FragColor *= texture(perlin_tex, 5 * vpos + vec3(0.0005 * t, 0.001 * t, 0.0)).r;
    gl_FragColor *= texture(perlin_tex, 3 * vpos + vec3(0.002 * t, 0.003 * t, 0.001 * t)).r;
    gl_FragColor.a *= 0.1;
    gl_FragColor.g *= 0.65;
    gl_FragColor.b *= 0.75;
    gl_FragColor.r *= 0.2;

    if(gl_FragColor.a > 0.1)
      gl_FragColor = vec4(1.0, 1.0, 1.0, 0.1*vpos.z);


}
