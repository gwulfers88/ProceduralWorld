// Fragment Code
out vec4 color;

layout (binding = 1) uniform sampler2D tex_dirt;
layout (binding = 2) uniform sampler2D tex_grass;
layout (binding = 3) uniform sampler2D tex_rock;
layout (binding = 4) uniform sampler2D tex_snow;

uniform bool enable_fog = true;
uniform vec4 fog_color = vec4(0.2f, 0.5f, 0.9f, 0.0f);

in TES_OUT
{
	vec2 tc;
	vec3 world_coord;
	vec3 eye_coord;
	vec3 normal;
	vec3 lightDir;
} fs_in;

vec4 fog(vec4 c)
{
	float z = length(fs_in.eye_coord);

	float de = 0.025 * smoothstep(0.0, 6.0, 10.0 - fs_in.world_coord.y);
	float di = 0.045 * (smoothstep(0.0, 40.0, 20.0 - fs_in.world_coord.y));

	float extinction   = exp(-z * de);
	float inscattering = exp(-z * di);

	return c * extinction + fog_color * (1.0 - inscattering);
}

vec3 ambient = vec3(0.0, 0.0, 0.0);
vec4 lightColor = vec4(1, 1, 1, 0);

void main(void)
{
	float scale = 32.0;

	vec3 lightDir = normalize(lightColor.rgb);
	vec3 normal = normalize(fs_in.normal);
	float intensity = clamp(dot(lightDir, normal), 0.0, 1.0);
	vec4 lightDiffuse = vec4(lightColor.rgb * intensity, 1);

	vec4 landscape = texture(tex_dirt, fs_in.tc*scale)*0.25;
	
	if(fs_in.world_coord.y < 25*0.25)
	{
		landscape += texture(tex_dirt, fs_in.tc*scale); // Dirt
	}
	else if(fs_in.world_coord.y >= 25*0.35 && fs_in.world_coord.y <= 25*0.5)
	{
		landscape += texture(tex_grass, fs_in.tc*scale); // Grass
	}
	else if(fs_in.world_coord.y > 25*0.5 && fs_in.world_coord.y <= 25*0.75)
	{
		landscape += texture(tex_rock, fs_in.tc*scale); // Rock
	}
	else if(fs_in.world_coord.y > 25*0.75)
	{
		landscape += texture(tex_snow, fs_in.tc*scale); // Snow
	}

	if (enable_fog)
	{
		color = vec4(fog(landscape).rgb, 1) * lightDiffuse;
	}
	else
	{
		color = vec4(landscape.rgb, 1) * lightDiffuse;
	}
}