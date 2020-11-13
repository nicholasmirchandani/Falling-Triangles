#version 450 core

layout(location = 0) in vec4 color_vert;
uniform int paused;
uniform float speedMult;

layout(location = 0) out vec4 color;

void main() {
	float average_color = (color_vert.r + color_vert.g + color_vert.b) / 3.0f;
	color = paused == 0 ? color_vert * sqrt(speedMult) : 0.4  * vec4(average_color, average_color, average_color, 1.0) * sqrt(speedMult);
}