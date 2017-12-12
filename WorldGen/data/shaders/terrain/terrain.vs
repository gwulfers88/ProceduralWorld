// Vertex Code
uniform vec3 terrain_offset;
out VS_OUT
{
	vec2 tc;
} vs_out;

void main(void)
{
	const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),
								   vec4( 0.5, 0.0, -0.5, 1.0),
								   vec4(-0.5, 0.0,  0.5, 1.0),
								   vec4( 0.5, 0.0,  0.5, 1.0));

	int x = gl_InstanceID & 63;
	int y = gl_InstanceID >> 6;
	vec2 offs = vec2(x, y);

	vs_out.tc = ((vertices[gl_VertexID].xz + offs + vec2(0.5)) / 64.0);
	
	gl_Position = vertices[gl_VertexID] + vec4(float(x - 32), 0.0,
											   float(y - 32), 0.0) + vec4(terrain_offset, 0);
}