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

  vec4 tref = texture(tex, 0.75 * vPosition.xy + vec2(t/1000.0));

  vec4 vPosition_local = vec4(vPosition, 1.0f) + 0.25 * (tref - vec4(0.5));

  // gl_Position = proj * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 0.003*t) * vPosition_local;

  gl_Position = proj * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 120) * vPosition_local;


  // color = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0f);

  color = tref;
  color.r *= 0.9;
  color.g *= 0.5;
  color.b *= 0.2;
}
