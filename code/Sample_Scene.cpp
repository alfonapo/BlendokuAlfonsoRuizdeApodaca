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
#include <cstdlib>
#include <ctime>



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

    bool Sample_Scene::initialize () {

        //inicializo generador de numeros aleatorios
        srand(time(0));

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

        randomFichasX();

        for(int i = 0; i < numero_de_cajas; i++)
        {
            fichas[i].left_x = fichasX2[i];
            fichas[i].bottom_y = fichasY[1];
            fichas[i].colocada = false;
            fichas[i].anchoXalto = 128;
        }

        randomRNG(fichas[0]);

        for(auto ficha: fichas)
        {
            cajas.push_back(&ficha);
        }

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
            casillas[i].bottom_y = fichasY[0];
            casillas[i].anchoXalto = 128;
            casillas[i].ficha = nullptr;
        }

        for(auto casilla : casillas)
        {
            cajas.push_back(&casilla);
        }

        fichas_colocadas = 0;
        ficha_tocada = nullptr;
        state = PLAYING;
        return true;
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

    void Sample_Scene::handle (basics::Event & event)
    {
        if (state == PLAYING)
        {
            switch (event.id)
            {
                case ID(touch-started):
                {
                    float x = *event[ID(x)].as< var::Float > ();
                    float y = *event[ID(y)].as< var::Float > ();

                    for( auto &ficha : fichas)
                    {
                        if(ficha.contains(x, y) && ficha.bottom_y == fichasY[1])
                        {
                            ficha_tocada = &ficha;
                            ficha_tocada_posicion_inicial_x = ficha.left_x;
                            ficha_tocada_posicion_inicial_y = ficha.bottom_y;
                            break;
                        }
                    }
                    break;
                }

                case ID(touch-moved):
                {
                    float x = *event[ID(x)].as< var::Float > ();
                    float y = *event[ID(y)].as< var::Float > ();

                    if(ficha_tocada)
                    {
                        ficha_tocada->left_x = x - ficha_tocada->anchoXalto/2;
                        ficha_tocada->bottom_y = y - ficha_tocada->anchoXalto/2;
                    }
                    break;
                }

                case ID(touch-ended):
                {
                    float x = *event[ID(x)].as< var::Float > ();
                    float y = *event[ID(y)].as< var::Float > ();

                    if(ficha_tocada)
                    {
                        for ( auto &casilla : casillas)
                        {
                            if(casilla.contains(x, y))
                            {
                                casilla.ficha = ficha_tocada;
                                ficha_tocada->colocada = true;
                                fichas_colocadas++;

                            } else
                                {
                                ficha_tocada->left_x = ficha_tocada_posicion_inicial_x;
                                ficha_tocada->bottom_y = ficha_tocada_posicion_inicial_y;
                                }
                        }

                        if(fichas_colocadas == 6)
                        {
                            bool error = false;

                            for(int i = 0; i < fichas_colocadas; i++)
                            {
                                if(casillas[i].ficha->r != colores[i].r || casillas[i].ficha->g != colores[i].g || casillas[i].ficha->b != colores[i].b)
                                {
                                    error = true;
                                }
                            }

                            if(error)
                            {
                                int i = 0;
                                for( auto &ficha : fichas)
                                {
                                    ficha.left_x = fichasX[i];
                                    ficha.bottom_y = fichasY[1];
                                    ficha.colocada = false;
                                    i++;
                                }

                                i = 0;
                                for ( auto &casilla : casillas)
                                {
                                    casilla.ficha = nullptr;
                                }

                                fichas_colocadas = 0;
                            }
                            if(!error)
                            {
                                win_condition = true;
                                state = GAME_FINISHED;
                            }
                        }
                    }
                    ficha_tocada = nullptr;
                    break;
                }

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
            Canvas * canvas = context->get_renderer<Canvas>(ID(canvas));

            if (!canvas)
            {
                canvas = Canvas::create(ID(canvas), context, {{canvas_width, canvas_height}});
            }

            if(state == PLAYING)
            {
                if (canvas) {
                    canvas->clear();

                    for( auto ficha : fichas)
                    {
                        if(!ficha.colocada)
                        {
                            ficha.render(canvas);
                        }
                    }

                    for( auto casilla : casillas)
                    {
                        casilla.render(canvas);
                    }
                }
            }

            if(state == GAME_FINISHED)
            {
                if (canvas)
                {
                    canvas->clear();
                    if(font)
                    {
                        Text_Layout winText (*font, L"Has Ganado");

                        canvas->draw_text({canvas_width/2.f,canvas_height/2.f}, winText, CENTER);
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

    void Sample_Scene::randomRNG(Ficha &ficha)
    {
        float r_start = float(rand() % 255) / 255.f;
        float g_start = float(rand() % 255) / 255.f;
        float b_start = float(rand() % 255) / 255.f;

        float r_end = float(rand() % 255) / 255.f;
        float g_end = float(rand() % 255) / 255.f;
        float b_end = float(rand() % 255) / 255.f;

        if(r_end < r_start)
        {
            float sup_r = r_start;
            r_start = r_end;
            r_end = sup_r;
        }

        if(g_end < g_start)
        {
            float sup_g = g_start;
            g_start = g_end;
            g_end = sup_g;
        }

        if (b_end < b_start)
        {
            float sup_b = b_start;
            b_start = b_end;
            b_end = sup_b;
        }

        float r_step = (r_end - r_start)/ float (fichas.size());
        float g_step = (g_end - g_start)/ float (fichas.size());
        float b_step = (b_end - b_start)/ float (fichas.size());

        if(r_step < 0.3 && r_end <= 0.5)
        {
            r_end += 0.2;
            r_step = (r_end - r_start)/ 6;
        }

        if(r_step < 0.3 && r_end >= 0.5)
        {
            r_start -= 0.2;
            r_step = (r_end - r_start)/ 6;
        }

        if(g_step < 0.3 && g_end <= 0.5)
        {
            g_end += 0.2;
            g_step = (g_end - g_start)/ 6;
        }

        if(g_step < 0.3 && g_end >= 0.5)
        {
            g_start -= 0.2;
            g_step = (g_end - g_start)/ 6;
        }

        if(b_step < 0.3 && b_end <= 0.5)
        {
            b_end += 0.2;
            b_step = (b_end - b_start)/ 6;
        }

        if(b_step < 0.3 && b_end >= 0.5)
        {
            b_start -= 0.2;
            b_step = (b_end - b_start)/ 6;
        }

        for ( auto &ficha : fichas)
        {
            ficha.r = r_start;
            r_start +=r_step;

            ficha.g = g_start;
            g_start += g_step;

            ficha.b = b_start;
            b_start+=b_step;

            colores.push_back({ficha.r, ficha.g, ficha.b});
        }
    }

//--------------------------------------------------------------------------------------------------

    void Sample_Scene::randomFichasX()
    {
        int nums[6] = {0,1,2,3,4,5};
        bool indexUsado = true;
        int indiceX1 = 0;

        for(int i = 0; i < 6; i++)
        {
            indices.push_back(nums[i]);
        }

        do
            {
                int index = (rand() % 7) ;

                for ( auto &indice : indices)
                {
                    if (indice == index)
                    {
                        indexUsado = false;
                        indice = 0;
                    }
                }

                if (!indexUsado)
                {
                    fichasX2[indiceX1] = fichasX[index];
                    indiceX1++;
                    indexUsado = true;
                }


            }while (indiceX1 < 6);













    }
}




