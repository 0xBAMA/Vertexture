#version 330
varying  vec4 color;
varying  vec4 norm;
uniform sampler2D point_sprite;

void main() {
	gl_FragColor = color;
	vec4 tref = texture(point_sprite,gl_PointCoord.xy);
	gl_FragColor *= tref.r;
	if(gl_FragColor.a < 0.4)
		discard;

	gl_FragColor.a = 1.0;
	gl_FragDepth = gl_FragCoord.z - 0.01*(tref.r-0.5);
	gl_FragColor.rgb *= 1-dot(vec3(gl_PointCoord-vec2(0.5), tref.r-0.5), vec3(1.0));

	//fcymod3 is used to draw something along the lines of scanlines
	int fcxmod2 = int(gl_FragCoord.x) % 2;
	int fcymod2 = int(gl_FragCoord.y) % 2;

	if((fcymod2 == 1) || (fcxmod2 == 1)) {
		// discard;
		gl_FragColor.r = 0;
	}
}
