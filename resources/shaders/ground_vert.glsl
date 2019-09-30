#version 330

in  vec3 vPosition;
in  vec3 vNormal;
in  vec3 vColor;
out vec4 color;

uniform int t;
uniform mat4 proj;

uniform sampler2D tex;

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

  // vec4 tref = texture(tex, 0.2 * vPosition.xy + vec2(t/7000.0) + 0.15 * vPosition.xy + vec2(t/7000.0));
  vec4 tref = texture(tex, 0.25 * vPosition.xy);
  // vec4 tref = vec4(0.5); //no displacement

  vec4 vPosition_local = vec4(0.5*vPosition, 1.0f) + 0.2 * vec4(0,0,tref.z - 0.5,0);

  // gl_Position = proj * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 0.003*t) * vPosition_local;

  gl_Position = proj * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), 0.25) * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 2.15) * rotationMatrix(vec3(0.0f, 0.0f, 1.0f), 0.5 * sin(0.0005 * t) + 0.3) * vPosition_local;


  // color = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0f);

  color = tref;
  color.r *= 0.9;
  color.g *= 0.7;
  color.b *= 0.3;
}
