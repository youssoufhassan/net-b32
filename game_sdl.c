// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "game_sdl.h"

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

/* **************************************************************** */

#define FONT "res/arial.ttf"
#define FONTSIZE 36
#define BACKGROUND "res/background.png"
#define ENDPOINT_N_IMG "res/endpoint_n.png"
#define ENDPOINT_E_IMG "res/endpoint_e.png"
#define ENDPOINT_S_IMG "res/endpoint_s.png"
#define ENDPOINT_W_IMG "res/endpoint_w.png"
#define CROSS_IMG "res/cross.png"
#define SEGMENT_NS_IMG "res/segment_ns.png"
#define SEGMENT_EW_IMG "res/segment_ew.png"
#define CORNER_N_IMG "res/corner_n.png"
#define CORNER_E_IMG "res/corner_e.png"
#define CORNER_S_IMG "res/corner_s.png"
#define CORNER_W_IMG "res/corner_w.png"
#define TEE_N_IMG "res/tee_n.png"
#define TEE_E_IMG "res/tee_e.png"
#define TEE_S_IMG "res/tee_s.png"
#define TEE_W_IMG "res/tee_w.png"

/* **************************************************************** */

struct Env_t {
  SDL_Texture* background;
  SDL_Texture* endpoint_n;
  SDL_Texture* endpoint_e;
  SDL_Texture* endpoint_s;
  SDL_Texture* endpoint_w;
  SDL_Texture* cross;
  SDL_Texture* segment_ns;
  SDL_Texture* segment_ew;
  SDL_Texture* corner_n;
  SDL_Texture* corner_e;
  SDL_Texture* corner_s;
  SDL_Texture* corner_w;
  SDL_Texture* tee_n;
  SDL_Texture* tee_e;
  SDL_Texture* tee_s;
  SDL_Texture* tee_w;
  SDL_Texture* text;
  SDL_Texture* congrats_text;
  game g;
  bool help;
};

/* **************************************************************** */

Env* init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]) {
  Env* env = malloc(sizeof(struct Env_t));

  if (argc != 2) {
    env->g = game_default();
  } else {
    env->g = game_load(argv[1]);
  }
  /* init background texture from PNG image */
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background) ERROR("IMG_LoadTexture: %s\n", BACKGROUND);

  /* init endpoint textures from PNG image */
  env->endpoint_n = IMG_LoadTexture(ren, ENDPOINT_N_IMG);
  if (!env->endpoint_n) ERROR("IMG_LoadTexture: %s\n", ENDPOINT_N_IMG);
  env->endpoint_e = IMG_LoadTexture(ren, ENDPOINT_E_IMG);
  if (!env->endpoint_e) ERROR("IMG_LoadTexture: %s\n", ENDPOINT_E_IMG);
  env->endpoint_s = IMG_LoadTexture(ren, ENDPOINT_S_IMG);
  if (!env->endpoint_s) ERROR("IMG_LoadTexture: %s\n", ENDPOINT_S_IMG);
  env->endpoint_w = IMG_LoadTexture(ren, ENDPOINT_W_IMG);
  if (!env->endpoint_w) ERROR("IMG_LoadTexture: %s\n", ENDPOINT_W_IMG);

  /* init cross texture from PNG image */
  env->cross = IMG_LoadTexture(ren, CROSS_IMG);
  if (!env->cross) ERROR("IMG_LoadTexture: %s\n", CROSS_IMG);

  /* init segment texture from PNG image */
  env->segment_ns = IMG_LoadTexture(ren, SEGMENT_NS_IMG);
  if (!env->segment_ns) ERROR("IMG_LoadTexture: %s\n", SEGMENT_NS_IMG);
  env->segment_ew = IMG_LoadTexture(ren, SEGMENT_EW_IMG);
  if (!env->segment_ew) ERROR("IMG_LoadTexture: %s\n", SEGMENT_EW_IMG);

  /* init corner texture from PNG image */
  env->corner_n = IMG_LoadTexture(ren, CORNER_N_IMG);
  if (!env->corner_n) ERROR("IMG_LoadTexture: %s\n", CORNER_N_IMG);
  env->corner_e = IMG_LoadTexture(ren, CORNER_E_IMG);
  if (!env->corner_e) ERROR("IMG_LoadTexture: %s\n", CORNER_E_IMG);
  env->corner_s = IMG_LoadTexture(ren, CORNER_S_IMG);
  if (!env->corner_s) ERROR("IMG_LoadTexture: %s\n", CORNER_S_IMG);
  env->corner_w = IMG_LoadTexture(ren, CORNER_W_IMG);
  if (!env->corner_w) ERROR("IMG_LoadTexture: %s\n", CORNER_W_IMG);

  /* init tee texture from PNG image */
  env->tee_n = IMG_LoadTexture(ren, TEE_N_IMG);
  if (!env->tee_n) ERROR("IMG_LoadTexture: %s\n", TEE_N_IMG);
  env->tee_e = IMG_LoadTexture(ren, TEE_E_IMG);
  if (!env->tee_e) ERROR("IMG_LoadTexture: %s\n", TEE_E_IMG);
  env->tee_s = IMG_LoadTexture(ren, TEE_S_IMG);
  if (!env->tee_s) ERROR("IMG_LoadTexture: %s\n", TEE_S_IMG);
  env->tee_w = IMG_LoadTexture(ren, TEE_W_IMG);
  if (!env->tee_w) ERROR("IMG_LoadTexture: %s\n", TEE_W_IMG);

  /* init text texture using Arial font */
  SDL_Color color = {0, 0, 255, 255}; /* blue color in RGBA */
  TTF_Font* font = TTF_OpenFont(FONT, FONTSIZE);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);
  SDL_Surface* surf = TTF_RenderText_Blended(font, "No game found !", color);
  env->text = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);

  SDL_Color red = {255, 0, 0, 255};
  SDL_Surface* congrats_surf = TTF_RenderText_Blended(font, "Congratulations !", red);
  env->congrats_text = SDL_CreateTextureFromSurface(ren, congrats_surf);
  SDL_FreeSurface(surf);
  TTF_CloseFont(font);

  /* init help menu to not shown */
  env->help = false;

  return env;
}

/* **************************************************************** */

void render(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  SDL_Rect rect;

  /* get current window size */
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  /* render background texture */
  SDL_QueryTexture(env->background, NULL, NULL, &rect.w, &rect.h);
  rect.x = w / 2 - rect.w / 2;
  rect.y = h / 2 - rect.h / 2;
  SDL_RenderCopy(ren, env->background, NULL, &rect);

  /* render text texture */
  if (env->g == NULL) {
    SDL_QueryTexture(env->text, NULL, NULL, &rect.w, &rect.h);
    rect.x = w / 2 - rect.w / 2;
    rect.y = h / 2 - rect.h / 2;
    SDL_RenderCopy(ren, env->text, NULL, &rect);
  } else {
    /* code pour afficher le menu d'aide en appuyant sur h (ne fonctionne pas)
    if (env->help){
      SDL_Window* help_menu;
      SDL_Renderer* renderer;
      SDL_Surface* surf;
      TTF_Font* font;
      SDL_Texture* help_text;
      SDL_Color color = {0, 255, 255, 255};
      SDL_Texture* background;

      if (SDL_Init(SDL_INIT_VIDEO) != 0) ERROR("Error: SDL_Init VIDEO (%s)", SDL_GetError());

      help_menu = SDL_CreateWindow("help menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 360,
    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE); if (!help_menu) ERROR("Error: SDL_CreateWindow (%s)", SDL_GetError());

      renderer = SDL_CreateRenderer(help_menu, -1, SDL_RENDERER_ACCELERATED);
      if (!renderer) ERROR("Error: SDL_CreateRenderer (%s)", SDL_GetError());
      font = TTF_OpenFont(FONT, FONTSIZE);
      if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
      TTF_SetFontStyle(font, TTF_STYLE_BOLD);
      surf = TTF_RenderText_Blended(font, "No game found !", color);
      help_text = SDL_CreateTextureFromSurface(ren, surf);
      background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 200,200);
      if (background) ERROR("SDL_CreateTexture: %s\n", "background");

      SDL_RaiseWindow(help_menu);
      SDL_Event* ev = NULL;
      bool quit = false;
      while(!quit) {
          while(SDL_PollEvent(ev)){
            if (ev->type == SDL_KEYDOWN) {
              switch (ev->key.keysym.sym){
                case SDLK_q:
                  quit = true;
                  break;
              }
            }
            if(quit) break;
          }


        SDL_QueryTexture(background, NULL, NULL, &rect.w, &rect.h);
        rect.x = w / 2 - rect.w / 2;
        rect.y = h / 2 - rect.h / 2;
        SDL_RenderCopy(renderer, background, NULL, &rect);
        SDL_QueryTexture(help_text, NULL, NULL, &rect.w, &rect.h);
        rect.x = w / 2 - rect.w / 2;
        rect.y = h / 2 - rect.h / 2;
        SDL_RenderCopy(renderer, help_text, NULL, &rect);

        SDL_Delay(DELAY);
      }
      env->help = false;
      TTF_CloseFont(font);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(help_menu);
      SDL_FreeSurface(surf);
    }
    */
    /* affichage du jeu en prenant en compte l'orientation */
    for (int i = 0; i < game_nb_rows(env->g); i++) {
      for (int j = 0; j < game_nb_cols(env->g); j++) {
        shape s = game_get_piece_shape(env->g, i, j);
        direction d = game_get_piece_orientation(env->g, i, j);
        if (s == CROSS) {
          SDL_QueryTexture(env->cross, NULL, NULL, &rect.w, &rect.h);
          rect.x = 460 + j * 160;
          rect.y = 40 + i * 160;
          SDL_RenderCopy(ren, env->cross, NULL, &rect);
        }
        if (s == TEE) {
          if (d == NORTH) {
            SDL_QueryTexture(env->tee_n, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->tee_n, NULL, &rect);
          }
          if (d == EAST) {
            SDL_QueryTexture(env->tee_e, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->tee_e, NULL, &rect);
          }
          if (d == SOUTH) {
            SDL_QueryTexture(env->tee_s, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->tee_s, NULL, &rect);
          }
          if (d == WEST) {
            SDL_QueryTexture(env->tee_w, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->tee_w, NULL, &rect);
          }
        }
        if (s == ENDPOINT) {
          if (d == NORTH) {
            SDL_QueryTexture(env->endpoint_n, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->endpoint_n, NULL, &rect);
          }
          if (d == EAST) {
            SDL_QueryTexture(env->endpoint_e, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->endpoint_e, NULL, &rect);
          }
          if (d == SOUTH) {
            SDL_QueryTexture(env->endpoint_s, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->endpoint_s, NULL, &rect);
          }
          if (d == WEST) {
            SDL_QueryTexture(env->endpoint_w, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->endpoint_w, NULL, &rect);
          }
        }
        if (s == CORNER) {
          if (d == NORTH) {
            SDL_QueryTexture(env->corner_n, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->corner_n, NULL, &rect);
          }
          if (d == EAST) {
            SDL_QueryTexture(env->corner_e, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->corner_e, NULL, &rect);
          }
          if (d == SOUTH) {
            SDL_QueryTexture(env->corner_s, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->corner_s, NULL, &rect);
          }
          if (d == WEST) {
            SDL_QueryTexture(env->corner_w, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->corner_w, NULL, &rect);
          }
        }
        if (s == SEGMENT) {
          if (d == NORTH || d == SOUTH) {
            SDL_QueryTexture(env->segment_ns, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->segment_ns, NULL, &rect);
          }
          if (d == EAST || d == WEST) {
            SDL_QueryTexture(env->segment_ew, NULL, NULL, &rect.w, &rect.h);
            rect.x = 460 + j * 160;
            rect.y = 40 + i * 160;
            SDL_RenderCopy(ren, env->segment_ew, NULL, &rect);
          }
        }
      }
    }
    /* affichage d'un message si le jeu est gagné */
    if (game_won(env->g)) {
      SDL_QueryTexture(env->congrats_text, NULL, NULL, &rect.w, &rect.h);
      rect.x = w / 2 - rect.w / 2;
      rect.y = 20;
      SDL_RenderCopy(ren, env->congrats_text, NULL, &rect);
    }
  }
}

/* **************************************************************** */

bool process(SDL_Window* win, SDL_Renderer* ren, Env* env, SDL_Event* e) {
  if (e->type == SDL_QUIT) {
    return true;
  }

  else if (e->type == SDL_MOUSEBUTTONDOWN && !game_won(env->g)) {  // évènements pris en charge pour un clic souris
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    int cell_i_index;
    int cell_j_index;
    if (mouse.x - 460 > 0) {
      cell_j_index = (mouse.x - 460) / 160;
    }
    if (mouse.y - 40 > 0) {
      cell_i_index = (mouse.y - 40) / 160;
    }
    if (cell_i_index <= game_nb_rows(env->g) && cell_j_index <= game_nb_cols(env->g) &&
        mouse.x <= 460 + game_nb_cols(env->g) * 160 && mouse.y <= 40 + game_nb_rows(env->g) * 160) {
      game_play_move(env->g, cell_i_index, cell_j_index, 1);
    }
  } else if (e->type == SDL_KEYDOWN) {  // évènements pris en charge pour une touche précise du clavier appuyée
    switch (e->key.keysym.sym) {
      case SDLK_n:
        env->g = game_random(rand() % 9 + 2, rand() % 9 + 2, rand() % 2, rand() % 2, 0);
        game_shuffle_orientation(env->g);
        break;
      case SDLK_r:
        game_shuffle_orientation(env->g);
        break;
      /* fonction solve trop lente
      case SDLK_s:
        game_solve(env->g);
        break;
      */
      case SDLK_z:
        game_undo(env->g);
        break;
      case SDLK_y:
        game_redo(env->g);
        break;
      case SDLK_p:
        game_print(env->g);
        break;
      /*
      case SDLK_h:
        env->help = true;
        break;
      */
      case SDLK_q:
        return true;
        break;
    }
  }

  return false;
}

/* **************************************************************** */

void clean(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  SDL_DestroyTexture(env->background);
  SDL_DestroyTexture(env->endpoint_n);
  SDL_DestroyTexture(env->endpoint_e);
  SDL_DestroyTexture(env->endpoint_s);
  SDL_DestroyTexture(env->endpoint_w);
  SDL_DestroyTexture(env->cross);
  SDL_DestroyTexture(env->text);
  SDL_DestroyTexture(env->congrats_text);
  SDL_DestroyTexture(env->segment_ns);
  SDL_DestroyTexture(env->segment_ew);
  SDL_DestroyTexture(env->corner_n);
  SDL_DestroyTexture(env->corner_e);
  SDL_DestroyTexture(env->corner_s);
  SDL_DestroyTexture(env->corner_w);
  SDL_DestroyTexture(env->tee_n);
  SDL_DestroyTexture(env->tee_e);
  SDL_DestroyTexture(env->tee_s);
  SDL_DestroyTexture(env->tee_w);

  free(env);
}

/* **************************************************************** */
