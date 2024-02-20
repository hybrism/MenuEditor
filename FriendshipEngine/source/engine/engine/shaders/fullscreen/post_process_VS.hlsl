#include "../common/Common.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType value;
    
    value.position = input.position;
    
 
    value.uv = input.uv;
    
    return value;
}