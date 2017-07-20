uniform mat4 worldViewProjection;
uniform mat4 modelLightViewProjection;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

varying vec2 fs_texcoord;
varying vec4 fs_texshadow;

out vec3 fs_normal;

void main()
{
	gl_Position = worldViewProjection * vec4(position, 1);
	fs_normal = normal;
	fs_texcoord = texcoord;

	fs_texshadow = modelLightViewProjection * vec4(position, 1);
}
