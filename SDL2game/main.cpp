// SDL2game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CommonFunc.h"
#include "BaseObject.h"
#include "game_map.h"
#include "MainObject.h"
#include "ImpTimer.h"
#include "ThreatsObject.h"
#include "ExplosionObject.h"
#include "TextObject.h"
#include "PlayerPower.h"
#include "Geometric.h"
#include "BossObject.h"


BaseObject g_background;
BaseObject g_menu_screen;
BaseObject g_gameover;
TTF_Font* font_time = NULL;

bool InitData()
 {
	bool success = true;
	int ret = SDL_Init(SDL_INIT_VIDEO);
	if(ret < 0)
	return false;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	g_window = SDL_CreateWindow("Game Moi", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if(g_window == NULL){
		success = false;
	}
	else
	{
		g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
		if(g_screen == NULL)
		success = false;
		else{
			SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
			int imgFlags = IMG_INIT_PNG;
			if(!(IMG_Init(imgFlags) && imgFlags))
			success = false;  
		}

		if (TTF_Init() == -1)
		{
			success = false;
		}
		font_time = TTF_OpenFont("font//dlxfont_.ttf", 15);
		if (font_time == NULL)
		{
			success = false;
		}
	}
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1){
		success = false;
	}
	g_sound_bullet[0] = Mix_LoadWAV("sound//Laser.wav");
	g_sound_bullet[1] = Mix_LoadWAV("sound//Fire.wav");
	g_sound_exp[0] = Mix_LoadWAV("sound//explosion.wav");
	g_sound_exp[1] = Mix_LoadWAV("sound//Explosion+1.wav");
	
	if (g_sound_bullet[0] == NULL || g_sound_bullet[1] == NULL || g_sound_exp[0] == NULL || g_sound_exp[1] == NULL )
	{
		success = false;
	}
	return success;
 }

 bool LoadBackground()
 {
bool ret = g_background.LoadImg("img//5.png", g_screen);
if(ret == false)
return false;

return true;
 }
 bool LoadMenuGame()
 {
	 bool ret = g_menu_screen.LoadImg("img//menu.png", g_screen);
	 if(ret == false)
return false;

return true;
 }

 bool LoadGameover()
 {
	 bool ret = g_gameover.LoadImg("img//gameover.png", g_screen);
	 if (ret==false)
	 {
		 return false;
	 }
	 return true;
 }
 void close()
 {
	g_background.Free();

	 SDL_DestroyRenderer(g_screen);
	g_screen = NULL;

	SDL_DestroyWindow(g_window);
	g_window = NULL;

	IMG_Quit();
	SDL_Quit();

 }

 std::vector<ThreatsObject*> MakeThreadList()
 {
	 std::vector<ThreatsObject*> list_threats;

	 ThreatsObject* dynamic_threats = new ThreatsObject[20];
	 for (int i= 0; i < 20;i++)
	 {
		 ThreatsObject* p_threat = (dynamic_threats + i);
		 if (p_threat != NULL)
		 {
			 p_threat->LoadImg("img//left.png",g_screen);
			 p_threat->set_clips();
			 p_threat->set_type_move(ThreatsObject::MOVE_IN_SPACE_THREAT);
			 p_threat->set_x_pos(500 + i*500);
			 p_threat->set_y_pos(200);
			 int pos1 = p_threat->get_x_pos() - 60;
			 int pos2 = p_threat->get_x_pos() + 60;
			 p_threat->SetAnimationPos(pos1,pos2);
			 p_threat->set_input_left(1);
			 list_threats.push_back(p_threat);
		 }
	 }

	 ThreatsObject* threat_objs = new ThreatsObject[20];

	 for (int i = 0; i < 20; i++)
	 {
		 ThreatsObject* p_threat = (threat_objs + i);
		 if (p_threat != NULL)
		 {
			 p_threat->LoadImg("img//Attack.png",g_screen);
			 p_threat->set_clips();
			 p_threat->set_x_pos(700 + i*1200);
			 p_threat->set_y_pos(250);
			 p_threat->set_type_move(ThreatsObject::STATIC_THREAT);
			 p_threat->set_input_left(0);

			 BulletObject* p_bullet = new BulletObject();
			 p_threat->InitBullet(p_bullet, g_screen);
			 list_threats.push_back(p_threat);
		 }
	 }

	 

	 return list_threats;
 }
int main(int argc, char* argv[])
{
	
	ImpTimer fps_timer;
	if(InitData() == false)
	return -1;

	if(LoadBackground() == false)
	return -1;

	if(LoadMenuGame() == false)
		return -1;

	if(LoadGameover() == false)
		return -1;

	Mix_Chunk* game_sound = Mix_LoadWAV("sound//nhacgame.wav");
	Mix_PlayChannel(-1, game_sound, -1);

	GameMap game_map;
	game_map.LoadMap("map/map01.dat");
	game_map.LoadTiles(g_screen);



	MainObject p_player;
	p_player.LoadImg("img//player_right.png", g_screen);
	p_player.set_clips();


	PlayerPower player_power;
	player_power.Init(g_screen);

	PlayerMoney player_money;
	player_money.Init(g_screen);
	player_money.SetPos(SCREEN_WIDTH*0.5 -300, 8);

	std::vector<ThreatsObject*> threats_list = MakeThreadList();

	//Boss-Threat
	BossObject bossObject;
	bool ret = bossObject.LoadImg("img//boss_object.png", g_screen);
	bossObject.set_clips();
	int xPosBoss = MAX_MAP_X*TILE_SIZE - SCREEN_WIDTH*0.6;
	bossObject.set_xpos(xPosBoss);
	bossObject.set_ypos(10);

	ExplosionObject exp_threat;


	bool tRet = exp_threat.LoadImg("img//exp3.png", g_screen);
	if (!tRet) return -1;
	exp_threat.set_clip();


	ExplosionObject exp_main;
	bool mRet = exp_main.LoadImg("img//exp3.png", g_screen);
	
	if(!mRet) return -1;
	exp_main.set_clip();

	int num_die = 0;
	int num_boss = 0;

	//Time text
	TextObject time_game;
	time_game.SetColor(TextObject::WHITE_TEXT);


	TextObject mark_game;
	mark_game.SetColor(TextObject::WHITE_TEXT);

	UINT mark_value = 0;

	TextObject money_game;
	money_game.SetColor(TextObject::WHITE_TEXT);
	BaseObject StartButton[2];
	BaseObject ExitButton[2];
	//BaseObject ContinueButton[2];
	StartButton[0].LoadImg("img//start2.png", g_screen);
	StartButton[1].LoadImg("img//start2.png", g_screen);
	ExitButton[0].LoadImg("img//exit2.png", g_screen);
	ExitButton[1].LoadImg("img//exit2.png", g_screen);
	//ContinueButton[0].LoadImg("img//5.png", g_screen);
	//ContinueButton[1].LoadImg("img//5.png", g_screen);
	SDL_Rect Button = StartButton[0].GetRect();
	StartButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2, SCREEN_HEIGHT/2 -100);
	StartButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2, SCREEN_HEIGHT/2 -100);

	Button = ExitButton[0].GetRect();
	ExitButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2, SCREEN_HEIGHT/2 + Button.h*1.5 -125);
	ExitButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2, SCREEN_HEIGHT/2 + Button.h*1.5 -125);

	//Button = ContinueButton[0].GetRect();
	//ContinueButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2, SCREEN_HEIGHT/2 - Button.h*1.5);
	//ContinueButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2, SCREEN_HEIGHT/2 - Button.h*1.5);
	int MousePosX = 0;
	int MousePosY = 0;
	//bool play = false;
	//while(!play){
	
	SDL_Event event1; 
	bool is_quit = false;
	bool menu_quit = false;
	bool over_quit = false;
	while (!menu_quit)
	{
		SDL_RenderClear(g_screen);
		g_menu_screen.Render(g_screen, NULL);
		while (SDL_PollEvent(&g_event) != 0)
		{
			if(g_event.type == SDL_MOUSEMOTION)
			{
				MousePosX = g_event.motion.x;
				MousePosY = g_event.motion.y;
			}
			if (g_event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (g_event.button.button == SDL_BUTTON_LEFT)
				{
					if (SDLCommonFunc::CheckMousePos(MousePosX, MousePosY, StartButton[0].GetRect()) == true)
					{
						menu_quit = true;
					}
					if (SDLCommonFunc::CheckMousePos(MousePosX, MousePosY, ExitButton[0].GetRect()) == true)
					{
    					menu_quit = true;
						is_quit = true;
						//play = true;
					}
				}
			}
		}
		if (SDLCommonFunc::CheckMousePos(MousePosX, MousePosY, StartButton[0].GetRect()) == true)
		{
			StartButton[1].Render(g_screen);
		}
		else
		{
			StartButton[0].Render(g_screen);
		}
		if (SDLCommonFunc::CheckMousePos(MousePosX, MousePosY, ExitButton[0].GetRect()) == true)
		{
			ExitButton[1].Render(g_screen);
		}
		else
		{
			ExitButton[0].Render(g_screen);
		}
		SDL_RenderPresent(g_screen);
		SDL_Delay(100);
	}

	
	while(!is_quit)
	{
		fps_timer.start();
		while(SDL_PollEvent(&g_event) != 0)
		{
			if(g_event.type == SDL_QUIT)
			{
				is_quit = true;
			}

			p_player.HandelInputAction(g_event,g_screen,g_sound_bullet);
		}
		
	SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
	SDL_RenderClear(g_screen);

	 g_background.Render(g_screen, NULL);

		
	 Map map_data = game_map.getMap();

	 p_player.HandleBullet(g_screen);

	 p_player.SetMapXY(map_data.start_x_, map_data.start_y_);
	 p_player.Doplayer(map_data);
	 p_player.Show(g_screen);

	 game_map.SetMap(map_data);
	 game_map.DrawMap(g_screen);

	 //Draw geometric
	 GeometricFormat rectangle_size(0, 0, SCREEN_WIDTH, 40);
	 ColorData color_data(200, 60, 157);
	 Geometric::RenderRectangle(rectangle_size, color_data, g_screen);

	 GeometricFormat outLineSize(1, 1, SCREEN_WIDTH - 1, 38);
	 ColorData color_data2(51, 200, 75);

	 Geometric::RenderOutline(outLineSize, color_data2, g_screen);

	 player_power.Show(g_screen);
	 player_money.Show(g_screen);

	 for (int i = 0; i < threats_list.size(); i++)
	 {
		 ThreatsObject* p_threat = threats_list.at(i);
		 if (p_threat != NULL)
		 {
			 p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
			 p_threat->ImpMoveType(g_screen);
			 p_threat->DoPlayer(map_data);
			 p_threat->MakeBullet(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
			 p_threat->Show(g_screen);
			
			 SDL_Rect rect_player = p_player.GetRectFrame();
			 bool bCol1 = false;
			 std::vector<BulletObject*> tBullet_list = p_threat->get_bullet_list();
			 for (int jj = 0; jj < tBullet_list.size(); jj++)
			 {
				 BulletObject* pt_bullet =  tBullet_list.at(jj);
				 if (pt_bullet)
				 {
					 
					 bCol1 = SDLCommonFunc::CheckCollision(pt_bullet->GetRect(), rect_player);
					 if (bCol1)
					 {
						 break;
					 }
				 }
			 }

			 SDL_Rect rect_threat = p_threat->GetRectFrame();
			 bool bCol2 = SDLCommonFunc::CheckCollision(rect_player, rect_threat);
			 if (bCol1 || bCol2)
			 {
				 int width_exp_frame = exp_main.get_frame_width();
				 int heiht_exp_height = exp_main.get_frame_heigh();
				 for (int ex = 0; ex < 4; ex++)
				 {
					 int x_pos = (p_player.GetRect().x + p_player.get_frame_width()*0.5) - width_exp_frame*0.5;
					 int y_pos = (p_player.GetRect().y + p_player.get_frame_height()*0.5) - heiht_exp_height*0.5;

					 exp_main.set_frame(ex);
					 exp_main.SetRect(x_pos, y_pos);
					 exp_main.Show(g_screen);
					 SDL_RenderPresent(g_screen);
				 }
				 Mix_PlayChannel(-1, g_sound_exp[1], 0);
				 num_die++;
				 if (num_die < 3)
				 {
					 p_player.SetRect(0, 0);
					 p_player.set_comeback_time(60);
					 SDL_Delay(1000);
					 player_power.Decrease();
					 player_power.Render(g_screen);
					 continue;
				 }
				 else
				 {
					 bool i_quit = false;
	SDL_Event event;
	BaseObject GameOverMenu;
	BaseObject MainMenuButton[2];
	BaseObject ExitButton[2];
	//TTF_Font* font_time = NULL;
	
	GameOverMenu.LoadImg("img//gameover.png",g_screen);	
	MainMenuButton[0].LoadImg("img//new.png",g_screen);
	MainMenuButton[1].LoadImg("img//new.png",g_screen);
	ExitButton[0].LoadImg("img//exit2.png",g_screen);
	ExitButton[1].LoadImg("img//exit2.png",g_screen);
	SDL_Rect Button = MainMenuButton[0].GetRect();
	MainMenuButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 - 100);
	MainMenuButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 - 100);
	Button = ExitButton[0].GetRect();
	ExitButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 + Button.h*1.5 - 125);
	ExitButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 + Button.h*1.5 - 125);
	int MousePosX1 = 0;
	int MousePosY1 = 0;	
	//font_time = TTF_OpenFont("font//dlxfont_.ttf",300);
	while(!i_quit)
	{
		SDL_RenderClear(g_screen);
		GameOverMenu.Render(g_screen,NULL);
		while(SDL_PollEvent(&event) != 0)
		{

			if(event.type == SDL_MOUSEMOTION)
			{
				MousePosX1 = event.motion.x;
				MousePosY1 = event.motion.y;
			}
			if(event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,MainMenuButton[0].GetRect()) == true)
					{
						i_quit = true;
						is_quit = true;
						//play = false;
					}
					if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,ExitButton[0].GetRect()) == true)
					{
						i_quit = true;
						is_quit = true;
						
					}
				}
			}
		}
		
		if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,MainMenuButton[0].GetRect()) == true)
		{
			MainMenuButton[1].Render(g_screen);
		}
		else
		{
			MainMenuButton[0].Render(g_screen);
		}
		if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,ExitButton[0].GetRect()) == true)
		{
			ExitButton[1].Render(g_screen);
		}
		else
		{
			ExitButton[0].Render(g_screen);
		}
		SDL_RenderPresent(g_screen);
		SDL_Delay(100);
	}
	GameOverMenu.Free();
				 }
			 }
		 }
	 }

	 



	 int frame_exp_width = exp_threat.get_frame_width();
	 int frame_exp_height = exp_threat.get_frame_heigh();
	 std::vector<BulletObject*> bullet_arr = p_player.get_bullet_list();
	 for(int r = 0; r < bullet_arr.size() ; r++)
	 {
		 BulletObject* p_bullet = bullet_arr.at(r);
		 if (p_bullet != NULL)
		 {
			 for(int t = 0; t <threats_list.size();t++)
			 {
				 ThreatsObject* obj_threat = threats_list.at(t);
				 if (obj_threat != NULL)
				 {
					 SDL_Rect tRect;
					 tRect.x = obj_threat->GetRect().x;
					 tRect.y = obj_threat->GetRect().y;
					 tRect.w = obj_threat->get_width_frame();
					 tRect.h = obj_threat->get_height_frame();

					 SDL_Rect bRect = p_bullet->GetRect();

					 bool bCol = SDLCommonFunc::CheckCollision(bRect, tRect);

					 if (bCol)
					 {
						 mark_value++;
						 for (int ex = 0; ex < NUM_FRAME_EXP; ex++)
						 {
							 int x_pos = p_bullet->GetRect().x - frame_exp_width*0.5;
							 int y_pos = p_bullet->GetRect().y - frame_exp_height*0.5;

							 exp_threat.set_frame(ex);
							 exp_threat.SetRect(x_pos, y_pos);
							 exp_threat.Show(g_screen);
						 }

						 p_player.RemoveBullet(r);
						 Mix_PlayChannel(-1, g_sound_exp[0], 0);
						 obj_threat->Free();
						 threats_list.erase(threats_list.begin() + t);
					 }
				 }
			 }
		 }
	 }

	 //Show game time
	 std::string str_time = "Time: ";
	 Uint32 time_val = SDL_GetTicks() / 1000;
	 Uint32 val_time = 300 - time_val;
	 if (val_time <= 0)
	 {
		 bool i_quit = false;
	SDL_Event event;
	BaseObject GameOverMenu;
	BaseObject MainMenuButton[2];
	BaseObject ExitButton[2];
	//TTF_Font* font_time = NULL;
	GameOverMenu.LoadImg("img//gameover.png",g_screen);	
	MainMenuButton[0].LoadImg("img//new.png",g_screen);
	MainMenuButton[1].LoadImg("img//new.png",g_screen);
	ExitButton[0].LoadImg("img//exit2.png",g_screen);
	ExitButton[1].LoadImg("img//exit2.png",g_screen);
	SDL_Rect Button = MainMenuButton[0].GetRect();
	MainMenuButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2  - 100);
	MainMenuButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 - 100);
	Button = ExitButton[0].GetRect();
	ExitButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 + Button.h*1.5 - 125);
	ExitButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 + Button.h*1.5 - 125);
	int MousePosX1 = 0;
	int MousePosY1 = 0;	
	//font_time = TTF_OpenFont("font//dlxfont_.ttf",300);
	while(!i_quit)
	{
		SDL_RenderClear(g_screen);
		GameOverMenu.Render(g_screen,NULL);
		while(SDL_PollEvent(&event) != 0)
		{

			if(event.type == SDL_MOUSEMOTION)
			{
				MousePosX1 = event.motion.x;
				MousePosY1 = event.motion.y;
			}
			if(event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,MainMenuButton[0].GetRect()) == true)
					{
						i_quit = true;
						is_quit = true;
						//play = false;
					}
					if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,ExitButton[0].GetRect()) == true)
					{
						i_quit = true;
						is_quit = true;
						
					}
				}
			}
		}
		
		if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,MainMenuButton[0].GetRect()) == true)
		{
			MainMenuButton[1].Render(g_screen);
		}
		else
		{
			MainMenuButton[0].Render(g_screen);
		}
		if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,ExitButton[0].GetRect()) == true)
		{
			ExitButton[1].Render(g_screen);
		}
		else
		{
			ExitButton[0].Render(g_screen);
		}
		SDL_RenderPresent(g_screen);
		SDL_Delay(100);
	}
	GameOverMenu.Free();
				 
	 }
	 else
	 {
		 std::string str_val = std::to_string(val_time);
		 str_time += str_val;

		 time_game.SetText(str_time);
		 time_game.LoadFromrenderText(font_time, g_screen);
		 time_game.RenderText(g_screen, SCREEN_WIDTH - 200, 15);
	 }

	 std::string val_str_mark = std::to_string(mark_value);
	 std::string strMark("Mark: ");
	 strMark += val_str_mark;

	 mark_game.SetText(strMark);
	 mark_game.LoadFromrenderText(font_time, g_screen);
	 mark_game.RenderText(g_screen, SCREEN_WIDTH*0.5 - 50, 15);

	 int money_count = p_player.GetMoneyCount();
	 std::string money_str = std::to_string(money_count);

	 money_game.SetText(money_str);
	 money_game.LoadFromrenderText(font_time, g_screen);
	 money_game.RenderText(g_screen, SCREEN_WIDTH*0.5 - 250, 15);
	 
	 //Show Boss
	 int val = MAX_MAP_X*TILE_SIZE - (map_data.start_x_ + p_player.GetRect().x);
	 if (val <= SCREEN_WIDTH)
	 {
		 bossObject.SetMapXY(map_data.start_x_, map_data.start_y_);
	     bossObject.DoPlayer(map_data);
	     bossObject.MakeBullet(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
	     bossObject.Show(g_screen);

		 SDL_Rect rect_player = p_player.GetRectFrame();
		 BossObject* boss_threat = &bossObject;
		 bool bCol1 = false;
		 std::vector<BulletObject*> rBullet_list = boss_threat->get_bullet_list();
		 for(int jj=0;jj < rBullet_list.size();jj++){
			 BulletObject* bullet_boss = rBullet_list.at(jj);
			 if(bullet_boss){
				 bCol1 = SDLCommonFunc::CheckCollision(bullet_boss->GetRect(),rect_player);
				 if(bCol1){
					 break;
				 }
			 }
		 }
		 SDL_Rect rect_boss = boss_threat->GetRectFrame();
		 bool bCol2 = SDLCommonFunc::CheckCollision(rect_player, rect_boss);
		 if(bCol1||bCol2){
			 int width_exp_frame = exp_main.get_frame_width();
			 int heiht_exp_height = exp_main.get_frame_heigh();
				 for (int ex = 0; ex < 4; ex++)
				 {
					 int x_pos = (p_player.GetRect().x + p_player.get_frame_width()*0.5) - width_exp_frame*0.5;
					 int y_pos = (p_player.GetRect().y + p_player.get_frame_height()*0.5) - heiht_exp_height*0.5;

					 exp_main.set_frame(ex);
					 exp_main.SetRect(x_pos, y_pos);
					 exp_main.Show(g_screen);
					 SDL_RenderPresent(g_screen);
				 }
				 Mix_PlayChannel(-1, g_sound_exp[1], 0);
				 num_die++;
				 if (num_die < 3)
				 {
					 p_player.SetRect(0, 0);
					 p_player.set_comeback_time(60);
					 SDL_Delay(1000);
					 player_power.Decrease();
					 player_power.Render(g_screen);
					 continue;
				 }
				 else
				 {
					  bool i_quit = false;
	SDL_Event event;
	BaseObject GameOverMenu;
	BaseObject MainMenuButton[2];
	BaseObject ExitButton[2];
	//TTF_Font* font_time = NULL;
	GameOverMenu.LoadImg("img//gameover.png",g_screen);	
	MainMenuButton[0].LoadImg("img//new.png",g_screen);
	MainMenuButton[1].LoadImg("img//new.png",g_screen);
	ExitButton[0].LoadImg("img//exit2.png",g_screen);
	ExitButton[1].LoadImg("img//exit2.png",g_screen);
	SDL_Rect Button = MainMenuButton[0].GetRect();
	MainMenuButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 - 100);
	MainMenuButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 - 100);
	Button = ExitButton[0].GetRect();
	ExitButton[0].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 + Button.h*1.5 - 125);
	ExitButton[1].SetRect(SCREEN_WIDTH/2 - Button.w/2,SCREEN_HEIGHT/2 + Button.h*1.5 - 125);
	int MousePosX1 = 0;
	int MousePosY1 = 0;	
	//font_time = TTF_OpenFont("font//dlxfont_.ttf",300);
	while(!i_quit)
	{
		SDL_RenderClear(g_screen);
		GameOverMenu.Render(g_screen,NULL);
		while(SDL_PollEvent(&event) != 0)
		{

			if(event.type == SDL_MOUSEMOTION)
			{
				MousePosX1 = event.motion.x;
				MousePosY1 = event.motion.y;
			}
			if(event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,MainMenuButton[0].GetRect()) == true)
					{
						i_quit = true;
						is_quit = true;
						//play = false;
					}
					if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,ExitButton[0].GetRect()) == true)
					{
						i_quit = true;
						is_quit = true;
						
					}
				}
			}
		}
		
		if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,MainMenuButton[0].GetRect()) == true)
		{
			MainMenuButton[1].Render(g_screen);
		}
		else
		{
			MainMenuButton[0].Render(g_screen);
		}
		if(SDLCommonFunc::CheckMousePos(MousePosX1,MousePosY1,ExitButton[0].GetRect()) == true)
		{
			ExitButton[1].Render(g_screen);
		}
		else
		{
			ExitButton[0].Render(g_screen);
		}
		SDL_RenderPresent(g_screen);
		SDL_Delay(100);
	}
	GameOverMenu.Free();
				 
				 }
		 }

	 }
	 for(int r=0;r<bullet_arr.size();r++)
	 {
		 BulletObject* p_bullet = bullet_arr.at(r);
		 if(p_bullet != NULL)
		 {
			 BossObject* boss_threat = &bossObject ;
			 if(boss_threat != NULL)
			 {
				 SDL_Rect tRect;
				 tRect.x = boss_threat->GetRect().x;
				 tRect.y = boss_threat->GetRect().y;
				 tRect.w = boss_threat->get_width_frame();
				 tRect.h = boss_threat->get_height_frame();

				 SDL_Rect bRect = p_bullet->GetRect();

				 bool bCol = SDLCommonFunc::CheckCollision(bRect, tRect);
				  if (bCol)
					 {
						 mark_value++;
						 for (int ex = 0; ex < NUM_FRAME_EXP; ex++)
						 {
							int x_pos = p_bullet->GetRect().x - frame_exp_width*0.5;
							int y_pos = p_bullet->GetRect().y - frame_exp_height*0.5;

							 exp_threat.set_frame(ex);
							 exp_threat.SetRect(x_pos, y_pos);
							 exp_threat.Show(g_screen);
						 }
						 
						 p_player.RemoveBullet(r);
						 Mix_PlayChannel(-1, g_sound_exp[0], 0);
						 num_boss++;
						 if(num_boss<3){
							 continue;
						 }
						 else{
							 
				 
						 }
						 
				  }
			 }
		 }
	 }
	 
	
	

	SDL_RenderPresent(g_screen);

	int real_imp_time = fps_timer.get_ticks();
	int time_one_frame = 1000/FRAME_PER_SECOND; //ms

	if (real_imp_time < time_one_frame)
	{
		int delay_time = time_one_frame - real_imp_time;
		if(delay_time>=0)
		SDL_Delay(delay_time);
	}

	}
	///}
	for(int i=0;i<threats_list.size();i++)
	{
		ThreatsObject* p_threat = threats_list.at(i);
		if (p_threat)
		{
			p_threat->Free();
			p_threat = NULL;
		}
	}
	threats_list.clear();
    close();

	return 0;
}

