#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "..//misc.h"

template<class T>
class ConstantBuffer
{
private:
    ConstantBuffer(const ConstantBuffer<T>& rhs);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
    ID3D11DeviceContext* _deviceContext = nullptr;

public:
    ConstantBuffer() {}
    T _data;

    ID3D11Buffer* Get()const
    {
        return _buffer.Get();
    }

    ID3D11Buffer* const* GetAddressOf()const
    {
        return _buffer.GetAddressOf();
    }

    HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    {
        if (_buffer.Get() != nullptr)
            _buffer.Reset();

        _deviceContext = deviceContext;

        D3D11_BUFFER_DESC desc;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;
        desc.ByteWidth = static_cast<UINT>(sizeof(T) + /*padding*/(16 - (sizeof(T) % 16)));

        HRESULT hr = device->CreateBuffer(&desc, 0, _buffer.GetAddressOf());
        return hr;
    }

    void Update()
    {
        _deviceContext->UpdateSubresource(_buffer.Get(), 0, nullptr, &_data, 0, 0);
    }
};

template<class T>
class StracturedBuffer {
private:
    // コピー禁止
    StracturedBuffer(const StracturedBuffer<T>& rhs) = delete;
    StracturedBuffer& operator=(const StracturedBuffer<T>& rhs) = delete;

    Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
    ID3D11DeviceContext* _deviceContext = nullptr;
    size_t _elementCount = 0;

public:
    StracturedBuffer() {}

    std::vector<T> _data;

    // StructuredBufferの作成
    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, size_t elementCount) {
        if (!device || !context) return false;

        _deviceContext = context;
        _elementCount = elementCount;
        _data.resize(elementCount);

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(T) * elementCount;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = sizeof(T);

        HRESULT hr = device->CreateBuffer(&desc, nullptr, _buffer.ReleaseAndGetAddressOf());
        return SUCCEEDED(hr);
    }

    // StructuredBuffer のデータを更新
    void Update() {
        if (!_buffer || _data.empty()) return;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = _deviceContext->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return;

        memcpy(mappedResource.pData, _data.data(), sizeof(T) * _data.size());
        _deviceContext->Unmap(_buffer.Get(), 0);
    }

    // バッファを解放
    void Release() {
        _buffer.Reset();
        _data.clear();
        _deviceContext = nullptr;
        _elementCount = 0;
    }

    // バッファのポインタを取得
    ID3D11Buffer* Get() const { return _buffer.Get(); }
    ID3D11Buffer* const* GetAddressOf() const { return _buffer.GetAddressOf(); }
};