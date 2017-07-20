#pragma optimize (off)

#define M_STEPS 10
#define STEP_SIZE .01f
#define NEAR 1.0
#define FAR 100.0
#define EPSILON .01
#define NOISE_SIZE 40.f
#define FADE 2.f
#define NOISE_OPAC 20.f

uniform sampler2D depthTex;
uniform sampler3D volume;

uniform vec2 screenDim;

uniform mat4 invProj;
uniform mat4 modelView;
uniform vec3 eyeDir;

uniform vec4 octaveOff[4];

uniform vec4 lightPos = vec4( .97, 1.0, 0.8, 0.0 );

uniform mat4 worldViewProjection;

struct rsRet
{
	bool col;
	float near;
	float far;
};

varying vec3 fs_texCoord;
varying vec2 fs_screenPos;
varying vec2 fs_depth;
varying float fs_size;
varying vec3 fs_worldPos;
varying vec3 fs_color;
varying vec3 fs_origin;
varying float fs_life;

float max(float a, float b)
{
	return a > b ? a : b;
}

float min(float a, float b)
{
	return a < b ? a : b;
}

float saturate(float val)
{
	return max(min(val, 1.f), 0.f);
}

rsRet raySphereCol(vec3 pos, vec3 viewRay, vec3 spherePos, float radius, float near, float far)
{
	rsRet ret;
	ret.col = false;
	ret.far = 0;
	ret.near = 0;

	vec3 delta = pos - spherePos;

	float A = dot(viewRay, viewRay);
	float B = 2.f * dot(delta, viewRay);
	float C = dot(delta, delta) - radius * radius;

	float disc = B * B -  4.f * A * C;
	
	if (disc < EPSILON)
		return ret;
	
	// TODO: return near and far as well
	float sqrtDisc = sqrt(disc);
	ret.near = (-B - sqrtDisc) / (2.f * A);
	ret.far = (-B + sqrtDisc) / (2.f * A);
	ret.col = true;

	return ret;
}

vec4 noiseGen(vec3 coord, int octave)
{
	vec3 offset;
	float freq = 1, pers = 1;
	vec4 noise = vec4(0.f, 0.f, 0.f, 0.f);

	for (int i = 0; i < octave; i++)
	{
		offset = coord + octaveOff[i].xyz;
		vec4 val = texture(volume, offset);
		noise += pers * val;
		
		freq *= 3.f;
		pers *= .5f;
	}

	noise.a = abs(noise.a);

	return noise;
}

void main()
{
	vec2 screen = .5f * (fs_screenPos + vec2(1.f, 1.f));

	//textureLod(depthTex, screen * screenDim, 0).x;
	float fs_depth = texture(depthTex, screen).x;

	vec2 screenPos = vec2(fs_screenPos.x, fs_screenPos.y);
	
	vec4 depthView = invProj * vec4(screenPos, fs_depth * 2.f - 1.f, 1.f);
	float sampleDepth = -depthView.z / depthView.w;

	float near;
	float far;

	rsRet ret = raySphereCol(fs_worldPos, eyeDir, fs_origin, fs_size, near, far);
	near = ret.near;
	far = ret.far;

	if (!ret.col)
		discard;

	vec3 wNear = fs_worldPos + eyeDir * near;
	vec3 wFar = fs_worldPos + eyeDir * far;
	vec4 vNear = modelView * vec4(wNear, 1.f);
	vec4 vFar = modelView * vec4(wFar, 1.f);
	float nearDepth = -vNear.z / vNear.w;
	float farDepth = -vFar.z / vFar.w;
	float power = fs_texCoord.z;
	float deltaDepth = farDepth - sampleDepth;

	if (0.f < deltaDepth)
	{
		far -= deltaDepth;
		
		// for col reasons
		if (far < near)
		{
			discard;
		}
		
		wFar = fs_worldPos.xyz + eyeDir * vFar.xyz;
		farDepth = sampleDepth;
	}

	vec3 uTex =  (wNear - fs_origin) / fs_size;

	float noiseAdjustment = 1.f / NOISE_SIZE;
	vec3 texNear = wNear * noiseAdjustment;
	vec3 texFar = wFar * noiseAdjustment;

	int numSteps = int(length(texFar - texNear) / STEP_SIZE);
	numSteps = min(numSteps, M_STEPS - 2) + 2;

	vec3 currentTexDelta = (texFar - texNear) / float(numSteps - 1);
	float depthDelta = (farDepth - nearDepth) / float(numSteps - 1);

	float netOpacity = 0;
	vec4 netLight = vec4(0, 0, 0, 0);

	for (int i = 0; i < numSteps; i++)
	{
		vec4 noise = noiseGen(texNear, 4);
		noise.xyz += normalize(uTex);
		noise.xyz = normalize(noise.xyz);

		// fade edges
		float fade = 1.f;

		// soft particle
		fade = saturate((sampleDepth - nearDepth) / FADE);

		float lenSquared = dot(uTex, uTex);
		float falloff = 1.f - lenSquared;

		float opacity = noise.a * falloff * fade;

		// sum opacity
		netOpacity += opacity;
		
		// lighting
		vec4 light = lightPos * saturate(dot(noise.xyz, vec3(0, 1, 0)) * .5 + .5);

		// add to net light
		netLight += light;
		
		// sum everything else
		texNear += currentTexDelta;
		uTex += currentTexDelta;
		nearDepth += depthDelta;
	}

	//(numSteps - 1.f)
	vec4 color =
		vec4(fs_color, 1) * (netLight / float(numSteps)) * .8f + .2f;

	float opacity = saturate(netOpacity * NOISE_OPAC / float(numSteps)) * power;

	gl_FragColor = vec4(color.xyz, opacity);
}
