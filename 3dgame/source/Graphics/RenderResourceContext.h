#pragma once

#include <d3d11.h>
#include <wrl.h>
//#include "..//Component/SystemGbuffer.h"

class RenderResourceContext
{
public:
   Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view{};
   Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view{};
   Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view{};

   //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gbuffer_shader_resource_view[GBufferId::GB_Max]{};
};