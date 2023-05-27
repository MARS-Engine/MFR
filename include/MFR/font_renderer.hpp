#ifndef MARS_FONT_RENDERER_
#define MARS_FONT_RENDERER_

#include <pl/safe.hpp>
#include <MARS/engine/singleton.hpp>
#include <map>
#include <MARS/graphics/backend/template/texture.hpp>
#include <MARS/math/vector2.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace MFR {
    enum MFR_TEXT_TYPE {
        MFR_TEXT_TYPE_STATIC,
        MFR_TEXT_TYPE_STATIC_DYNAMIC
    };

    struct font_character {
        mars_math::vector2<uint32_t> position;
        mars_math::vector2<uint32_t> size;
        mars_math::vector2<int> bearing;
        long advance;
    };

    struct font {
        std::string path;
        std::map<unsigned char, font_character> characters;
        std::shared_ptr<mars_graphics::texture> texture;
    };

    struct text {
    private:
        std::string m_content;
    public:
        MFR_TEXT_TYPE type;

        void set_content(const std::string& _content);
    };

    class font_renderer : public mars_engine::singleton {
    private:
        std::vector<std::shared_ptr<text>> m_text;
        pl::safe<std::vector<std::shared_ptr<font>>> m_fonts;
    public:
        using mars_engine::singleton::singleton;

        ~font_renderer() {
            m_fonts.lock()->clear();
            m_text.clear();
        }

        void load_font(const std::string& _path);
        void render();
    };
}

#endif