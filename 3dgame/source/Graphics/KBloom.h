#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include "framebuffer.h"
#include "fullscreen_quad.h"

class Bloom
{
public:
    Bloom();
    ~Bloom();
    
    void Begin();

    void End();

    void Draw(ID3D11ShaderResourceView* srv);

public:
    std::unique_ptr<framebuffer> framebuffers[8];
    std::unique_ptr<fullscreen_quad> bit_block_transfer;
};

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ResizeSRV(ID3D11ShaderResourceView* srv, UINT Reduction_ratio);