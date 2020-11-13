#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color_in;

layout(location = 0) out vec4 color_vert;

void main() {
	gl_Position = position;
	color_vert = color_in;
}