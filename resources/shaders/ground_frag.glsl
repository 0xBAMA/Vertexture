#version 330
varying  vec4 color;
varying  vec2 norm_coord;


uniform int t;
uniform int show_normals;

uniform sampler2D height_tex;
uniform sampler2D normal_tex;
uniform sampler2D normal_smooth1_tex;
uniform sampler2D normal_smooth2_tex;



//thanks to Neil Mendoza via http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}


void
main()
{
  gl_FragColor = color;

  if(show_normals ==1)
  {
    vec4 norm;

    norm = texture(normal_tex, norm_coord) + texture(normal_smooth1_tex, norm_coord) + texture(normal_smooth2_tex, norm_coord);

    norm /= 3;

    vec3 light = (rotationMatrix(vec3(0.0,0.0,1.0), 2.2 * sin(0.01 * t)) * vec4(vec3(1.0,1.0,1.0),1.0)).xyz;

    gl_FragColor *= dot(light,norm.xyz);
  }


    //fcymod3 is used to draw something along the lines of scanlines
    int fcxmod2 = int(gl_FragCoord.x) % 2;
    int fcymod3 = int(gl_FragCoord.y) % 3;

    if((fcymod3 == 0) || (fcxmod2 == 0)) //add  '&& (fcxmod2 == 0)' for x based stuff
    {
      // discard;
      gl_FragColor.r *= 1.618;
    }

}
