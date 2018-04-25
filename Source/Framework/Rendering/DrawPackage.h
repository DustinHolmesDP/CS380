#pragma once

namespace DirectX
{
    class CommonStates;
    class BasicEffect;

    namespace SimpleMath
    {
        struct Matrix;
    }
}

struct ID3D11DeviceContext1;

struct DrawPackage
{
    ID3D11DeviceContext1 *context;
    DirectX::CommonStates *states;
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix projection;
};