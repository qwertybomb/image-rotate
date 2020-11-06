/* standard headers */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

/* SDL2 headers */
#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>

typedef struct image
{
    uint32_t * const data;
    int32_t const width, height;
} image_t;

image_t load_png(char const *filepath)
{
    /* load a png */
    IMG_Init(IMG_INIT_PNG);
    SDL_Surface * const surface = IMG_Load(filepath);
    if(surface == NULL)
    {
        fprintf(stderr, "Error: could not open %s\n", filepath);
        exit(EXIT_FAILURE);
    }
    
    /* create an array of pixels and store the image it */
    image_t result = {
        .data = malloc(sizeof(*result.data) * surface->w * surface->h * 2),
        .width = surface->w,
        .height = surface->h
    };
    memcpy(result.data, surface->pixels, surface->w * surface->h * sizeof(*result.data));
    
    /* cleanup */
    SDL_FreeSurface(surface);	 
    IMG_Quit();

    return result;
}

int main(int argc, char **argv)
{
    /* remove warnings */
    (void)argc, (void)argv;
    image_t image = load_png("test.png");

    /* initlize SDL */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    /* create a window */
    SDL_Window * const window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    image.width, image.height, 0);

    /* create a renderer */        
    SDL_Renderer * const renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    /* create a texture */
    SDL_Texture * const texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
                                        SDL_TEXTUREACCESS_STREAMING, image.width, image.height);

    int32_t scale = 1, scale_counter = 0;
    for (bool quit = false; !quit; )
    {
        /* check for a quit event */
        for(SDL_Event event = {0}; SDL_PollEvent(&event); )
        {
            if (event.type == SDL_QUIT) 
            {
                quit = true;
            }
        }

        SDL_Delay(5);

        /* process the image */
        {

            if (scale == 1)
            {
                scale = image.width / 2;
            }
            else
            {
                memcpy (image.data + image.width * image.height, image.data, 
                        sizeof(*image.data) * image.width * image.height);
                for(int32_t y = 0; y < image.height; y += scale * 2)
                {
                    for(int32_t x = 0; x < image.width; x += scale * 2)
                    {
                        /* move the upper left block to the upper right block */
                        for(int32_t i = 0; i < scale; ++i)
                        {
                            memcpy (image.data + (i + y) * image.width + x +
                                    scale_counter + image.width * image.height,
                                    image.data + (i + y) * image.width + x,
                                    sizeof(*image.data) * scale);
                        }

                        /* move the lower right block to the lower left block */
                        for(int32_t i = 0; i < scale; ++i)
                        {
                            memcpy (image.data + (i + y + scale) * image.width + x + 
                                    (scale - scale_counter) + image.width * image.height,
                                    image.data + (i + y + scale) * image.width + x + scale,
                                    sizeof(*image.data) * scale); 
                        }
                            
                        /* move the upper right block to the lower right block */                                            
                        for(int32_t i = 0; i < scale; ++i)
                        {
                            memcpy (image.data + (i + y + scale_counter) * image.width + x + 
                                    scale + image.width * image.height,
                                    image.data + (i + y) * image.width + x + scale,
                                    sizeof(*image.data) * scale); 
                        }
                        
                        /* move the lower left block the upper left block */
                        for(int32_t i = 0; i < scale; ++i)
                        {
                            memcpy (image.data + (i + y + (scale - scale_counter)) * image.width + x + image.width * image.height,
                                    image.data + (i + y + scale) * image.width + x,
                                    sizeof(*image.data) * scale); 
                        }
                    }
                }
                
                scale_counter = scale_counter + 1 > scale ? 0 : scale_counter + 1;
                if (scale_counter == 0)
                {
                    memcpy (image.data, image.data + image.width * image.height, 
                            sizeof(*image.data) * image.width * image.height);
                    scale /= 2;
                }                
                
            }
            
        }

        /* copy the image to the texture and then draw the texture */
        SDL_UpdateTexture(texture, NULL, image.data + image.width * image.height, sizeof(*image.data) * image.width);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    /* cleanup */
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}