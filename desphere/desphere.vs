in vec2 position;

varying vec2 center;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    center = uv;
    gl_Position = proj * view * model * vec4(position, 0.0, 1.0);
}
