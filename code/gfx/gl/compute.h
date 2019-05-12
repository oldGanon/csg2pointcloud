
string VoxelComputeShaderSource = R"shader(
#version 460

const float infinity = intBitsToFloat(2139095039);

struct sdf_edit
{
    vec4 params;
    vec3 position;
    float smoothing;
    uint shapeop;
};

layout(std430, binding = 1) buffer SDF
{
    uint edit_count;
    sdf_edit edits[];
};

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

float sum(vec3 v)
{
    return v.x + v.y + v.z;
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
    return min(d0,-d1)+e*e*0.25/r;
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

float sdf_max_sphere(sdf_edit shape, vec3 position)
{
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

    return d;
}

float sdf_max_cuboid(sdf_edit shape, vec3 position)
{
    position = abs(position) - shape.params.xyz;
    return max(max(position.x, position.y), position.z);
}

float sdf_max(vec3 position)
{
    float d0 = infinity;
    for (uint i = 0; i < edit_count; i++)
    {
        float d1 = infinity;
        sdf_edit edit = edits[i];
        switch(edit.shapeop / 4)
        {
            case 0: d1 = min(d1, sdf_max_sphere(edit, position)); break;
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

shared vec4 block;

void main(){
    if (gl_LocalInvocationID == 0)
    {
        atomicAdd(dispatch.num_groups_x, -1);
        block = blocks[atomicAdd(read_index, 1)];
    }
    
    memoryBarrierShared();
    barrier();

    float dimension = 0.25 * block.w;
    vec3 position = block.xyz + dimension * (vec3(gl_LocalInvocationID*2) - vec3(gl_WorkGroupSize-1));
    if (abs(sdf_max(position)) < dimension)
    {
        atomicAdd(dispatch.num_groups_x, 1);
        uint bockIndex = atomicAdd(write_index, 1);
        blocks[bockIndex].xyz = position;
        blocks[bockIndex].w = dimension;
    }

    memoryBarrierShared();
    barrier();
}
)shader";

string SplatComputeShaderSource = R"shader(
#version 460

layout(std430, binding = 2) buffer BLOCKS
{
    uint read_index;
    uint write_index;
    vec4 blocks[];
};

struct vertex
{
    vec3 position;
    vec3 normal;
    vec3 color;
};

layout(std430, binding = 2) buffer VERTEX
{
    uint vertex_count;
    vertex vertices[];
};

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main(){
    block = blocks[atomicAdd(read_index, 1)];
    vertex = atomicAdd(vertex_count, 1);
    vertices[vertex].position = block.xyz;
    vertices[vertex].normal = block.xyz;
    vertices[vertex].color = block.xyz;
}
)shader";
