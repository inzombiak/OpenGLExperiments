#version 430 core

layout(triangles) in;
layout(points, max_vertices = 3) out;

in vec4 tes_color[];
out vec4 geo_color;

void main(void)
{
	int i;

	for (i = 0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		geo_color = tes_color[i];
		EmitVertex();
	}
}