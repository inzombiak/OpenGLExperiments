#version 430

in vec4 vs_color;

out vec4 color;

void main()
{
	color = vec4(0.0f, 0.8f, 1.0f, 1.0f);
	//color = vs_color;
}