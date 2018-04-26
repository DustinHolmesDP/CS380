#pragma once
#include <vector>
#include "Misc/NiceTypes.h"
#include "Rendering/MeshRenderer.h"

// forward declarations
class MeshRenderer;
class Terrain;

template<typename T>
class MapLayer
{
    friend class Terrain;
public:
    MapLayer(const char *name, float height) : data(nullptr), yHeight(height), name(name),
        height(-1), width(-1), allocHeight(-1), allocWidth(-1), enabled(false), config()
    {}

    const T &get_value(int row, int col) const;
    const T &get_value(const GridPos &gridPos) const;

    void set_value(int row, int col, const T &value);
    void set_value(const GridPos &gridPos, const T &value);

    bool is_enabled() const { return enabled; }
    void set_enabled(bool state) { enabled = state; }
    void toggle_enabled() { enabled = !enabled; }

    void for_each(std::function<void(T &)> op);
private:

    T **data;
    float yHeight;
    const std::string name;
    int height;
    int width;
    int allocHeight;
    int allocWidth;
    bool enabled;

    union Config
    {
        Config() : alpha(0.0f) {}
        Color colors[2];
        float alpha;
    } config;

    void configure_float(const Color &posColor, const Color &negColor);
    void configure_bool(const Color &falseColor, const Color &trueColor);
    void configure_color(float alpha);

    void draw(MeshRenderer &instancer, const std::vector<std::vector<Vec3>> &positions);
    void draw_cell(MeshRenderer &instancer, size_t row, size_t col, const Vec3 &pos);

    void populate_with_value(int height, int width, const T &value);
    void populate_with_data(const std::vector<std::vector<T>> &data);

    void free_memory();
    void allocate_memory();
};

template<typename T>
inline const T &MapLayer<T>::get_value(int row, int col) const
{
    return data[row][col];
}

template<typename T>
inline const T &MapLayer<T>::get_value(const GridPos & gridPos) const
{
    return data[gridPos.row][gridPos.col];
}

template<typename T>
inline void MapLayer<T>::set_value(int row, int col, const T &value)
{
    data[row][col] = value;
}

template<typename T>
inline void MapLayer<T>::set_value(const GridPos &gridPos, const T &value)
{
    data[gridPos.row][gridPos.col] = value;
}

template<typename T>
inline void MapLayer<T>::for_each(std::function<void(T &)> op)
{
    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
            op(data[h][w]);
        }
    }
}

template<typename T>
inline void MapLayer<T>::configure_float(const Color &posColor, const Color &negColor)
{
    config.colors[0] = posColor;
    config.colors[1] = negColor;
}

template<typename T>
inline void MapLayer<T>::configure_bool(const Color &falseColor, const Color &trueColor)
{
    config.colors[0] = falseColor;
    config.colors[1] = trueColor;
}

template<typename T>
inline void MapLayer<T>::configure_color(float alpha)
{
    config.alpha = alpha;
}

template<typename T>
inline void MapLayer<T>::draw(MeshRenderer &instancer, const std::vector<std::vector<Vec3>> &positions)
{
    if (enabled == true)
    {
        for (int h = 0; h < height; ++h)
        {
            for (int w = 0; w < width; ++w)
            {
                auto pos = positions[h][w];
                pos.y = yHeight;

                draw_cell(instancer, h, w, pos);
            }
        }
    }
}

template<typename T>
inline void MapLayer<T>::draw_cell(MeshRenderer &instancer, size_t row, size_t col, const Vec3 &pos)
{
    static_assert(false, "no generic draw logic");
}

template<typename T>
inline void MapLayer<T>::populate_with_value(int inH, int inW, const T &value)
{
    if (allocHeight < inH || allocWidth < inW)
    {
        free_memory();

        allocHeight = inH;
        allocWidth = inW;

        allocate_memory();
    }

    height = inH;
    width = inW;

    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
            data[h][w] = value;
        }
    }
}

template<typename T>
inline void MapLayer<T>::populate_with_data(const std::vector<std::vector<T>> &d)
{
    const int inH = static_cast<int>(d.size());
    const int inW = static_cast<int>(d.front().size());

    if (allocHeight < inH || allocWidth < inW)
    {
        free_memory();

        allocHeight = inH;
        allocWidth = inW;

        allocate_memory();
    }
    
    height = inH;
    width = inW;

    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
            data[h][w] = d[h][w];
        }
    }
}

template<typename T>
inline void MapLayer<T>::free_memory()
{
    if (data != nullptr)
    {
        for (int h = 0; h < allocHeight; ++h)
        {
            delete[] data[h];
        }

        delete[] data;
    }
}

template<typename T>
inline void MapLayer<T>::allocate_memory()
{
    data = new T*[allocHeight];

    for (int h = 0; h < allocHeight; ++h)
    {
        data[h] = new T[allocWidth];
    }
}

template<>
inline void MapLayer<bool>::draw_cell(MeshRenderer &instancer, size_t row, size_t col, const Vec3 &pos)
{
    instancer.add_grid_instance(pos, config.colors[static_cast<unsigned>(data[row][col])]);
}

template<>
inline void MapLayer<Color>::draw_cell(MeshRenderer &instancer, size_t row, size_t col, const Vec3 &pos)
{
    const auto &c = data[row][col];
    if (c != Colors::White)
    {
        Color color = c;
        color.w = config.alpha;

        instancer.add_grid_instance(pos, color);
    }
}

template<>
inline void MapLayer<float>::draw_cell(MeshRenderer &instancer, size_t row, size_t col, const Vec3 &pos)
{
    const float value = data[row][col];

    Color color = (value >= 0.0f) ? config.colors[0] : config.colors[1];
    color.w *= std::abs(value);

    instancer.add_grid_instance(pos, color);
}
