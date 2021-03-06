#version 410 core

layout(triangles, equal_spacing, cw) in;

patch in vec4 patch_color;
out vec4 _color;

void main(void)
{
	gl_Position = (	gl_TessCoord.x * gl_in[0].gl_Position +
					gl_TessCoord.y * gl_in[1].gl_Position +
					gl_TessCoord.z * gl_in[2].gl_Position);
	_color = patch_color;
}