#version 330

in  vec3 vPosition;
in  vec3 vNormal;
in  vec3 vColor;
out vec4 color;
out vec2 norm_coord;

uniform int t;
uniform int scroll;
uniform float scale;
uniform mat4 proj;

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

void main()
{


  vec4 tref;
  vec4 n1,n2,n3;

  // float scale = 1.0;

  switch(scroll)
  {
    case 0:
      tref = texture(height_tex, scale * (0.25 * vPosition.xy));
      norm_coord = scale * (0.25 * vPosition.xy);

      break;
    case 1:
      tref = texture(height_tex, scale * (0.2 * vPosition.xy + vec2(t/1000.0) + 0.15 * vPosition.xy + vec2(t/7000.0)));
      norm_coord = scale * (0.2 * vPosition.xy + vec2(t/1000.0) + 0.15 * vPosition.xy + vec2(t/7000.0));

      break;
    case 2:
      tref = texture(height_tex, scale * ( 0.2 * vPosition.xy + vec2(t/7000.0) + 0.15 * vPosition.xy + vec2(t/7000.0)));
      norm_coord = scale * ( 0.2 * vPosition.xy + vec2(t/7000.0) + 0.15 * vPosition.xy + vec2(t/7000.0));

      break;
    default:
      tref = vec4(1.0, 0.0, 0.0, 1.0);
      norm_coord = vec2(1.0, 0.0);

      break;
  }

  vec4 vPosition_local = vec4(0.5*vPosition, 1.0f) + 0.2 * vec4(0,0,tref.z - 0.5,0);

  // norm = (n1.xyz + n2.xyz + n3.xyz) / 3.0;

  // gl_Position = proj * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 0.003*t) * vPosition_local;

  gl_Position = proj * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), 0.25) * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 2.15) * rotationMatrix(vec3(0.0f, 0.0f, 1.0f), 0.5 * sin(0.0005 * t) + 0.3) * vPosition_local;


  // color = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0f);

  color = tref;
  // color.r *= 0.9;
  // color.g *= 0.7;
  // color.b *= 0.3;


  color.r *= 0.4;
  color.g *= 0.5;
  color.b *= 0.4;


}
