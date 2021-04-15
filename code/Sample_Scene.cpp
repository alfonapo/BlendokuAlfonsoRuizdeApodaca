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
    Sample_Scene::Sample_Scene()
    {
        state         = UNINITIALIZED;
        suspended     = true;
        canvas_width  = 1280;
        canvas_height =  720;

    }

//--------------------------------------------------------------------------------------------------

    bool Sample_Scene::initialize () {

        srand(time(nullptr));

        //inicializa las fichas
        fichasInit();

        //inicializa las casillas
        casillasInit();

        fichas_colocadas = 0;
        ficha_tocada     = nullptr;

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
                        //si toca en una ficha de la linea de arriba se guarda la ficha tocada y su posicion
                        if(ficha.contains(x, y) && ficha.bottom_y == fichasY[1])
                        {
                            ficha_tocada                    = &ficha;
                            ficha_tocada_posicion_inicial_x = ficha.left_x;
                            ficha_tocada_posicion_inicial_y = ficha.bottom_y;
                            break;
                        }

                        //si toca el pause, se cambia el state a PAUSE
                        if(ficha.contains(x,y) && ficha.alto == 100)
                        {
                            state = PAUSE;
                        }
                    }

                    for ( auto &casilla : casillas)
                    {
                        //si toca en una casilla que tiene ficha, elimina su ficha y enseña la que le corresponde de la fila de arriba
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
                        ficha_tocada->left_x   = x - ficha_tocada->ancho/2;
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
                        for ( auto &casilla : casillas)
                        {
                            //si se deja la ficha sobre una casilla que no tenga ya ficha, se guarda la ficha en la casilla
                            if(casilla.contains(x, y) && casilla.ficha == nullptr)
                            {
                                casilla.ficha          = ficha_tocada;
                                ficha_tocada->colocada = true;
                                fichas_colocadas++;
                            }

                            //si no, la devuelve a su posicion inicial
                            else{
                                ficha_tocada->left_x   = ficha_tocada_posicion_inicial_x;
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
                                    ficha.left_x   = fichasX2[i];
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
                                state = GAME_FINISHED;
                            }
                        }
                    }
                    ficha_tocada = nullptr;
                    break;
                }
            }
        }

        if (state == PAUSE)
        {
            switch (event.id)
            {
                case ID(touch-started):
                {
                    float x = *event[ID(x)].as< var::Float > ();
                    float y = *event[ID(y)].as< var::Float > ();

                    for ( auto &opcion : opciones)
                    {
                        //si toca la opcion salir
                        if(opcion.contains(x,y) && opcion.r == 1)
                        {
                            director.stop();
                        }

                        //si toca la opcion continuar
                        if(opcion.contains(x,y) && opcion.g == 1)
                        {
                            state = PLAYING;
                        }

                        //si toca la opcion volver al menu
                        if(opcion.contains(x,y) && opcion.b == 1)
                        {
                            director.run_scene (shared_ptr< Scene >(new Menu_Scene));
                        }
                    }
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

                    for( auto &opcion : opciones2)
                    {
                        if(opcion.contains(x,y))
                        {
                            //si toca la opcion otro nivel
                            if(opcion.r == 1)
                            {
                                director.run_scene (shared_ptr< Scene >(new Sample_Scene));
                            }

                            //si toca la opcion salir
                            else{
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

                texture = Texture_2D::create (ID(test), context, "pause.png");

                if(texture)
                {
                    context->add(texture);

                    state = PLAYING;
                }
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

                    //pinta las fichas que no estan colocadas
                    for( auto ficha : fichas)
                    {
                        if(!ficha.colocada)
                        {
                            ficha.render(canvas);
                        }
                    }

                    //pinta las casillas
                    for( auto casilla : casillas)
                    {
                        casilla.render(canvas);
                    }

                    //pinta el boton de pause
                    canvas->fill_rectangle ({ 1150, canvas_height/2 }, { 100, 100 }, texture.get ());
                }
            }

            if(state == PAUSE)
            {
                if(canvas)
                {
                    canvas->clear();
                    if(font)
                    {
                        Text_Layout continuar (*font, L"Continuar");
                        Text_Layout back2Menu (*font, L"Volver al menu");
                        Text_Layout salir2    (*font, L"Salir");

                        //pinta las opciones del menu de pause
                        for( auto &opcion : opciones)
                        {
                            opcion.render(canvas);
                        }

                        //texto de las opciones
                        canvas->draw_text({canvas_width/2 - 10, canvas_height/2 + 130}, continuar, CENTER);
                        canvas->draw_text({canvas_width/2 - 10, canvas_height/2      }, back2Menu, CENTER);
                        canvas->draw_text({canvas_width/2 - 10, canvas_height/2 - 125}, salir2,    CENTER);
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
                        Text_Layout winText   (*font, L"Has Ganado");
                        Text_Layout reiniciar (*font, L"Otro nivel");
                        Text_Layout salir     (*font, L"Salir");

                        //pinta las opciones del menu final
                        for( auto &opcion : opciones2)
                        {
                            opcion.render(canvas);
                        }

                        //texto de las opciones
                        canvas->draw_text({canvas_width/2     ,canvas_height - 100}         , winText  , CENTER);
                        canvas->draw_text({canvas_width/2 - 10,canvas_height/2 + 70}        , reiniciar, CENTER);
                        canvas->draw_text({canvas_width/2 - 10,(canvas_height/2 + 70) - 128}, salir    , CENTER);
                    }
                }
            }
        }
    }

//--------------------------------------------------------------------------------------------------

    ///Genera un gradiante de colores para las fichas
    void Sample_Scene::randomRNG()
    {
        //Inicio del gradiante para cada color
        float r_start = float(rand() % 255) / 255.f;
        float g_start = float(rand() % 255) / 255.f;
        float b_start = float(rand() % 255) / 255.f;

        //Fin del gradiante para cada color
        float r_end = float(rand() % 255) / 255.f;
        float g_end = float(rand() % 255) / 255.f;
        float b_end = float(rand() % 255) / 255.f;

        //ordena los start y end de r, g y b
        if(r_end < r_start)
        {
            float sup_r = r_start;
            r_start     = r_end;
            r_end       = sup_r;
        }
        if(g_end < g_start)
        {
            float sup_g = g_start;
            g_start     = g_end;
            g_end       = sup_g;
        }
        if (b_end < b_start)
        {
            float sup_b = b_start;
            b_start     = b_end;
            b_end       = sup_b;
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
            r_step   = (r_end - r_start)/ 6;
        }
        if(g_step < 0.3 && g_end <= 0.5)
        {
            g_end += 0.2;
            g_step = (g_end - g_start)/ 6;
        }
        if(g_step < 0.3 && g_end >= 0.5)
        {
            g_start -= 0.2;
            g_step   = (g_end - g_start)/ 6;
        }
        if(b_step < 0.3 && b_end <= 0.5)
        {
            b_end += 0.2;
            b_step = (b_end - b_start)/ 6;
        }
        if(b_step < 0.3 && b_end >= 0.5)
        {
            b_start -= 0.2;
            b_step   = (b_end - b_start)/ 6;
        }

        //asigna los colores a las fichas sumandole step al start cada vuelta. guarda los colores en el vector de colores
        for ( auto &ficha : fichas)
        {
            if(ficha.alto != 100)
            {
                ficha.r  = r_start;
                r_start += r_step;

                ficha.g  = g_start;
                g_start += g_step;

                ficha.b  = b_start;
                b_start += b_step;

                colores.push_back({ficha.r, ficha.g, ficha.b});
            }
        }
    }

//--------------------------------------------------------------------------------------------------

    ///Desordena el array de posiciones de las fichas
    void Sample_Scene::randomFichasX()
    {
        int nums[6]     = {1,2,3,4,5,6};
        bool indexUsado = true;
        int indiceX1    = 0;

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
                    indice     = 0;
                }
            }

            if (!indexUsado)
            {
                fichasX2[indiceX1] = fichasX[index - 1];
                indexUsado         = true;
                indiceX1++;
            }

        }while (indiceX1 < 6);
    }

    //--------------------------------------------------------------------------------------------------

    ///Inicializa las fichas
    void Sample_Scene::fichasInit()
    {
        //fichas jugables
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

        //fichas del menu pause
        Ficha continuar;
        continuar.left_x   = canvas_width/3 + 80;
        continuar.bottom_y = canvas_height/1.7;
        continuar.colocada = false;
        continuar.ancho    = 256;
        continuar.alto     = 128;
        continuar.r        = 0;
        continuar.g        = 1;
        continuar.b        = 0;
        opciones.push_back(continuar);

        Ficha volverMenu;
        volverMenu.left_x   = canvas_width/3 + 80;
        volverMenu.bottom_y = canvas_height/2.4;
        volverMenu.colocada = false;
        volverMenu.ancho    = 256;
        volverMenu.alto     = 128;
        volverMenu.r        = 0;
        volverMenu.g        = 0;
        volverMenu.b        = 1;
        opciones.push_back(volverMenu);

        Ficha salir;
        salir.left_x   = canvas_width/3 + 80;
        salir.bottom_y = canvas_height/4.2;
        salir.colocada = false;
        salir.ancho    = 256;
        salir.alto     = 128;
        salir.r        = 1;
        salir.g        = 0;
        salir.b        = 0;
        opciones.push_back(salir);

        //fichas del menu final de nivel
        Ficha otroNivel;
        otroNivel.left_x   = canvas_width/3 + 80;
        otroNivel.bottom_y = canvas_height/2;
        otroNivel.colocada = false;
        otroNivel.ancho    = 256;
        otroNivel.alto     = 128;
        otroNivel.r        = 1;
        otroNivel.g        = 0;
        otroNivel.b        = 0;
        opciones2.push_back(otroNivel);

        Ficha salirFinal;
        salirFinal.left_x   = canvas_width/3 + 80;
        salirFinal.bottom_y = canvas_height/2 - 128;
        salirFinal.colocada = false;
        salirFinal.ancho    = 256;
        salirFinal.alto     = 128;
        salirFinal.r        = 0;
        salirFinal.g        = 0;
        salirFinal.b        = 1;
        opciones2.push_back(salirFinal);

        Ficha menuPause;
        menuPause.left_x   = 1150;
        menuPause.bottom_y = canvas_height/2 - 50;
        menuPause.colocada = false;
        menuPause.ancho    = 100;
        menuPause.alto     = 100;
        menuPause.r        = 0;
        menuPause.g        = 0;
        menuPause.b        = 0;
        fichas.push_back(menuPause);

        randomFichasX();

        //propiedades comunes de las fichas jugables
        for(int i = 0; i < numero_de_cajas; i++)
        {
            fichas[i].left_x   = fichasX2[i];
            fichas[i].bottom_y = fichasY[1];
            fichas[i].colocada = false;
            fichas[i].ancho    = 128;
            fichas[i].alto     = 128;
        }

        randomRNG();
    }

    //--------------------------------------------------------------------------------------------------

    ///Inicializa las casillas
    void Sample_Scene::casillasInit()
    {
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
            casillas[i].left_x   = fichasX[i];
            casillas[i].bottom_y = fichasY[0];
            casillas[i].ancho    = 128;
            casillas[i].alto     = 128;
            casillas[i].ficha    = nullptr;
        }
    }

}




