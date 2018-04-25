#pragma once

// thin wrapper around a primitive batch
class DebugRenderer
{
public:
    bool initialize();
    void reset();

    void draw_line(const Vec3 &p0, const Vec3 &p1, const Color &color);
    void draw_arrow(const Vec3 &head, const Vec3 &tail, const Color &color);

    void draw();

private:
    std::vector<DirectX::VertexPositionColor> buffer;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> batch;
    std::unique_ptr<DirectX::BasicEffect> effect;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout;
};