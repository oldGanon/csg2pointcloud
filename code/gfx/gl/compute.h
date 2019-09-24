const char *GLSL_Version = "#version 460\n";

const char *SDFShaderLib = R"lib(
const float infinity = intBitsToFloat(2139095039);

struct sdf_edit
{
    vec4 params;
    vec4 position;
    vec4 rotation;
    vec4 color;
    float smoothing;
    uint shapeop;
    float rotationscale;
    float influence;
};

layout(std430, binding = 1) readonly buffer SDF
{
    uint edit_count;
    sdf_edit edits[];
};

float sum(vec3 v)
{
    return v.x + v.y + v.z;
}

float hmax(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

vec3 rotate(vec4 q, vec3 v)
{
    vec3 t = cross(2.0*q.xyz,v);
    return v + q.w*t + cross(q.xyz,t);
}

vec3 invrotate(vec4 q, vec3 v)
{
    vec3 t = cross(2.0*q.xyz,v);
    return v - q.w*t + cross(q.xyz,t);
}

float sdf_range(sdf_edit edit, vec3 position)
{
    return hmax(abs(position - edit.position.xyz)) - edit.influence;
}

float sdf_add(float d0, float d1)
{
    return min(d0, d1);
}

float sdf_sub(float d0, float d1)
{
    return max(d0,-d1);
}

float sdf_smoothadd(float d0, float d1, float r)
{
    float e = max(r-abs(d0-d1),0);
    return min(d0,d1)-e*e*0.25/r;
}

float sdf_smoothsub(float d0, float d1, float r)
{
    float e = max(r-abs(d0+d1),0);
    return max(d0,-d1)+e*e*0.25/r;
}

float quadratic(float a, float b, float c)
{
    float m = b*b-4.0*a*c;
    if (m < 0.0) return infinity;
    float r0 = -b/(2.0*a);
    float r1 = sqrt(m)/(2.0*a);
    float r = min(abs(r0-r1),abs(r0+r1));
    return r;
}

float sdf_max_ellipsoid(sdf_edit shape, vec3 position)
{
    position = invrotate(shape.rotation, position - shape.position.xyz);
    vec3 c = abs(position);
    vec3 cc = c*c;
    vec3 r = shape.params.xyz;
    vec3 rr = r*r;
    float d = infinity;

    vec3 qa = 1.0/rr;
    vec3 qb = -2.0*c/rr;
    vec3 qc = cc/rr;

    float qcs = sum(qc) - 1.0;
    d = min(d, quadratic(sum(qa), sum(qb), qcs));
    if (qcs <= 0.0) return -d;

    d = min(d, max(quadratic(qa.y + qa.z, qb.y + qb.z, qc.y + qc.z - 1.0), c.x));
    d = min(d, max(quadratic(qa.x + qa.z, qb.x + qb.z, qc.x + qc.z - 1.0), c.y));
    d = min(d, max(quadratic(qa.x + qa.y, qb.x + qb.y, qc.x + qc.y - 1.0), c.z));

    d = min(d, max(max(c.x, c.y), abs(c.z - r.z)));
    d = min(d, max(max(c.x, abs(c.y - r.y)), c.z));
    d = min(d, max(max(abs(c.x - r.x), c.y), c.z));

    return d * shape.rotationscale;
}

float sdf_max_cuboid(sdf_edit shape, vec3 position)
{
    position = invrotate(shape.rotation, position - shape.position.xyz);
    vec3 d = abs(position) - shape.params.xyz;
    return hmax(d) * shape.rotationscale;
}

float sdf_max(vec3 position, float dim)
{
    float d0 = infinity;
    for (uint i = 0; i < edit_count; i++)
    {
        sdf_edit edit = edits[i];
        if (sdf_range(edit, position) > dim) continue;

        float d1 = infinity;
        switch(edit.shapeop / 4)
        {
            case 0: d1 = min(d1, sdf_max_ellipsoid(edit, position)); break;
            case 1: d1 = min(d1, sdf_max_cuboid(edit, position)); break;
            default: break;
        }
        switch(edit.shapeop % 4)
        {
            case 0: d0 = sdf_add(d0, d1); break;
            case 1: d0 = sdf_sub(d0, d1); break;
            case 2: d0 = sdf_smoothadd(d0, d1, edit.smoothing); break;
            case 3: d0 = sdf_smoothsub(d0, d1, edit.smoothing); break;
            default: break;
        }
    }
    return d0;
}

float sdf_eval_ellipsoid(sdf_edit shape, vec3 position)
{
    position = abs(invrotate(shape.rotation, position - shape.position.xyz));
    vec3 r = shape.params.xyz;
    float k0 = length(position/r);
    float k1 = length(position/(r*r));
    return k0*(k0-1.0f)/k1;
}

float sdf_eval_cuboid(sdf_edit shape, vec3 position)
{
    position = abs(invrotate(shape.rotation, position - shape.position.xyz));
    vec3 d = abs(position) - shape.params.xyz;
    return length(max(d,0.0)) + min(hmax(d),0.0);
}

float sdf_eval_edit(vec3 position, sdf_edit edit, float d0)
{
    float d1 = infinity;
    switch(edit.shapeop / 4)
    {
        case 0: d1 = min(d1, sdf_eval_ellipsoid(edit, position)); break;
        case 1: d1 = min(d1, sdf_eval_cuboid(edit, position)); break;
        default: break;
    }
    switch(edit.shapeop % 4)
    {
        case 0: d0 = sdf_add(d0, d1); break;
        case 1: d0 = sdf_sub(d0, d1); break;
        case 2: d0 = sdf_smoothadd(d0, d1, edit.smoothing); break;
        case 3: d0 = sdf_smoothsub(d0, d1, edit.smoothing); break;
        default: break;
    }
    return d0;
}

float sdf_eval(vec3 position)
{
    float d0 = infinity;
    for (uint i = 0; i < edit_count; i++)
    {
        sdf_edit edit = edits[i];
        if (sdf_range(edit, position) > 0.001) continue;
        d0 = sdf_eval_edit(position, edit, d0);
    }
    return d0;
}

vec3 sdf_normal(vec3 position)
{
    const float h = 0.0001;
    const vec2 k = vec2(1,-1);
    return normalize(k.xyy*sdf_eval(position+k.xyy*h) +
                     k.yyx*sdf_eval(position+k.yyx*h) +
                     k.yxy*sdf_eval(position+k.yxy*h) +
                     k.xxx*sdf_eval(position+k.xxx*h));
}

vec4 sdf_color(vec3 position)
{
    float d0 = infinity;
    sdf_edit edit = edits[0];
    vec4 c0 = edit.color;    
    for (uint i = 0; i < edit_count; i++)
    {
        sdf_edit edit = edits[i];
        if (sdf_range(edit, position) > 0.001) continue;
        float d1 = sdf_eval_edit(position, edit, d0);
        float c1 = clamp((d0-d1)/edit.smoothing,0,1);
        c0 = mix(c0, edit.color, c1);
        d0 = d1;
    }
    return c0;
}

)lib";

const char *VoxelComputeShaderSource = R"shader(
layout(std430, binding = 2) buffer BLOCKS
{
    uint read_index;
    uint write_index;
    vec4 blocks[];
};

layout(std430, binding = 3) buffer DISPATCH
{
    uint num_groups_x;
    uint num_groups_y;
    uint num_groups_z;
} dispatch;

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

shared vec4 block;

void main(){
    if (gl_LocalInvocationIndex == 0)
    {
        atomicAdd(dispatch.num_groups_x, -1);
        block = blocks[atomicAdd(read_index, 1)];
    }
    
    memoryBarrierShared();
    barrier();

    float dimension = 0.25 * block.w;
    vec3 position = block.xyz + dimension * (vec3(gl_LocalInvocationID*2) - vec3(gl_WorkGroupSize-1));
    if (abs(sdf_max(position, dimension)) < dimension)
    {
        atomicAdd(dispatch.num_groups_x, 1);
        uint bockIndex = atomicAdd(write_index, 1);
        blocks[bockIndex].xyz = position;
        blocks[bockIndex].w = dimension;
    }
}
)shader";

const char *SplatComputeShaderSource = R"shader(
layout(std430, binding = 2) readonly buffer BLOCKS
{
    uint read_index;
    uint write_index;
    vec4 blocks[];
};

struct splat
{
    uint positionx;
    uint positiony;
    uint positionz;
    uint normal;
    uint color;
};

layout(std430, binding = 4) buffer SPLAT
{
    splat splats[];
};

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

vec3 hash(vec3 x)
{
    uvec3 ux = uvec3(ivec3(floor(x)));
    const uint k = 1103515245U;
    ux = ((ux>>8U)^ux.yzx)*k;
    ux = ((ux>>8U)^ux.yzx)*k;
    ux = ((ux>>8U)^ux.yzx)*k;
    return vec3(ux)*(1.0/float(0xffffffffU));
}

uint normalpack(vec3 normal)
{
    uint x = (int(normal.x * 511.0) & 1023);
    uint y = (int(normal.y * 511.0) & 1023) << 10;
    uint z = (int(normal.z * 511.0) & 1023) << 20;
    return x|y|z;
}

uint colorpack(vec4 color)
{
    uint r = uint(color.r * 255.0);
    uint g = uint(color.g * 255.0) << 8;
    uint b = uint(color.b * 255.0) << 16;
    uint a = uint(color.a * 255.0) << 24;
    return r|g|b|a;
}

void main(){
    uint index = gl_GlobalInvocationID.x;
    vec4 block = blocks[read_index + index];
    float dist = sdf_eval(block.xyz);
    vec3 normal = sdf_normal(block.xyz);
    vec3 position = block.xyz - normal * dist;
    normal = normalize(hash(block.xyz*0xfffffffU) * 0.25 + normal);
    splats[index].positionx = floatBitsToInt(position.x);
    splats[index].positiony = floatBitsToInt(position.y);
    splats[index].positionz = floatBitsToInt(position.z);
    splats[index].normal = normalpack(normal);
    splats[index].color = colorpack(sdf_color(block.xyz));
}
)shader";
