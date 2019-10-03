#version 330

in  vec3 vPosition;
in  vec3 vNormal;
in  vec3 vColor;
out vec4 color;
out vec3 norm;

uniform int t;
uniform int scroll;
uniform mat4 proj;

uniform sampler2D ground_tex;
uniform sampler2D height_tex;
uniform sampler2D normal_tex;
uniform sampler2D color_tex;

uniform float scale;

uniform float thresh;

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

void main()
{

  vec2 offset = vec2(0.0005 * t, 0.0001 * t);


  vec4 ground_read;
  switch(scroll)
    {
      case 0:
        ground_read = texture(ground_tex, scale * (0.25 * vPosition.xy));
        break;
      case 1:
        ground_read = texture(ground_tex, scale * (0.2 * vPosition.xy + vec2(t/1000.0) + 0.15 * vPosition.xy + vec2(t/7000.0)));
        break;
      case 2:
        ground_read = texture(ground_tex, scale * ( 0.2 * vPosition.xy + vec2(t/7000.0) + 0.15 * vPosition.xy + vec2(t/7000.0)));
        break;
      default:
        ground_read = vec4(1.0, 0.0, 0.0, 1.0);
        break;
    }










  vec4 height_read = texture(height_tex, 2*vPosition.xy + offset);
  vec4 normal_read = texture(normal_tex, 2*vPosition.xy + offset);
  vec4 color_read = texture(color_tex, 2*vPosition.xy + offset);

  // color = (normal_read + color_read) / 2;//wrong but looks interesting
  color = color_read / 1.618;


  height_read.x *= 0.1 * (sin(0.08 * t) + 1.0) * sin(vPosition.x * vPosition.y * 0.01);



  vec4 vPosition_local = vec4(0.5*vPosition, 1.0f) + vec4(0.0, 0.0, 0.01 * height_read.x, 0.0);
  norm = normal_read.xyz;


  gl_Position = proj * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), 0.25) * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 2.15) * rotationMatrix(vec3(0.0f, 0.0f, 1.0f),   0.5 * sin(0.0005 * t) + 0.3) * vPosition_local;

  // color = vec4(0.2, 0.6, 0.55, 5*0.1618);

}
