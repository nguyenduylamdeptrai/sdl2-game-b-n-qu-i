#include "stdafx.h"
#include "CommonFunc.h"
#include "BaseObject.h"
#include "TextObject.h"

bool SDLCommonFunc::CheckMousePos(const int& x, const int& y, const SDL_Rect& Obj_)
{
	return ( x >= Obj_.x && x <= Obj_.w + Obj_.x && y >= Obj_.y && Obj_.y <= Obj_.y + Obj_.h);
}

bool SDLCommonFunc::CheckCollision(const SDL_Rect& object1, const SDL_Rect& object2)
{
	int left_a = object1.x;
  int right_a = object1.x + object1.w;
  int top_a = object1.y;
  int bottom_a = object1.y + object1.h;
 
  int left_b = object2.x;
  int right_b = object2.x + object2.w;
  int top_b = object2.y;
  int bottom_b = object2.y + object2.h;
 
  // Case 1: size object 1 < size object 2
  if (left_a > left_b && left_a < right_b)
  {
    if (top_a > top_b && top_a < bottom_b)
    {
      return true;
    }
  }
 
  if (left_a > left_b && left_a < right_b)
  {
    if (bottom_a > top_b && bottom_a < bottom_b)
    {
      return true;
    }
  }
 
  if (right_a > left_b && right_a < right_b)
  {
    if (top_a > top_b && top_a < bottom_b)
    {
      return true;
    }
  }
 
  if (right_a > left_b && right_a < right_b)
  {
    if (bottom_a > top_b && bottom_a < bottom_b)
    {
      return true;
    }
  }
  
  // Case 2: size object 1 < size object 2
  if (left_b > left_a && left_b < right_a)
  {
    if (top_b > top_a && top_b < bottom_a)
    {
      return true;
    }
  }
 
  if (left_b > left_a && left_b < right_a)
  {
    if (bottom_b > top_a && bottom_b < bottom_a)
    {
      return true;
    }
  }
 
  if (right_b > left_a && right_b < right_a)
  {
    if (top_b > top_a && top_b < bottom_a)
    {
      return true;
    }
  }
 
  if (right_b > left_a && right_b < right_a)
  {
    if (bottom_b > top_a && bottom_b < bottom_a)
    {
      return true;
    }
  }
 
   // Case 3: size object 1 = size object 2
  if (top_a == top_b && right_a == right_b && bottom_a == bottom_b)
  {
    return true;
  }
 
  return false;
}
bool SDLCommonFunc::GameOverMenu(SDL_Renderer* screen)
{
	bool is_quit = false;
	SDL_Event event;
	BaseObject GameOverMenu;
	BaseObject MainMenuButton[2];
	BaseObject ExitButton[2];
	//TTF_Font* font_time = NULL;
	GameOverMenu.LoadImg("img//gameover.png",screen);	
	MainMenuButton[0].LoadImg("img//start2.png",screen);
	MainMenuButton[1].LoadImg("img//start2.png",screen);
	ExitButton[0].LoadImg("img//exit2.png",screen);
	ExitButton[1].LoadImg("img//exit2.png",screen);
	SDL_Rect Button = MainMenuButton[0].GetRect();
	MainMenuButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 - Button.h/2);
	MainMenuButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 - Button.h/2);
	Button = ExitButton[0].GetRect();
	ExitButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 + Button.h);
	ExitButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 + Button.h);
	int MousePosX = 0;
	int MousePosY = 0;	
	//font_time = TTF_OpenFont("font//dlxfont_.ttf",300);
	while(!is_quit)
	{
		SDL_RenderClear(screen);
		GameOverMenu.Render(screen,NULL);
		while(SDL_PollEvent(&event) != 0)
		{

			if(event.type == SDL_MOUSEMOTION)
			{
				MousePosX = event.motion.x;
				MousePosY = event.motion.y;
			}
			if(event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					if(SDLCommonFunc::CheckMousePos(MousePosX,MousePosY,MainMenuButton[0].GetRect()) == true)
					{
						is_quit = true;
						return false;
					}
					if(SDLCommonFunc::CheckMousePos(MousePosX,MousePosY,ExitButton[0].GetRect()) == true)
					{
						is_quit = true;
						return true;
					}
				}
			}
		}
		
		if(SDLCommonFunc::CheckMousePos(MousePosX,MousePosY,MainMenuButton[0].GetRect()) == true)
		{
			MainMenuButton[1].Render(screen);
		}
		else
		{
			MainMenuButton[0].Render(screen);
		}
		if(SDLCommonFunc::CheckMousePos(MousePosX,MousePosY,ExitButton[0].GetRect()) == true)
		{
			ExitButton[1].Render(screen);
		}
		else
		{
			ExitButton[0].Render(screen);
		}
		SDL_RenderPresent(screen);
		SDL_Delay(100);
	}
	GameOverMenu.Free();
	
}