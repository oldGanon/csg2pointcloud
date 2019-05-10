string ComputeShaderSource = R"shader(
#version 460

layout(std430, binding = 2) buffer DISPATCH
{
    uint  num_groups_x;
    uint  num_groups_y;
    uint  num_groups_z;
} dispatch;

layout(std430, binding = 3) buffer BLOCKS
{
    uint read_index;
    uint write_index;
    uint padding[2];
    vec4 blocks[];
};

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;
void main(){
    float BlockDimension = 1.0f / gl_NumWorkGroups.x;
    vec3 BlockCenter = BlockDimension * (vec3(gl_WorkGroupID*2) - vec3(gl_NumWorkGroups-1));
    float CellDimension = BlockDimension / gl_WorkGroupSize.x;
    vec3 CellCenter = BlockCenter + CellDimension * (vec3(gl_LocalInvocationID*2) - vec3(gl_WorkGroupSize-1));

    uint BlockIndex = atomicAdd(write_index, 1);
    blocks[BlockIndex].xyz = CellCenter;
    blocks[BlockIndex].w = CellDimension;
    atomicMax(dispatch.num_groups_x, BlockIndex+1);
    dispatch.num_groups_y = 1;
    dispatch.num_groups_z = 1;
}
)shader";

string ComputeShaderSource2 = R"shader(
#version 460

struct sdf_shape
{
    vec4 shape;
    vec3 position;
    uint type;
};

layout(std430, binding = 1) buffer SDF
{
    uint edit_count;
    sdf_shape edits[];
};

layout(std430, binding = 2) buffer DISPATCH
{
    uint  num_groups_x;
    uint  num_groups_y;
    uint  num_groups_z;
} dispatch;

layout(std430, binding = 3) buffer BLOCKS
{
    uint read_index;
    uint write_index;
    vec4 blocks[];
};

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

float sdf_eval_sphere(sdf_shape shape, vec3 position)
{
    return length(shape.position - position) - shape.shape.x;
}

float sdf_eval(vec3 position)
{
    float d = intBitsToFloat(2139095039);
    for (uint i = 0; i < edit_count; i++)
    {
        sdf_shape shape = edits[i];
        switch(shape.type)
        {
            case 0: d = min(d, sdf_eval_sphere(shape, position));
            default: break;
        }
    }
    return d;
}

shared int new;
shared vec4 block;

void main(){
    if (gl_LocalInvocationID == 0)
    {
        new = -1;
        block = blocks[atomicAdd(read_index, 1)];
    }
    
    memoryBarrierShared();
    barrier();

    float dimension = 0.25 * block.w;
    vec3 position = block.xyz + dimension * (vec3(gl_LocalInvocationID*2) - vec3(gl_WorkGroupSize-1));
    if (sdf_eval(position) < dimension)
    {
        atomicAdd(new, 1);
        uint bockIndex = atomicAdd(write_index, 1);
        blocks[bockIndex].xyz = position;
        blocks[bockIndex].w = dimension;
    }

    memoryBarrierShared();
    barrier();

    if (gl_LocalInvocationID == 0)
    {
        atomicAdd(dispatch.num_groups_x, new);
        atomicExchange(dispatch.num_groups_y, 1);
        atomicExchange(dispatch.num_groups_z, 1);
    }
}
)shader";
