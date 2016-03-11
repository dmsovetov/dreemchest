[VertexShader]
#version 120

uniform mat4 u_vp, u_transform;

void main() {
	gl_Position = u_vp * u_transform * gl_Vertex;
}

[FragmentShader]
#version 120

uniform vec4 u_color;

void main() {
	gl_FragColor = u_color;
}