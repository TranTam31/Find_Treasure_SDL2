#ifndef MAIN_OBJECT_H_
#define MAIN_OBJECT_H_
#include <vector>
#include "CommonFunc.h"
#include "BaseObject.h"
#include"BulletObject.h"

#define PLAYER_SPEED 6
#define MAX_FALL_SPEED 18
#define PLAYER_JUMP_VAL 18
class MainObject : public BaseObject
{
public:
	MainObject();
	~MainObject();

	SDL_Rect GetRectFrame();
 	bool LoadImg(std::string path, SDL_Renderer* screen);
	void Show(SDL_Renderer* des);
	void set_clips();


	void DoPlayer(Map& map_data);
	void CheckToMap(Map& map_data);
	void SetMapXY(const int map_x, const int map_y) { map_x_ = map_x; map_y_ = map_y; }
	void CenterEntityOnMap(Map& map_data);
	void HandelInputAction(SDL_Event events, SDL_Renderer* screen);
	
	void RemoveBullet(const int& idx);
	void set_bullet_list(std::vector<BulletObject*> bullet_list)
	{
		p_bullet_list_ = bullet_list;
	}
	std::vector<BulletObject*> get_bullet_list() const { return p_bullet_list_; }
	void HandleBullet(SDL_Renderer* des);

	void IncreaseMoney();
	int GetMoneyCount() const { return money_count; }
private:
	int money_count;
	std::vector<BulletObject*> p_bullet_list_;
	float x_val_; // một lần bấm thì đi bao nhiêu
	float y_val_;

	float x_pos_; //vị trí hiện tại tính từ vị trí đầu tiên của cả map
	float y_pos_;

	int width_frame_;
	int height_frame_;

	SDL_Rect frame_clip_[8];
	Input input_type_;
	int frame_;
	int status_;
	bool on_ground_;

	int map_x_; // map này có ý nghĩa khác với cái map kia nha, cái lày là vị trí của bản đồ trong screen đang hiển thị
	int map_y_;
};
#endif // !MAIN_OBJECT_H_

