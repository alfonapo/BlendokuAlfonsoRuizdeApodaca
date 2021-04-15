/*
 * MENU SCENE
 * Copyright © 2021+ Alfonso Ruiz de Apodaca Caparrós
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * alfon.rda@gmail.com
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
        suspended     = false;
        state         = Intro;
    }

//--------------------------------------------------------------------------------------------------

    bool Menu_Scene::initialize ()
    {
        //crea las fichas y les asigna sus propiedades
        Sample_Scene::Ficha jugar;
        jugar.bottom_y = canvas_height/1.7;
        jugar.r        = 0;
        jugar.g        = 0;
        jugar.b        = 1;
        opciones.push_back(jugar);

        Sample_Scene::Ficha salir;
        salir.bottom_y = canvas_height/2.4;
        salir.r        = 0;
        salir.g        = 0;
        salir.b        = 0.5;
        opciones.push_back(salir);

        Sample_Scene::Ficha ayuda;
        ayuda.bottom_y = canvas_height/4.2;
        ayuda.r        = 0;
        ayuda.g        = 0;
        ayuda.b        = 0.3;
        opciones.push_back(ayuda);

        //propiedades comunes a todas las opciones
        for( auto &opcion : opciones)
        {
            opcion.left_x = canvas_width/3 + 80;
            opcion.colocada = false;
            opcion.ancho = 256;
            opcion.alto = 128;
        }
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
                if(state == MenuPpal)
                {
                    float x = *event[ID(x)].as< var::Float > ();
                    float y = *event[ID(y)].as< var::Float > ();

                    for( auto &opcion : opciones)
                    {
                        if(opcion.contains(x,y))
                        {
                            //si toca la opcion jugar
                            if(opcion.b == 1)
                            {
                                director.run_scene (shared_ptr< Scene >(new Sample_Scene));
                            }
                            //si toca la opcion ayuda
                            else if (opcion.b == 0.5){
                                state = Ayuda;
                            }
                            //si toca la opcion salir
                            else{
                                director.stop();
                            }
                        }
                    }
                    break;
                }
                //pantalla de intro o de ayuda
                else{
                    state = MenuPpal;
                }
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
                if(state == Intro)
                {
                    canvas->clear();

                    if(font)
                    {
                        Text_Layout tituloTxt    (*font, L"Blendoku");
                        Text_Layout continuarTxt (*font, L"Toca la pantalla para continuar...");

                        canvas->draw_text({canvas_width/2 - 10,canvas_height/2 + 70},            tituloTxt, CENTER);
                        canvas->draw_text({canvas_width/2 - 10,(canvas_height/2 + 70) - 128}, continuarTxt, CENTER);
                    }
                }

                if(state == MenuPpal)
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
                        Text_Layout ayudaTxt (*font, L"Ayuda");

                        canvas->draw_text({canvas_width/2 - 10, canvas_height/2 + 130}, jugarTxt, CENTER);
                        canvas->draw_text({canvas_width/2 - 10, canvas_height/2      }, ayudaTxt, CENTER);
                        canvas->draw_text({canvas_width/2 - 10, canvas_height/2 - 125}, salirTxt, CENTER);
                    }
                }

                if (state == Ayuda)
                {
                    canvas->clear();

                    if(font)
                    {
                        Text_Layout ayudaTxt1 (*font, L"Coloca las fichas superiores en las casillas inferiores por orden de color,");
                        Text_Layout ayudaTxt2 (*font, L"la más oscura tiene que quedar a la izquierda y la más clara a la derecha.");
                        Text_Layout ayudaTxt3 (*font, L"Toca la pantalla para volver...");

                        canvas->draw_text({canvas_width/2 - 10, canvas_height/2 + 130}, ayudaTxt1, CENTER);
                        canvas->draw_text({canvas_width/2 - 10, canvas_height/2      }, ayudaTxt2, CENTER);
                        canvas->draw_text({canvas_width/2 - 10, canvas_height/2 - 300}, ayudaTxt3, CENTER);
                    }
                }
            }
        }
    }
}




