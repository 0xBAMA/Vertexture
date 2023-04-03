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

uniform int bounce;

uniform vec3 ucolor;
uniform vec3 offset;

uniform sampler2D rock_height_tex;
uniform sampler2D rock_normal_tex;

//thanks to Neil Mendoza via http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main() {
	vec4 trefh = texture(rock_height_tex,  (0.5 * offset.xy));// + timeoffset);
	vec4 trefn = texture(rock_normal_tex, (0.5 * offset.xy));

	norm = trefn;
	color = vec4(ucolor,1.0);

	float height_scale = 1.5*clamp(0.3 * (trefh.z - 0.5),0,1) + 0.05;
	vec4 texture_height_offset;

	if(bounce == 1)
		texture_height_offset = 0.6 * vec4(0,0,height_scale,0) + vec4(0,0,trefn.x * 0.005 * (sin(0.05 * t) - 0.7),0.0) + vec4(0,0,trefh.x * 0.005 * (sin(0.05 * t) - 0.7),0.0);
	else
		texture_height_offset = 0.6 * vec4(0,0,height_scale,0);

	vec4 vPosition_rotated = rotationMatrix(vec3(0.0,0.0,1.0), 10 * offset.x * offset.y) * vec4(vPosition,1.0);
	vec4 vPosition_local = vec4(0.5*vPosition_rotated.xy, vPosition_rotated.z, 1.0f) + texture_height_offset + vec4(offset.xy,0,0);
	gl_Position = proj * rotationMatrix(vec3(0.0f, 1.0f, 0.0f), 0.25) * rotationMatrix(vec3(1.0f, 0.0f, 0.0f), 2.15) * rotationMatrix(vec3(0.0f, 0.0f, 1.0f), 0.5 * sin(0.0005 * t) + 0.3) * vPosition_local;
}
