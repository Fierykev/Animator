#define SHADOW_BIAS .00f

uniform int shadowOn;

uniform sampler2D shadowmap;
uniform sampler2D tex;
uniform int usingTex;

uniform vec3 lightpos;
uniform vec3 lightdir;
uniform vec3 eyeVec;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float shininess;

uniform vec4 diffuseAmbient;
uniform vec4 globalAmbient;

uniform vec2 dim;

varying vec2 fs_texcoord;
varying vec4 fs_texshadow;

in vec3 fs_normal;

void main()
{
	//re-homogenize position
	vec3 texshadow = fs_texshadow.xyz / fs_texshadow.w;

	vec4 color = usingTex == 1 ?
		texture(tex, fs_texcoord) : diffuse;

	// don't illuminate something that the light does not touch
	if (1.f < abs(texshadow.x) || 1.f < abs(texshadow.y) || 1.f < abs(texshadow.z) || shadowOn == 0)
	{
		gl_FragColor = globalAmbient + diffuseAmbient * color;
		return;
	}

	//move the texture coords from (-1,1) to (0,1)
	texshadow.x = (texshadow.x + 1.0) / 2.0;
	texshadow.y = (texshadow.y + 1.0) / 2.0;

	// find the position in the shadow map of the current pixel
	float shadowMapDepth = texture(shadowmap, texshadow.xy).x;
	
	// if the pixel lies behind the area in the shadow map, it is in the shadow
	if (shadowMapDepth < texshadow.z + SHADOW_BIAS)
	{
		gl_FragColor  = globalAmbient + diffuseAmbient * color;
		return;
	}
	//gl_FragColor  = vec4(1,1,1,1);
	//	return;
	float sum = 0;
	int searcharea = 4; // 4 x 4

	for (int y = -searcharea / 2; y < searcharea / 2; y++)
	{
		for (int x = -searcharea / 2; x < searcharea / 2; x++)
		{
			sum += texture(shadowmap, vec2(texshadow.x + x * 1.f / dim.x,
			 texshadow.y + y  * 1.f / dim.y)).x < texshadow.z ? 0 : 1;
		}
	}
	
	sum /= searcharea * searcharea;
	
	// calculate the illumination at this point
	vec3 light = normalize(lightpos - fs_normal.xyz);
	float dotlight = dot(normalize(fs_normal), light);

	// calculate specular (need to fix)
	vec3 lDir = normalize(lightdir);
	vec3 norm = normalize(fs_normal);
	vec3 r = normalize(2 * dot(lDir, norm) * norm - lDir);
	float dp = dot(r, normalize(eyeVec));
	vec4 spec = specular * max(pow(dp, shininess), 0) * length(color);

	gl_FragColor = globalAmbient + diffuseAmbient * color + sum * dotlight * (color + spec);
}