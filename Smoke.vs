#pragma optimize (off)

uniform mat4 invView;
uniform mat4 worldViewProjection;
uniform sampler1D colorTex;

in vec4 loc;
in vec4 pos;
in float size;
in vec2 texCoord;
in float life;
in vec3 origin;

varying vec3 fs_texCoord;
varying vec2 fs_screenPos;
varying vec2 fs_depth;
varying float fs_size;
varying vec3 fs_worldPos;
varying vec3 fs_color;
varying vec3 fs_origin;
varying float fs_life;

void main()
{
	vec3 position_3x3 = loc.xyz * size;
	position_3x3 = mat3(invView) * position_3x3 + pos.xyz;
	vec4 position = worldViewProjection * vec4(position_3x3.xyz, 1);
	gl_Position = position;

	fs_texCoord = vec3(texCoord, life);

	fs_screenPos = position.xy / position.w;

	fs_depth = position.zw;

	fs_size = size;

	fs_worldPos = position_3x3;
	
	fs_color = texture(colorTex, 1.f - life).xyz;

	fs_origin = pos.xyz;

	fs_life = life;
}
