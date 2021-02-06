/*
 * SAMPLE SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include <memory>
#include <thread>
#include <map>

#include <basics/Scene>
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Atlas>
#include <basics/Transformation>
#include <basics/Point>
#include <basics/Size>
#include <basics/Timer>
#include <basics/Raster_Font>
#include <basics/Texture_2D>


#include "Sprite.hpp"


namespace example
{

    using basics::Canvas;
    using basics::Atlas;
    using basics::Point2f;
    using basics::Size2f;
    using basics::Texture_2D;
    using basics::Graphics_Context;

    class Sample_Scene : public basics::Scene
    {

        typedef std::shared_ptr< basics::Texture_2D > Texture_Handle;
        typedef std::unique_ptr< basics::Raster_Font > Font_Handle;

        enum State
        {
            UNINITIALIZED,
            PLAYING,
            GAME_FINISHED,
        };


        /* *
         * contenedor para las fichas y las casillas
         *
         * funcion contains comprueba si el punto x,y esta dentro de la caja
         * */
        struct Caja
        {
            float left_x, bottom_y;
            float anchoXalto;

            virtual void render (Canvas * canvas) = 0;

            bool contains (float x, float y)
            {
                return x > left_x && y > bottom_y && x < left_x + anchoXalto && y < bottom_y + anchoXalto;
            }
        };


        /* *
         * ficha coloreada (r,g,b)
         * */
        struct Ficha : public Caja
        {
            float r, g, b;

            bool colocada;

            void render (Canvas * canvas) override
            {
                canvas->set_color (r, g, b);
                canvas->fill_rectangle ({left_x,bottom_y}, {anchoXalto,anchoXalto});
            }
        };

        /* *
         * Casilla vacía que puede contener una ficha
         * */
        struct Casilla : public Caja
        {
            Ficha * ficha;

            void render (Canvas * canvas) override
            {
                if (ficha != nullptr)
                {
                    Ficha aux(*ficha);
                    aux.left_x = this->left_x;
                    aux.bottom_y = this->bottom_y;
                    aux.render(canvas);
                }

                canvas->set_color (1, 1, 1);
                canvas->draw_rectangle({left_x, bottom_y}, {anchoXalto, anchoXalto});
            }
        };

        struct RGB
        {
            float r, g, b;
        };

        std::vector< Ficha   > fichas;
        std::vector< Casilla > casillas;
        std::vector< Caja * > cajas;
        std::vector< RGB > colores;
        std::vector< int > indices;

        Ficha * ficha_tocada;
        float   ficha_tocada_posicion_inicial_x;
        float   ficha_tocada_posicion_inicial_y;

        const static int numero_de_cajas = 6;

        float       fichasX [numero_de_cajas] = {110, 287, 465, 643, 822, 1000};
        float       fichasX2 [numero_de_cajas];
        float       fichasY [2] = {117, 478};

        unsigned    canvas_width;
        unsigned    canvas_height;

        int         fichas_colocadas;
        bool        win_condition;

    private:

        State                 state;
        bool                  suspended;

        Texture_Handle        texture;
        Font_Handle           font;

    public:

        Sample_Scene();

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

    private:

        void load ();
        void run  (float time);
        void randomRNG (Ficha &ficha);
        void randomFichasX ();

    };

}
