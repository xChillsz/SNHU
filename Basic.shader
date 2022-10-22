#shader vertex
#version 440 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 position;

void main()
{
	gl_Position = position;
}

#shader fragment
#version 440 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(0.0, 0.0, 1.0, 1.0);
}