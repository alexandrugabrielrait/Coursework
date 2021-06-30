#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 v_color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float deformation;

out vec3 frag_color;
out vec3 frag_normal;

float modDivisor = 300.0;

float modFloat(float x){return x - floor(x * (1.0 / modDivisor)) * modDivisor;}
vec4 modFloat(vec4 x){return x - floor(x * (1.0 / modDivisor)) * modDivisor;}
vec4 perm(vec4 x){return modFloat(((x * 34.0) + 1.0) * x);}

// noise generation function
float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

void main()
{
	frag_color = v_color;
	frag_normal = v_normal;
	
	vec3 N = normalize(mat3(Model) * v_normal);
	vec4 P;
    float coordSum = abs(N.x) + abs(N.y) + abs(N.z);
    float x = noise(vec3(coordSum));

    if (v_normal.y <= 0)
	    P = vec4(v_position * (1 - 0.5 * deformation) + 0.75 * deformation * N * x, 1);
    else
	    P = vec4(v_position * (1 - 0.5 * deformation) + 0.75 * deformation * N * pow(x, 4), 1);

	gl_Position = Projection * View * Model * P;
}