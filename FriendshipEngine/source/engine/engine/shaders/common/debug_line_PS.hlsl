#include "../vfx/vfx_common.hlsli"

PixelOutput main(ParticlePixelInputType input)
{
    PixelOutput result;

    result.color = input.color;
    return result;
}