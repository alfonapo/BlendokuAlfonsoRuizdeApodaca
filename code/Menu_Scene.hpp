/*
 * MENU SCENE
 * Copyright © 2021+ Alfonso Ruiz de Apodaca Caparrós
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * alfon.rda@gmail.com
 */

#ifndef MENU_HEADER
#define MENU_HEADER

#include <memory>
#include <basics/Atlas>
#include <basics/Canvas>
#include <basics/Point>
#include <basics/Scene>
#include <basics/Size>
#include <basics/Timer>

#include "Sample_Scene.hpp"


namespace example
{

    using basics::Canvas;
    using basics::Atlas;
    using basics::Point2f;
    using basics::Size2f;
    using basics::Texture_2D;
    using basics::Graphics_Context;

    class Menu_Scene : public basics::Scene
    {

        typedef std::shared_ptr< basics::Texture_2D > Texture_Handle;
        typedef std::unique_ptr< basics::Raster_Font > Font_Handle;

        enum state
        {
            MenuPpal,
            Intro,
            Ayuda
        };

        std::vector < Sample_Scene::Ficha > opciones;

        unsigned    canvas_width;
        unsigned    canvas_height;


    private:

        state                 state;
        bool                  suspended;
        Font_Handle           font;

    public:

        Menu_Scene();

        basics::Size2u get_view_size () override
        {
            return { canvas_width, canvas_height };
        }

        bool initialize () override;
        void suspend    () override;
        void resume     () override;

        void handle     (basics::Event & event) override;
        void update     (float time) override;
        void render     (basics::Graphics_Context::Accessor & context) override;

    };

}
#endif