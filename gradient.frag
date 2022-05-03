uniform vec2 screen_res;
uniform vec2 p_center;
uniform vec4 p_color;
uniform float p_radius;

float dist(vec2 a, vec2 b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void main()
{
    vec2 center_coords = vec2(p_center.x, screen_res.y - p_center.y);
    vec2 frag_coords = vec2(gl_FragCoord.x - 0.5, gl_FragCoord.y - 0.5);

    float d = dist(frag_coords, center_coords) / p_radius;
    vec4 center_color = p_color;
    vec4 outside_color = vec4(center_color.rgb, 0.0);

    gl_FragColor = mix(center_color, outside_color, d);
}