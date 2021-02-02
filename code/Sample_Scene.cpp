/*
 * SAMPLE SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Sample_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Log>
#include <basics/Scaling>
#include <basics/Rotation>
#include <basics/Translation>

using namespace basics;
using namespace std;

namespace example
{
    Sample_Scene::Sample_Scene()
    {
        state     = UNINITIALIZED;
        suspended = true;
        canvas_width  = 1280;
        canvas_height =  720;
    }

//--------------------------------------------------------------------------------------------------

    bool Sample_Scene::initialize ()
    {
        // inicializar cada ficha:
        //  para cada ficha:
        //      establecer su posición, tamaño y color

        Ficha ficha1;
        fichas.push_back(ficha1);
        Ficha ficha2;
        fichas.push_back(ficha2);
        Ficha ficha3;
        fichas.push_back(ficha3);
        Ficha ficha4;
        fichas.push_back(ficha4);
        Ficha ficha5;
        fichas.push_back(ficha5);
        Ficha ficha6;
        fichas.push_back(ficha6);

        for(int i = 0; i < numero_de_cajas; i++)
        {
            fichas[i].left_x = fichasX[i];
            fichas[i].bottom_y = fichasY[0];

            fichas[i].r = 1;
            fichas[i].g = 1;
            fichas[i].b = 1;
        }

        for(auto ficha: fichas)
        {
            cajas.push_back(&ficha);
        }
        // inicializar cada casilla:
        //  para cada casilla:
        //      establcer su posición y tamaño (ficha es nullptr siempre)

        Casilla casilla1;
        casillas.push_back(casilla1);
        Casilla casilla2;
        casillas.push_back(casilla2);
        Casilla casilla3;
        casillas.push_back(casilla3);
        Casilla casilla4;
        casillas.push_back(casilla4);
        Casilla casilla5;
        casillas.push_back(casilla5);
        Casilla casilla6;
        casillas.push_back(casilla6);

        for(int i = 0; i < numero_de_cajas; i++)
        {
            casillas[i].left_x = fichasX[i];
            casillas[i].bottom_y = fichasY[1];

            casillas[i].ficha = nullptr;
        }

        for(auto casilla : casillas)
        {
            cajas.push_back(&casilla);
        }
        // meter las fichas en cajas
        // meter las casillas en cajas
        ficha_tocada = nullptr;
        state = PLAYING;
    }

//--------------------------------------------------------------------------------------------------

    void Sample_Scene::suspend ()
    {
        suspended = true;
    }

//--------------------------------------------------------------------------------------------------

    void Sample_Scene::resume ()
    {
        suspended = false;
    }

//--------------------------------------------------------------------------------------------------

    void Sample_Scene::handle (Event & event)
    {
        switch (event.id)
        {
            case ID(touch-started):
            case ID(touch-moved):

            {
                // Se determina qué opción se ha tocado:

                Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };

                break;
            }

            case ID(touch-ended):

            {
                // Se determina qué opción se ha dejado de tocar la última y se actúa como corresponda:

                Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };

                break;
            }
        }
    }

//--------------------------------------------------------------------------------------------------

    void Sample_Scene::update (float time)
    {
        if (!font)
        {
            Graphics_Context::Accessor context = director.lock_graphics_context ();

            if (context)
            {
                font.reset (new Raster_Font("fonts/Fuente.fnt", context));
            }
        }
    }

//---------------------------------------------------------------------------------------------------

    void Sample_Scene::render (basics::Graphics_Context::Accessor & context) {
        if (!suspended)
        {
            Canvas *canvas = context->get_renderer<Canvas>(ID(canvas));

            if (!canvas)
            {
                canvas = Canvas::create(ID(canvas), context, {{canvas_width, canvas_height}});
            }

            if(state == PLAYING)
            {
                if (canvas) {
                    canvas->clear();

                    for( auto caja : cajas)
                    {
                        caja->render(canvas);
                    }
                }
            }
        }
    }


//--------------------------------------------------------------------------------------------------

    void Sample_Scene::load ()
    {
        /*
         if (!suspended)
        {
            Graphics_Context::Accessor context = director.lock_graphics_context ();

            if (context)
            {
                texture = Texture_2D::create (ID(test), context, "test.png");

                if (texture)
                {
                    context->add (texture);
                    state = RUNNING;
                }
            }
        }
         */

    }

//--------------------------------------------------------------------------------------------------

    void Sample_Scene::run (float )
    {
    }

//--------------------------------------------------------------------------------------------------




}




