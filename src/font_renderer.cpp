#include <MFR/font_renderer.hpp>
#include <MARS/graphics/backend/template/buffer.hpp>
#include <MARS/engine/object_engine.hpp>

using namespace MFR;

void font_renderer::load_font(const std::string& _path) {
    FT_Library library;
    if (FT_Init_FreeType(&library)) {
        FT_Done_FreeType(library);
    }

    FT_Face face;
    if (FT_New_Face( library, _path.c_str(), 0, &face )) {
        FT_Done_Face(face);
        FT_Done_FreeType(library);
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    mars_math::vector3<uint32_t> size;

    std::map<unsigned char, font_character> characters;

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER) || face->glyph->bitmap.width == 0 || face->glyph->bitmap.rows == 0)
            continue;

        size.x = std::max(face->glyph->bitmap.width, size.x);
        size.y = std::max(size.y, face->glyph->bitmap.rows);
        size.z = std::max(size.x, size.y);

        font_character c_data = {
                .position = { },
                .size = { face->glyph->bitmap.width, face->glyph->bitmap.rows },
                .bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top },
                .advance = face->glyph->advance.x
        };

        characters.insert(std::make_pair(c, c_data));
    }


    bool found = false;
    size_t final_size = 0;

    do {
        size_t tex_size = std::pow(2, ++final_size);

        if ((characters.size() * size.z) / (tex_size * size.z) > tex_size)
            continue;

        found = true;
    } while (!found);

    auto val = std::pow(2, ceil(log(std::pow(2, final_size) * size.z)/log(2)) - 1);
    mars_math::vector2<int> real_size(val, val);

    std::vector<std::shared_ptr<mars_graphics::buffer>> m_update_buffers;

    auto builder = engine()->graphics()->builder<mars_graphics::texture_builder>();
    builder.set_size({ real_size.x, real_size.y }).set_format(mars_graphics::MARS_FORMAT_S_R8).set_usage(mars_graphics::MARS_TEXTURE_USAGE_TRANSFER);
    builder.initialize();

    mars_math::vector2<uint32_t> offset = {0, 0};

    for (auto& character : characters) {
        FT_Load_Char(face, character.first, FT_LOAD_RENDER);
        auto buff_buidler = engine()->graphics()->builder<mars_graphics::buffer_builder>();
        auto buffer = buff_buidler.set_size(character.second.size.x * character.second.size.y).set_type(mars_graphics::MARS_MEMORY_TYPE_TRANSFER).build();
        buffer->update(face->glyph->bitmap.buffer);
        buffer->copy_data(0);
        m_update_buffers.push_back(buffer);

        builder.copy_buffer_to_image(buffer, { offset.x, offset.y, character.second.size.x, character.second.size.y });
        character.second.position = offset;

        offset.x += character.second.size.x;

        if (offset.x + size.x >= real_size.x) {
            offset.x = 0;
            offset.y += size.y;
        }

    }

    auto fonts = m_fonts.lock();

    auto new_font = fonts->emplace_back(std::make_shared<font>());
    new_font->path = _path;
    new_font->characters = characters;
    new_font->texture = builder.build();

    m_update_buffers.clear();

    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

void font_renderer::render() {

}