#version 330

in  vec3 vPosition;
in  vec3 vNormal;
in  vec3 vColor;
out vec3 vpos;

uniform int t;
uniform mat4 proj;

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

  vec4 vPosition_local = vec4(0.5*vPosition, 1.0f);

  gl_Position = proj * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), 0.25) * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 2.15) * rotationMatrix(vec3(0.0f, 0.0f, 1.0f),   0.5 * sin(0.0005 * t) + 0.3) * vPosition_local;

  vpos = 0.2 * vPosition;

  // if(sample.x < 0.5)
  //   color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  // else
  //   color = vec4(0.2, 0.2, 0.2, 7.0/255.0);

}
