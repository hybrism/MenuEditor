#include "../common/common.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    result.color.rgb = float3(1, 1, 1);
    result.color.a = 1;
    
    return result;
}
