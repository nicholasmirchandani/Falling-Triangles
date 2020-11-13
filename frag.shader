#version 450 core

layout(location = 0) in vec4 color_vert;
uniform int paused;
uniform float speedMult;

layout(location = 0) out vec4 color;

void main() {
	color = paused == 0 ? color_vert * sqrt(speedMult) : vec4(0.4, 0.4, 0.4, 1.0) * sqrt(speedMult);
}