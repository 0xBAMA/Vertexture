#version 330

in  vec3 vPosition;
in  vec3 vNormal;
in  vec3 vColor;
out vec4 color;
out vec4 norm;

uniform int t;
uniform int scroll;
uniform float scale;
uniform mat4 proj;

uniform vec3 ucolor;
uniform vec3 offset;

uniform sampler2D rock_height_tex;
uniform sampler2D rock_normal_tex;

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




  vec2 timeoffset = vec2(0.05 * sin(0.03 * t), 0.05 * cos(0.03 * t));



  vec4 trefh;

  switch(scroll)
  {
    case 0:
      trefh = texture(rock_height_tex, scale * (0.25 * offset.xy) + timeoffset);
      break;
    case 1:
      trefh = texture(rock_height_tex, scale * (0.2 * offset.xy + vec2(t/1000.0) + 0.15 * offset.xy + vec2(t/7000.0)) + timeoffset);
      break;
    case 2:
      trefh = texture(rock_height_tex, scale * ( 0.2 * offset.xy + vec2(t/7000.0) + 0.15 * offset.xy + vec2(t/7000.0)) + timeoffset);
      break;
    default:
      trefh = vec4(1.0, 0.0, 0.0, 1.0);
      break;
  }


  //trefh tells the height of the ground under the dude




  vec4 trefn;

  switch(scroll)
  {
    case 0:
      trefn = texture(rock_normal_tex, scale * (0.25 * offset.xy) + timeoffset);
      break;
    case 1:
      trefn = texture(rock_normal_tex, scale * (0.2 * offset.xy + vec2(t/1000.0) + 0.15 * offset.xy + vec2(t/7000.0))+ timeoffset);
      break;
    case 2:
      trefn = texture(rock_normal_tex, scale * ( 0.2 * offset.xy + vec2(t/7000.0) + 0.15 * offset.xy + vec2(t/7000.0))+timeoffset);
      break;
    default:
      trefn = vec4(1.0, 0.0, 0.0, 1.0);
      break;
  }

  // color = vec4(0.3*dot(trefh, trefn));
  norm = trefn;
  // color = vec4(0.2,0.6,0.2,1.0);
  color = vec4(ucolor,1.0);



  //from trefn, derive basis vectors
  //vec3 basisx;
  //vec3 basisy;
  //vec3 basisz = 0.1 * trefn.xyz;

  //basisx = 0.1 * normalize(cross(vec3(-1,-1,-1),basisz));
  //basisy = 0.1 * normalize(cross(basisx,basisz));

  //now you're starting at the point that's some amount above or below water, which you tell by comparing
  //trefh's components to 0.5 - less than 0.5, underwater, greater than 0.5, above ground.

  //so uPosition has told you location of the dude or tree, in terms of where it is on the ground

  //vPosition will now be something you multiply with the basis vectors to get the final location of the primitives


















  // vec4 vPosition_local = vec4(0.5*vPosition, 1.0f) + <vec4 representing offset from ground> + + 0.2 * vec4(0,0,trefh.z - 0.5,0);
  vec4 vPosition_local = vec4(0.5*vPosition, 1.0f) - 0.5 * vec4(timeoffset.x,timeoffset.y,trefh.z - 0.5,0) + vec4(offset.x,offset.y,0,0);

  // gl_Position = proj * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 0.003*t) * vPosition_local;

  gl_Position = proj * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), 0.25) * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 2.15) * rotationMatrix(vec3(0.0f, 0.0f, 1.0f), 0.5 * sin(0.0005 * t) + 0.3) * vPosition_local;



}
