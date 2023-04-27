#include"BaseObject.h"
#include"CommonFunc.h"
#include "game_map.h"
#include"MainObject.h"
#include "ImpTimer.h"
#include"ThreatObject.h"
#include"ExplosionObject.h"
#include"TextObject.h"
#include"PlayerMoney.h"
#include"Geometric.h"
using namespace SDLCommonFunc;

BaseObject g_background;
TTF_Font* font_time = NULL;
TTF_Font* font_time_name = NULL;

bool InitData()
{
	bool success = true;
	int ret = SDL_Init(SDL_INIT_VIDEO); // thiết lập môi trường cho SDL

	if (ret < 0) return false;
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	g_window = SDL_CreateWindow("WHERE'S TREASURE?", SDL_WINDOWPOS_UNDEFINED, 
		                         SDL_WINDOWPOS_UNDEFINED,
		                         SCREEN_WIDTH, 
		                         SCREEN_HEIGHT, 
		                         SDL_WINDOW_SHOWN); // tạo khung wimdow
	if (g_window == NULL) success = false;
	else {
		g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED); // render ra screen
		if (g_screen == NULL) success = false;
		else {
			SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR); // set màu sắc cho màn hình
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) && imgFlags)) success = false;
		}
		// text
		if (TTF_Init() == -1) success = false;
		font_time = TTF_OpenFont("font//SuperMario256.ttf", 15); // 15 là kích cỡ text
		font_time_name = TTF_OpenFont("font//SuperMario256.ttf", 35);
		if (font_time == NULL) success = false;
	}
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) success = false;
	g_sound_jump = Mix_LoadWAV("sound//jump_sound.wav");
	g_sound_bullet = Mix_LoadWAV("sound//Laser.wav");
	g_sound_exp = Mix_LoadWAV("sound//explosion.wav");
	g_sound_main = Mix_LoadWAV("sound//sound_main.wav");
	if (g_sound_bullet == NULL || g_sound_exp == NULL || g_sound_jump == NULL || g_sound_main == NULL) {
		success = false;
	}
	return success;
}


void close()
{
	g_background.Free();

	SDL_DestroyRenderer(g_screen);
	g_screen = NULL;

	IMG_Quit();
	SDL_Quit();
}

std::vector<ThreatObject*> MakeThreadListA()  // hàm này để tạo ra các Threats là con vạt
{
	std::vector<ThreatObject*> list_threats;

	ThreatObject* threats_objs = new ThreatObject[20];
	
	for (int i = 0; i < 20; i++) {
		ThreatObject* p_threat = (threats_objs + i);
		if (p_threat != NULL) {
			p_threat->LoadImg("img//zombie.png", g_screen);
			p_threat->set_clip();
			p_threat->set_x_pos(700+i*1200);
			p_threat->set_y_pos(388+64);

			list_threats.push_back(p_threat);
		}
	}
	return list_threats;
}

std::vector<ThreatObject*> MakeThreadListB()  // hàm này để tạo ra các Threats là bom
{
	std::vector<ThreatObject*> list_threats;

	ThreatObject* dynamic_threat = new ThreatObject[11];

	for (int i = 0; i < 10; i++) {
		ThreatObject* p_threat = (dynamic_threat + i);
		if (p_threat != NULL) {
			p_threat->LoadImg("img//hand_zombie.png", g_screen);
			p_threat->set_clip(); 
			p_threat->set_x_pos(2*(500 + i * 500)); // vị trí của con này trong màn hình
			p_threat->set_y_pos(390+64);

			int pos1 = p_threat->get_x_pos() - 60;
			int pos2 = p_threat->get_x_pos() + 60;
			list_threats.push_back(p_threat);
		}
	}
	return list_threats;
}

ThreatObject* MakeThreadListC()  // hàm này để tạo ra vạch đích
{
	ThreatObject* p_threat = new ThreatObject;

		//ThreatObject* p_threat = (dynamic_threat + i);
		if (p_threat != NULL) {
			p_threat->LoadImg("img//treasure.png", g_screen);
			p_threat->set_clip();
			p_threat->set_x_pos(12032); // vị trí của con này trong màn hình
			p_threat->set_y_pos(390);

			int pos1 = p_threat->get_x_pos() - 60;
			int pos2 = p_threat->get_x_pos() + 60;
		}
	return p_threat;
}

int main(int argc, char* argv[])
{
	ImpTimer fps_timer;

	if (InitData() == false) return -1;
	g_background.LoadImg("img//background.jpg", g_screen);
	Mix_PlayChannel(-1, g_sound_main, 0);

	GameMap game_map;
	game_map.LoadMap("map//map03.dat");
	game_map.LoadTiles(g_screen);

	MainObject p_player;
	p_player.LoadImg("img//player_right.png", g_screen);
	p_player.set_clips();

	// tạo ra Threat A và B
	std::vector<ThreatObject*> threats_listA = MakeThreadListA();
	std::vector<ThreatObject*> threats_listB = MakeThreadListB();
	// đây là treasure
	ThreatObject* p_threat = MakeThreadListC();

	// hình ảnh của đồng tiền
	PlayerMoney player_money;
	player_money.Init(g_screen);
	player_money.SetPos(5, 75);

	// các text money, thời gian, số lượng Threat bắn được
	TextObject time_game;
	time_game.SetColor(TextObject::WHITE_TEXT);

	TextObject mark_game;
	mark_game.SetColor(TextObject::WHITE_TEXT);
	UINT mark_value = 0;

	TextObject money_game;
	money_game.SetColor(TextObject::WHITE_TEXT);

	//tên trò chơi
	TextObject game_name;
	game_name.SetColor(TextObject::WHITE_TEXT);

	// bom nổ nha
	ExplosionObject exp_threat;
	bool tRet = exp_threat.LoadImg("img//explosion.png", g_screen);
	if (!tRet) return -1;
	exp_threat.set_clip();

	bool is_quit = false;
	while (!is_quit)
	{
		fps_timer.start();
		while (SDL_PollEvent(&g_event) != 0) {
			if (g_event.type == SDL_QUIT)
			{
				is_quit = true;
			}
			p_player.HandelInputAction(g_event, g_screen, g_sound_bullet, g_sound_jump);
		}
		SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
		SDL_RenderClear(g_screen);
		g_background.Render(g_screen, NULL);

		Map map_data = game_map.getMap();

		p_player.HandleBullet(g_screen);
		p_player.SetMapXY(map_data.start_x_, map_data.start_y_); //lấy khoảng map đang hiện trên screen
		p_player.DoPlayer(map_data);
		p_player.Show(g_screen);

		game_map.SetMap(map_data);  //đoạn này là để liên tục cập nhật lại map ở trong màn hình
		game_map.DrawMap(g_screen);

		//DrawGeometric
		GeometricFormat rectangle_size(0, 0, 120, 110); // đây là phần trong
		ColorData color_data(18, 56, 9);
		Geometric::RenderRectangle(rectangle_size, color_data, g_screen);

		GeometricFormat outLineSize(1, 1, 120 - 2, 110 - 2); // đây là phần boder ngoài
		ColorData color_data2(255, 255, 255);
		Geometric::RenderOutline(outLineSize, color_data2, g_screen);

		GeometricFormat outLineSize1(2, 2, 120 - 4, 110 - 4);
		Geometric::RenderOutline(outLineSize1, color_data2, g_screen);

		GeometricFormat outLineSize2(3, 3, 120 - 6, 110 - 6);
		Geometric::RenderOutline(outLineSize2, color_data2, g_screen);

		player_money.Show(g_screen);



		// va chạm của nhân vật và Threat
		for (int i = 0; i < threats_listA.size(); i++) {
			ThreatObject* p_threat = threats_listA.at(i);
			if (p_threat != NULL) {
				p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
				p_threat->Show(g_screen);

				// xử lý va chạm của nhân vật và Threat
				SDL_Rect rect_player = p_player.GetRectFrame();
				SDL_Rect rect_threat = p_threat->GetRectFrame();
				bool bCol2 = SDLCommonFunc::CheckCollision(rect_player, rect_threat);
				if (bCol2 == true) {
					if (MessageBox(NULL, L"--GAME OVER--", L"info", MB_OK | MB_ICONSTOP) == IDOK) {
						p_threat->Free();
						close();
						SDL_Quit();
						return 0;
					}
				}
			}
		}

		for (int i = 0; i < threats_listB.size(); i++) {
			ThreatObject* p_threat = threats_listB.at(i);
			if (p_threat != NULL) {
				p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
				p_threat->Show(g_screen);

				// xử lý va chạm của nhân vật và Threat
				SDL_Rect rect_player = p_player.GetRectFrame();
				SDL_Rect rect_threat = p_threat->GetRectFrame();
				bool bCol = SDLCommonFunc::CheckCollision(rect_player, rect_threat);
				if (bCol == true) {
					Mix_HaltChannel(-1);
					if (MessageBox(NULL, L"--GAME OVER--", L"info", MB_OK | MB_ICONSTOP) == IDOK) {
						p_threat->Free();
						close();
						SDL_Quit();
						return 0;
					}
				}
			}
		}

		// va chạm của nhân vật và cạch đích
		p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
		p_threat->Show(g_screen);
		// xử lý va chạm của nhân vật và vạch đích
		SDL_Rect rect_player = p_player.GetRectFrame();
		SDL_Rect rect_threat = p_threat->GetRectFrame();
		bool bCol = SDLCommonFunc::CheckCollision(rect_player, rect_threat);
		if (bCol == true) {
			Mix_HaltChannel(-1);
			if (MessageBox(NULL, L"--The treasure is yours!--", L"info", MB_OK | MB_ICONSTOP) == IDOK) {
				p_threat->Free();
				close();
				SDL_Quit();
				return 0;
			}
		}
		// lấy vị trí của nổ
		int frame_exp_width = exp_threat.get_frame_width();
		int frame_exp_height = exp_threat.get_frame_height();

		std::vector<BulletObject*> bullet_arr = p_player.get_bullet_list();
		for (int r = 0; r < bullet_arr.size(); ++r) {
			BulletObject* p_bullet = bullet_arr.at(r);
			if (p_bullet != NULL) {
				for (int t = 0; t < threats_listA.size(); ++t) {
					ThreatObject* obj_threat = threats_listA.at(t);
					if (obj_threat != NULL) {
						SDL_Rect tRect;
						tRect.x = obj_threat->GetRect().x; // lấy vị trí của con Threat
						tRect.y = obj_threat->GetRect().y;
						tRect.w = obj_threat->get_width_frame(); // đây là lấy vị trí chính xác của frame đang hiện thị
						tRect.h = obj_threat->get_height_frame();

						SDL_Rect bRect = p_bullet->GetRect(); // lấy vị trí của viên đạn
						bool bCol = SDLCommonFunc::CheckCollision(bRect, tRect);

						if (bCol) {
							mark_value++;
							for (int ex = 0; ex < NUM_FRAME_EXP; ++ex) {
								int x_pos = p_bullet->GetRect().x - frame_exp_width * 0.5; // đặt vị trí của nổ tại vị trí của viên đạn vừa chạm vào Threat
								int y_pos = p_bullet->GetRect().y - frame_exp_height * 0.5;

								exp_threat.set_frame(ex);
								exp_threat.SetRect(x_pos, y_pos);
								exp_threat.Show(g_screen);
							}

							p_player.RemoveBullet(r);
							Mix_PlayChannel(-1, g_sound_exp, 0);
							obj_threat->Free();
							threats_listA.erase(threats_listA.begin() + t);
						}
					}
				}
			}
		}

		//show game time
		std::string str_time = "Time: ";
		Uint32 time_val = SDL_GetTicks() / 80;

		std::string str_val = std::to_string(time_val);
		str_time += str_val;

		time_game.SetText(str_time);
		time_game.LoadFromRenderText(font_time, g_screen);
		time_game.RenderText(g_screen, 10, 15);

		// số Threat
		std::string val_str_mark = std::to_string(mark_value);
		std::string strMark("Zombie: ");
		strMark += val_str_mark;

		mark_game.SetText(strMark);
		mark_game.LoadFromRenderText(font_time, g_screen);
		mark_game.RenderText(g_screen, 10, 50);

		// số tiền
		int money_count = p_player.GetMoneyCount();
		std::string money_str = std::to_string(money_count);

		money_game.SetText(money_str);
		money_game.LoadFromRenderText(font_time, g_screen);
		money_game.RenderText(g_screen, 40, 83);

		//hiển thị tên trò chơi
		std::string str_name = "WHERE'S TREASURE ?";
		game_name.SetText(str_name);
		game_name.LoadFromRenderText(font_time_name, g_screen);
		game_name.RenderText(g_screen, SCREEN_WIDTH / 2 - 205, 25);

		// update lại màn hình
		SDL_RenderPresent(g_screen);

		// tạo độ trễ cho nhân vật di chuyển chậm lại
		int real_imp_time = fps_timer.get_tick();
		int time_one_frame = 1000 / FRAME_PER_SECOND;

		if (real_imp_time < time_one_frame) {
			int delay_time = time_one_frame - real_imp_time;
			if (delay_time >= 0) SDL_Delay(delay_time);
		}
	}

	// phần này là hủy đi các threat đã tạo ra
	for (int i = 0; i < threats_listA.size(); i++) {
		ThreatObject* p_threat = threats_listA.at(i);
		if (p_threat) {
			p_threat->Free();
			p_threat = NULL;
		}
	}
	threats_listA.clear();

	for (int i = 0; i < threats_listB.size(); i++) {
		ThreatObject* p_threat = threats_listB.at(i);
		if (p_threat) {
			p_threat->Free();
			p_threat = NULL;
		}
	}
	threats_listB.clear();
	close();
	return 0;
}