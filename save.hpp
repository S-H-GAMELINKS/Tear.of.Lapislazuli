﻿/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Tear.of.Lapislazuli
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#ifndef LINKS_SAVE_HPP_
#define LINKS_SAVE_HPP_

#include <cstdint>

extern int SAVE_CHOICE;

void SAVEDATA_DELETE();
int SAVEDATA_LOAD();
void SAVEDATA_SAVE();

void SAVESNAP();

void SCRIPT_OUTPUT_CHOICE_LOOP_SAVESNAP();
void Mouse_Move_SAVE(int MouseY);

void setSaveSnapChoice(bool b);

//通常セーブデータ
struct alignas(4) SaveData_t {
	std::int32_t ENDFLAG;    //ENDFLAG
	std::int32_t SP;			//行数
	std::int32_t CP;			//文字位置
	std::int32_t CHAR;		//立ち絵情報
	std::int32_t BG;			//背景画像情報
	std::int32_t BGM;		//BGM情報
	std::int32_t SAVE_CHOICE;//選択肢画面でのセーブ情報
	std::int32_t LapisLazuliFav; //瑠璃の好感度
	std::int32_t EventFlag;
};

#endif
