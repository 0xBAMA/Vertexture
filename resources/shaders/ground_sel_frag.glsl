#version 330
varying  vec4 color;


void
main()
{

    // gl_FragColor = vec4(color.rgb * (0.8f-gl_FragCoord.z), 1.0f);
    gl_FragColor = color;

}
