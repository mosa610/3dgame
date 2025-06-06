#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include "..\Graphics\Sprite.h"

struct ComponentIBL
{
	ComponentIBL(const wchar_t* diffuse_iem_file_name = L".\\resources\\country_club_4k\\diffuse_iem.dds",
		const wchar_t* specular_pmrem_file_name = L".\\resources\\country_club_4k\\specular_pmrem.dds",
		const wchar_t* lut_ggx_file_name = L".\\resources\\lut_ggx.dds")
		: diffuse_iem_file_name(diffuse_iem_file_name),
		specular_pmrem_file_name(specular_pmrem_file_name),
		lut_ggx_file_name(lut_ggx_file_name) {}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuse_iem_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specular_pmrem_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> lut_ggx_shader_resource_view;

	std::unique_ptr<Sprite> skymap_sprite;

public:
	const wchar_t* diffuse_iem_file_name = L".\\resources\\country_club_4k\\diffuse_iem.dds";
    const wchar_t* specular_pmrem_file_name = L".\\resources\\country_club_4k\\specular_pmrem.dds";
    const wchar_t* lut_ggx_file_name = L".\\resources\\lut_ggx.dds";
};