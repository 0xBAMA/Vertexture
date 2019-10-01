#version 330
varying  vec4 color;

in vec2 texcoord;

in vec4 vpos;

uniform sampler2D tex;


void
main()
{

    float height_offset = 0.2 * (color.z - 0.5);

    gl_FragColor  =  vec4(0.2, 0.6, 0.6, 0.35);  //the water's color

    if(vpos.z < 0.0 + height_offset)  gl_FragColor  = vec4((vpos.z + 0.2) * vec3(0.55, 0.45, 0.15), 1.0); //the ground's color

    if((vpos.z > 0.0 + height_offset) && vpos.z > 0.0) discard;   //throw away everything above the water

}
