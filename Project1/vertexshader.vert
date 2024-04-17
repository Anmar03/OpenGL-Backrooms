#version 430 core

// Uniform inputs
uniform mat4 mv;
uniform mat4 projection;
uniform vec3 light_pos;

// Per-vertex inputs
in vec3 position;
in vec3 normal;
in vec2 uv;


// Outputs
out VS_OUT {
vec3 N;
vec3 L;
vec3 V;
} vs_out;

out vec2 UV;

void main()
{
    //gl_Position = mvp * vec4(position, 1.0);

    // calculate view-space coords
    vec4 P = mv * vec4(position, 1.0);

    // calculate normals
    vs_out.N = mat3(mv) * normal;

    vs_out.L = light_pos - P.xyz;

    // calculate view vector
    vs_out.V = -P.xyz;

    gl_Position = projection * P;

    UV = uv;
}
