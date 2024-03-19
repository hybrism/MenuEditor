#include "../common/common.hlsli"

Texture2D aMainTexture : register(t0);

PixelOutput main(SpritePixelInputType input)
{
    PixelOutput result;
    float4 diffuse = aMainTexture.Sample(aDefaultSampler, input.uv) * input.color;
	
    if (diffuse.a < 0.1f)
        discard;
    
	//IF we want to use lighting on sprites, check TGE! They have some cool stuff :)
    
    // 0-1 values only
    // x = left to right, y = right to left, z = down to up, w = up to down
    float4 clip = float4(input.clip);
    
    diffuse.a =
    step(clip.x, input.uv.x) *
    step(input.uv.x, 1 - clip.y) *
    step(input.uv.y, 1 - clip.z) *
    step(clip.w, input.uv.y) * diffuse.a;
    
    result.color.rgb = diffuse.rgb;
    result.color.a = diffuse.a;
    	
    return result;
}