#version 330 core  
out vec4 o_FragColor;

#define LIGHT_POS   vec3(2.0, -5.0, 3.0)
#define LIGHT_COLOR vec3(1.0, 1.0, 1.0)

#define NUM_PRIMITIVES 2
#define MAX_STEPS 100
#define MINIMUM_HIT_DISTANCE 0.001
#define MAXIMUM_TRACE_DISTANCE 1000.0

struct Primitives {
    vec4 color;
    float type; // 0: sphere, 1: cube, 2: cylinder
    mat4 matrix;
    mat4 inv_matrix;
};

in vec2 v_UV;
uniform vec3 u_ViewPos;
uniform vec2 u_Resolution;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform Primitives u_Primitives[NUM_PRIMITIVES];

#include <etugl/uniforms.glsl>
#include <etugl/phong_model.glsl>
#include <utils.glsl>

float get_scale(mat4 m) {
    float sx = length(m[0].xyz);
    float sy = length(m[1].xyz);
    float sz = length(m[2].xyz);
    return min(min(sx, sy), sz);
}

void main(void) { 
    vec2 uv = ((v_UV * 2.0) - 1.0) * vec2(u_Resolution.x / u_Resolution.y, 1.0);
    vec3 result = vec3(0.0);

    vec3 ray_origin = u_ViewPos;
    vec3 ray_direction = vec3(uv, -1.0);

    float total_distance = 0.0;
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 current_position = ray_origin + (ray_direction * total_distance);

        float min_scene_dist = MAXIMUM_TRACE_DISTANCE; 
        int candidate_idx = -1;

        for (int j = 0; j < NUM_PRIMITIVES; j++) {
            vec3 transformed_position = vec3(u_Primitives[j].inv_matrix * inverse(u_View) * vec4(current_position, 1.0));
            float dist_local = calculate_distance(transformed_position, 
                                                  vec3(0.0), 1.0, 1.0, 
                                                  u_Primitives[j].type);

            // float scale = get_scale(u_Primitives[j].inv_matrix);
            // float dist_world = dist_local * scale;

            if (dist_local < min_scene_dist) {
                min_scene_dist = dist_local;
                candidate_idx = j;
            }
        }

        if (min_scene_dist < MINIMUM_HIT_DISTANCE) {            
            // vec3 transformed_position = vec3(u_Primitives[candidate_idx].inv_matrix * vec4(current_position, 1.0));
            // vec3 object_space_normal = calculate_normal(transformed_position, 1.0, 1.0, u_Primitives[candidate_idx].type);

            // mat3 normalMatrix = transpose(mat3(u_Primitives[candidate_idx].inv_matrix));
            // vec3 world_space_normal = normalize(normalMatrix * object_space_normal);

            // vec3 light_dir = normalize(LIGHT_POS - current_position);
            // vec3 diffuse = compute_diffuse(world_space_normal, light_dir, LIGHT_COLOR);

            result = u_Primitives[candidate_idx].color.xyz;
            break;
        }

        if (total_distance > MAXIMUM_TRACE_DISTANCE)
            break;

        total_distance += min_scene_dist;
    }
    
    o_FragColor = vec4(result, 1.0);
}
