/*
 * SAMPLE SCENE
 * Copyright © 2021+ Alfonso Ruiz de Apodaca Caparrós
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * alfon.rda@gmail.com
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

        //fichas y guardarlas en sus vectores
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

        Ficha reiniciar;
        reiniciar.left_x = canvas_width/3 + 80;
        reiniciar.bottom_y = canvas_height/2;
        reiniciar.colocada = false;
        reiniciar.ancho = 256;
        reiniciar.alto = 128;
        reiniciar.r = 0;
        reiniciar.g = 1;
        reiniciar.b = 0;
        opciones.push_back(reiniciar);

        Ficha salir;
        salir.left_x = canvas_width/3 + 80;
        salir.bottom_y = canvas_height/3;
        salir.colocada = false;
        salir.ancho = 256;
        salir.alto = 128;
        salir.r = 1;
        salir.g = 0;
        salir.b = 0;
        opciones.push_back(salir);

        //desordena las fichas
        randomFichasX();

        //propiedades de las fichas
        for(int i = 0; i < numero_de_cajas; i++)
        {
            fichas[i].left_x = fichasX2[i];
            fichas[i].bottom_y = fichasY[1];
            fichas[i].colocada = false;
            fichas[i].ancho = 128;
            fichas[i].alto = 128;
        }

        //colores de las fichas
        randomRNG();

        for(auto ficha: fichas)
        {
            cajas.push_back(&ficha);
        }

        //casillas y guardarlas en su vectore
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

        //propiedades de las casillas
        for(int i = 0; i < numero_de_cajas; i++)
        {
            casillas[i].left_x = fichasX[i];
            casillas[i].bottom_y = fichasY[0];
            casillas[i].ancho = 128;
            casillas[i].alto = 128;
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

                    //si toca en una ficha de la linea de arriba se guarda la ficha tocada y su posicion
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

                    //si toca en una casilla que tiene ficha, elimina su ficha y enseña la de arriba
                    for ( auto &casilla : casillas)
                    {
                        if(casilla.contains(x,y) && casilla.ficha)
                        {
                            float r = casilla.ficha->r;
                            float g = casilla.ficha->g;
                            float b = casilla.ficha->b;

                            for ( auto &ficha : fichas)
                            {
                                if (ficha.r == r && ficha.g == g && ficha.b == b )
                                {
                                    ficha.colocada = false;
                                    fichas_colocadas--;
                                }
                            }

                            casilla.ficha = nullptr;
                        }
                    }

                    break;
                }

                case ID(touch-moved):
                {
                    float x = *event[ID(x)].as< var::Float > ();
                    float y = *event[ID(y)].as< var::Float > ();

                    // si hay una ficha tocada, la arrastra (modificado para que se arrastre desde el centro)
                    if(ficha_tocada)
                    {
                        ficha_tocada->left_x = x - ficha_tocada->ancho/2;
                        ficha_tocada->bottom_y = y - ficha_tocada->alto/2;
                    }
                    break;
                }

                case ID(touch-ended):
                {
                    float x = *event[ID(x)].as< var::Float > ();
                    float y = *event[ID(y)].as< var::Float > ();

                    if(ficha_tocada)
                    {
                        //si se deja la ficha sobre una casilla que no tenga ya ficha, se guarda la ficha en el puntero de la casilla
                        for ( auto &casilla : casillas)
                        {
                            if(casilla.contains(x, y) && casilla.ficha == nullptr)
                            {
                                casilla.ficha = ficha_tocada;
                                ficha_tocada->colocada = true;
                                fichas_colocadas++;

                            }
                            //si no, la devuelve a su posicion inicial
                            else
                                {
                                ficha_tocada->left_x = ficha_tocada_posicion_inicial_x;
                                ficha_tocada->bottom_y = ficha_tocada_posicion_inicial_y;
                                }
                        }

                        if(fichas_colocadas == 6)
                        {
                            bool error = false;

                            //busca algun error en la fila de casillas
                            for(int i = 0; i < fichas_colocadas; i++)
                            {
                                if(casillas[i].ficha->r != colores[i].r || casillas[i].ficha->g != colores[i].g || casillas[i].ficha->b != colores[i].b)
                                {
                                    error = true;
                                }
                            }

                            //si lo hay, desordena las fichas otra vez y limpia los punteros de las casillas
                            if(error)
                            {
                                int i = 0;

                                randomFichasX();
                                for( auto &ficha : fichas)
                                {
                                    ficha.left_x = fichasX2[i];
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

                            //si no lo hay, cambia de estado a game finished
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

        if (state == GAME_FINISHED)
        {
            switch (event.id)
            {
                case ID(touch-started):
                {
                    float x = *event[ID(x)].as< var::Float > ();
                    float y = *event[ID(y)].as< var::Float > ();

                    //menu de fin de juego
                    for( auto &opcion : opciones)
                    {
                        if(opcion.contains(x,y))
                        {
                            if(opcion.g == 1)
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

            //pinta las fichas y las casillas
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

            //pinta el menu de fin de juego
            if(state == GAME_FINISHED)
            {
                if (canvas)
                {
                    canvas->clear();
                    if(font)
                    {
                        Text_Layout winText (*font, L"Has Ganado");
                        Text_Layout reiniciar (*font, L"Otro nivel");
                        Text_Layout salir (*font, L"Salir");

                        for( auto &opcion : opciones)
                        {
                            opcion.render(canvas);
                        }

                        canvas->draw_text({canvas_width/2.f,canvas_height - 100}, winText, CENTER);
                        canvas->draw_text({canvas_width/2 - 10,canvas_height/2 + 70}, reiniciar, CENTER);
                        canvas->draw_text({canvas_width/2 - 10,(canvas_height/2 + 70) - 128}, salir, CENTER);
                    }
                }
            }
        }
    }

//--------------------------------------------------------------------------------------------------

    /* *
     * Genera un gradiante de colores para las fichas
     * */
    void Sample_Scene::randomRNG()
    {
        //Inicio del gradiante para cada color
        float r_start = float(rand() % 255) / 255.f;
        float g_start = float(rand() % 255) / 255.f;
        float b_start = float(rand() % 255) / 255.f;

        //Fin del graciante para cada color
        float r_end = float(rand() % 255) / 255.f;
        float g_end = float(rand() % 255) / 255.f;
        float b_end = float(rand() % 255) / 255.f;

        //Los starts son menores que los los ends
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

        //Pasos intermedios para sumarlos a cada ficha
        float r_step = (r_end - r_start)/ float (fichas.size());
        float g_step = (g_end - g_start)/ float (fichas.size());
        float b_step = (b_end - b_start)/ float (fichas.size());

        //Si salen muy pegados, amplia la diferencia para que no sean demasiado parecidos los colores
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

        //asigna los colores a las fichas sumandole step al start cada vuelta. guarda los colores en el vector de colores
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

    /* *
     * Desordena el array de posiciones de las fichas
     * */
    void Sample_Scene::randomFichasX()
    {
        int nums[6] = {1,2,3,4,5,6};
        bool indexUsado = true;
        int indiceX1 = 0;

        for(int i = 0; i < 6; i++)
        {
            indices.push_back(nums[i]);
        }

        do
            {
                int index = rand() % 6 + 1 ;

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
                    fichasX2[indiceX1] = fichasX[index - 1];
                    indiceX1++;
                    indexUsado = true;
                }

            }while (indiceX1 < 6);
    }
}




