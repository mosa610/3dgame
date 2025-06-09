#pragma once

struct ComponentBloom
{
	ComponentBloom() : bloom_extraction_threshold(1.0f), bloom_intensity(1.0f) {}
	float bloom_extraction_threshold;
	float bloom_intensity;
	float something[2];
};