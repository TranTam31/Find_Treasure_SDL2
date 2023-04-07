#include "PlayerMoney.h"

PlayerMoney::PlayerMoney()
{
	x_pos_ = 0;
	y_pos_ = 0;
}

PlayerMoney::~PlayerMoney()
{

}

void PlayerMoney::Init(SDL_Renderer* screen)
{
	bool ret = LoadImg("img//money_img.png", screen);
}

void PlayerMoney::Show(SDL_Renderer* screen)
{
	rect_.x = x_pos_;
	rect_.y = y_pos_;
	Render(screen);
}
//tại sao nên xây dựng cả một hàm như này
//vì là ở đây nhé, hàm init thì đơn giản nhưng mà hàm show thì nó lại có phần render ở trong
//mà render có thể kế thừa từ BaseObject. đó
// hàm render ở BaseObject để làm gì nhể???