/*
 * SAMPLE SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */


#include "Menu_Scene.hpp"

#include <basics/Canvas>
#include <basics/Director>
#include <basics/Log>
#include <basics/Scaling>
#include <basics/Rotation>
#include <basics/Translation>
#include <cstdlib>
#include <ctime>

using namespace basics;
using namespace std;

namespace example
{
    Menu_Scene::Menu_Scene()
    {
        canvas_width  = 1280;
        canvas_height =  720;
        suspended = false;
    }

//--------------------------------------------------------------------------------------------------

    bool Menu_Scene::initialize ()
    {
        Sample_Scene::Ficha jugar;
        jugar.left_x = canvas_width/3 + 80;
        jugar.bottom_y = canvas_height/2;
        jugar.colocada = false;
        jugar.ancho = 256;
        jugar.alto = 128;
        jugar.r = 0;
        jugar.g = 0;
        jugar.b = 1;
        opciones.push_back(jugar);

        Sample_Scene::Ficha salir;
        salir.left_x = canvas_width/3 + 80;
        salir.bottom_y = canvas_height/3;
        salir.colocada = false;
        salir.ancho = 256;
        salir.alto = 128;
        salir.r = 0;
        salir.g = 0;
        salir.b = 0.5;
        opciones.push_back(salir);

        return true;
    }

//--------------------------------------------------------------------------------------------------

    void Menu_Scene::suspend ()
    {
        suspended = true;
    }

//--------------------------------------------------------------------------------------------------

    void Menu_Scene::resume ()
    {
        suspended = false;
    }

//--------------------------------------------------------------------------------------------------

    void Menu_Scene::handle (basics::Event & event)
    {
        switch (event.id)
        {
            case ID(touch-started):
            {
                float x = *event[ID(x)].as< var::Float > ();
                float y = *event[ID(y)].as< var::Float > ();

                for( auto &opcion : opciones)
                {
                    if(opcion.contains(x,y))
                    {
                        if(opcion.b == 1)
                        {
                            director.run_scene (shared_ptr< Scene >(new Sample_Scene));
                        } else{
                            director.stop();
                        }
                    }
                }
                break;
            }
        }
    }

//--------------------------------------------------------------------------------------------------

    void Menu_Scene::update (float time)
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

    void Menu_Scene::render (basics::Graphics_Context::Accessor & context)
    {
        if (!suspended)
        {
            Canvas * canvas = context->get_renderer<Canvas>(ID(canvas));

            if (!canvas)
            {
                canvas = Canvas::create(ID(canvas), context, {{canvas_width, canvas_height}});
            }

            if (canvas)
            {
                canvas->clear();

                for( auto &opcion : opciones)
                {
                    opcion.render(canvas);
                }

                if(font)
                {
                    Text_Layout jugarTxt (*font, L"Jugar");
                    Text_Layout salirTxt (*font, L"Salir");

                    canvas->draw_text({canvas_width/2 - 10,canvas_height/2 + 70}, jugarTxt, CENTER);
                    canvas->draw_text({canvas_width/2 - 10,(canvas_height/2 + 70) - 128}, salirTxt, CENTER);
                }
            }

        }
    }
}




