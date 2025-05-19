 //BLOOM
cbuffer BLOOM_CONSTANT_BUFFER : register(b8)
{
    float bloom_extraction_threshold;
    float bloom_intensity;
    float2 something;
};

//cbuffer PARAMETRIC_CONSTANT_BUFFER : register(b8)
//{
//    float bloom_extraction_threshold;
//    float gaussian_sigma;
//    float bloom_intensity;
//    float exposure;
//    float extraction_end;
//    float2 something;
//    float pad;
//};