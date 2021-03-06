﻿/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Tear.of.Lapislazuli
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#define WIN32_LEAN_AND_MEAN

#include "utility.hpp"
#include <DxLib.h>
int MessageBoxYesNo(LPCTSTR lpText) noexcept
{
	return MessageBox(
		DxLib::GetMainWindowHandle(),
		lpText,
		"ゲームリンクス制作「瑠璃の泪」",
		MB_YESNO
	);
}
int MessageBoxOk(LPCTSTR lpText) noexcept
{
	return MessageBox(
		DxLib::GetMainWindowHandle(),
		lpText,
		"ゲームリンクス制作「瑠璃の泪」",
		MB_OK
	);
}

//画面クリア処理関数
void SCREEN_CLEAR() noexcept {
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	SetDrawScreen(DX_SCREEN_FRONT);
}
