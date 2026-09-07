#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL3/SDL.h>

#include "bmp.h"


// Custom SDL event for when the user selects a file.
static Uint32 FILE_SELECTED_EVENT;


// Only show BMP files in the file dialog.
static const SDL_DialogFileFilter filters[] =
{
    { "Bitmap Images", "bmp" }
};


// ============================================================
// FILE DIALOG CALLBACK
// ============================================================

static void SDLCALL file_dialog_callback(
    void *userdata,
    const char * const *filelist,
    int filter)
{
    if (filelist == NULL)
    {
        SDL_Log(
            "File dialog error: %s",
            SDL_GetError()
        );

        return;
    }


    // User cancelled.
    if (filelist[0] == NULL)
    {
        SDL_Log("No file selected.");

        return;
    }


    /*
        Make our own copy of the filename.

        SDL owns filelist[0], so we should not keep
        that pointer after this callback returns.
    */
    char *filename = SDL_strdup(filelist[0]);

    if (filename == NULL)
    {
        SDL_Log("Could not copy filename.");

        return;
    }


    /*
        Send the filename back to the main event loop.
    */
    SDL_Event event;

    SDL_zero(event);

    event.type = FILE_SELECTED_EVENT;

    event.user.data1 = filename;


    if (!SDL_PushEvent(&event))
    {
        SDL_Log(
            "Could not push file event: %s",
            SDL_GetError()
        );

        SDL_free(filename);
    }
}


// ============================================================
// CHECK IF MOUSE CLICK IS INSIDE BUTTON
// ============================================================

static bool point_inside_rect(
    float x,
    float y,
    const SDL_FRect *rect)
{
    return
        x >= rect->x &&
        x <= rect->x + rect->w &&
        y >= rect->y &&
        y <= rect->y + rect->h;
}


// ============================================================
// MAIN
// ============================================================

int main(int argc, char *argv[])
{
    // --------------------------------------------------------
    // INITIALIZE SDL
    // --------------------------------------------------------

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log(
            "SDL_Init failed: %s",
            SDL_GetError()
        );

        return EXIT_FAILURE;
    }


    // --------------------------------------------------------
    // REGISTER CUSTOM EVENT
    // --------------------------------------------------------

    FILE_SELECTED_EVENT = SDL_RegisterEvents(1);

    if (FILE_SELECTED_EVENT == 0)
    {
        SDL_Log(
            "Could not register custom event: %s",
            SDL_GetError()
        );

        SDL_Quit();

        return EXIT_FAILURE;
    }


    // --------------------------------------------------------
    // CREATE MAIN WINDOW
    // --------------------------------------------------------

    SDL_Window *window = SDL_CreateWindow(
        "Image Processor",
        600,
        400,
        0
    );

    if (window == NULL)
    {
        SDL_Log(
            "Could not create window: %s",
            SDL_GetError()
        );

        SDL_Quit();

        return EXIT_FAILURE;
    }


    // --------------------------------------------------------
    // CREATE MAIN RENDERER
    // --------------------------------------------------------

    SDL_Renderer *renderer =
        SDL_CreateRenderer(
            window,
            NULL
        );

    if (renderer == NULL)
    {
        SDL_Log(
            "Could not create renderer: %s",
            SDL_GetError()
        );

        SDL_DestroyWindow(window);

        SDL_Quit();

        return EXIT_FAILURE;
    }


    SDL_WindowID main_window_id =
        SDL_GetWindowID(window);


    // --------------------------------------------------------
    // OPEN BMP BUTTON
    // --------------------------------------------------------

    SDL_FRect open_button =
    {
        .x = 200.0f,
        .y = 165.0f,
        .w = 200.0f,
        .h = 70.0f
    };


    // --------------------------------------------------------
    // MAIN LOOP
    // --------------------------------------------------------

    bool running = true;


    while (running)
    {
        SDL_Event event;


        while (SDL_PollEvent(&event))
        {
            // =================================================
            // CLOSE MAIN WINDOW
            // =================================================

            if (
                event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == main_window_id
            )
            {
                running = false;
            }


            // =================================================
            // FILE SELECTED
            // =================================================

            if (event.type == FILE_SELECTED_EVENT)
            {
                /*
                    Get the filename that was copied
                    inside file_dialog_callback().
                */

                char *filename =
                    (char *)event.user.data1;


                printf(
                    "Selected file: %s\n",
                    filename
                );


                // =============================================
                // READ BMP
                // =============================================

                bmp_image *image =
                    read_bmp(filename);


                if (image == NULL)
                {
                    fprintf(
                        stderr,
                        "Failed to read BMP: %s\n",
                        filename
                    );

                    SDL_free(filename);

                    continue;
                }


                // =============================================
                // DISPLAY BMP
                // =============================================

                /*
                    write_bmp() creates the BMP window
                    and stays inside its own loop until
                    you close that BMP window.
                */

                int result =
                    write_bmp(
                        filename,
                        image
                    );


                if (result != EXIT_SUCCESS)
                {
                    fprintf(
                        stderr,
                        "Failed to display BMP: %s\n",
                        filename
                    );
                }


                // =============================================
                // FREE BMP AFTER WINDOW CLOSES
                // =============================================

                /*
                    We only reach this point AFTER
                    write_bmp() returns.

                    Since write_bmp() returns when its
                    image window closes, this is where
                    we free the BMP.
                */

                free_bmp(image);

                image = NULL;


                /*
                    Also free our copied filename.
                */

                SDL_free(filename);

                filename = NULL;
            }


            // =================================================
            // MOUSE CLICK
            // =================================================

            if (
                event.type ==
                SDL_EVENT_MOUSE_BUTTON_DOWN
            )
            {
                /*
                    Only respond to left mouse button.
                */
                if (
                    event.button.button ==
                    SDL_BUTTON_LEFT
                )
                {
                    /*
                        Only respond to clicks in the MAIN window.
                */
                    if (
                        event.button.windowID ==
                        main_window_id
                    )
                    {
                        float mouse_x =
                            event.button.x;

                        float mouse_y =
                            event.button.y;


                        if (
                            point_inside_rect(
                                mouse_x,
                                mouse_y,
                                &open_button
                            )
                        )
                        {
                            // ================================
                            // OPEN FILE DIALOG
                            // ================================

                            SDL_ShowOpenFileDialog(
                                file_dialog_callback,

                                NULL,

                                window,

                                filters,

                                SDL_arraysize(filters),

                                NULL,

                                false
                            );
                        }
                    }
                }
            }
        }


        // ====================================================
        // DRAW MAIN WINDOW
        // ====================================================

        SDL_SetRenderDrawColor(
            renderer,
            30,
            30,
            30,
            255
        );

        SDL_RenderClear(renderer);


        // ----------------------------------------------------
        // DRAW BUTTON
        // ----------------------------------------------------

        SDL_SetRenderDrawColor(
            renderer,
            70,
            130,
            220,
            255
        );

        SDL_RenderFillRect(
            renderer,
            &open_button
        );


        // ----------------------------------------------------
        // DRAW BUTTON TEXT
        // ----------------------------------------------------

        SDL_SetRenderDrawColor(
            renderer,
            255,
            255,
            255,
            255
        );

        SDL_RenderDebugText(
            renderer,
            open_button.x + 65.0f,
            open_button.y + 30.0f,
            "Open BMP"
        );


        SDL_RenderPresent(renderer);
    }


    // --------------------------------------------------------
    // CLEANUP
    // --------------------------------------------------------

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();


    return EXIT_SUCCESS;
}