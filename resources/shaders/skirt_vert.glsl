#version 330

in  vec3 vPosition;
in  vec3 vNormal;
in  vec3 vColor;
out vec4 color;

out vec4 vpos;

// out vec2 ground_texcoord;

uniform int t;
uniform int scroll;
uniform float scale;
uniform mat4 proj;

uniform float thresh;

uniform sampler2D ground_tex;
uniform sampler2D water_tex;


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


  vec4 vPosition_local = vec4(0.5*vPosition, 1.0f);

  vec2 offset = vec2(0.0005 * t, 0.0001 * t);
  vec4 height_read = texture(water_tex, 2*vPosition.xy + offset);


  // color = height_read;

  // float scale = 1.0;

  switch(scroll)
  {
    case 0:
      color = texture(ground_tex, scale * (0.25 * vPosition.xy));
      break;
    case 1:
      color = texture(ground_tex, scale * (0.2 * vPosition.xy + vec2(t/1000.0) + 0.15 * vPosition.xy + vec2(t/7000.0)));
      break;
    case 2:
      color = texture(ground_tex, scale * (0.2 * vPosition.xy + vec2(t/7000.0) + 0.15 * vPosition.xy + vec2(t/7000.0)));
      break;
    default:
      color = vec4(1.0, 0.0, 0.0, 1.0);
      break;
  }

  // float height_offset = 0.2 * (color.z - 0.5);


  vpos = vPosition_local;

  gl_Position = proj * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), 0.25) * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 2.15) * rotationMatrix(vec3(0.0f, 0.0f, 1.0f),   0.5 * sin(0.0005 * t) + 0.3) * vPosition_local;

}
