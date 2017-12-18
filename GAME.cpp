/*=============================================================================
  Copyright (c) 2017 S.H.(GAMELINKS)
  https://github.com/S-H-GAMELINKS/Tear.of.Lapislazuli
  This software is released under the MIT License, see LICENSE.
=============================================================================*/
#define WIN32_LEAN_AND_MEAN

//ライブラリとヘッダの読み込み
#include "GAME.h"
#include "DxLib.h"
#include "DEF.h"
#include <initializer_list>
#include <cassert>
#include <type_traits>
#include <string>
#include <vector>
#include <fstream>
#include "resource_manager.hpp"
#include "back_log.hpp"
#include "save.hpp"
#include "auto_skip.hpp"
#include "fmt/fmt/format.h"

// 文字列描画の位置
int DrawPointX = 0, DrawPointY = 0;

// 参照する文字列番号と文字列中の文字ポインタ
int SP = 0, CP = 0;

//スクリプト用読込配列
static std::vector<std::string> String;

//タイトル関連
int TITLE;
unsigned int Cr;
static_assert(
	std::is_same<decltype(Cr), decltype(DxLib::GetColor(std::declval<int>(), std::declval<int>(), std::declval<int>()))>::value,
	"Your DxLib version is too old."
);

//ゲームオーバー
static int GAMEOVER;

//選択肢読込変数
int ChoicePosY = choise_pos_y[0];
static std::string ChoiceStrings[2];
static constexpr const char* const ChoiceFiles[][2] = {
	{ "DATA/STR/CHOICE/A.txt", "DATA/STR/CHOICE/B.txt" },
	{ "DATA/STR/CHOICE/C.txt", "DATA/STR/CHOICE/D.txt" },
	{ "DATA/STR/CHOICE/E.txt", "DATA/STR/CHOICE/F.txt" },
	{ "DATA/STR/CHOICE/G.txt", "DATA/STR/CHOICE/H.txt" },
	{ "DATA/STR/CHOICE/I.txt", "DATA/STR/CHOICE/J.txt" },
	{ "DATA/STR/CHOICE/K.txt", "DATA/STR/CHOICE/L.txt" },
	{ "DATA/STR/CHOICE/M.txt", "DATA/STR/CHOICE/N.txt" }
};

//瑠璃の好感度
int LapislazuliFav = 0;

//エンドフラグ
int EndFlag = 99;

//イベントフラグ
int EventFlag = 0;

//ゲームメニュー変数
bool GAMEMENU_COUNT;

//既読スキップ変数
SkipData_t TextIgnoredFlags = {};

//非アクティブ用変数
static bool WindowActive = true;

// １文字分一時記憶配列
static char OneMojiBuf[3];

//キー操作
int Key[256];
int TitleMenuPosY = menu_init_pos_y;
int GAME_y = game_menu_base_pos_y;

//設定用変数
ConfigData_t ConfigData = {
	/*bgm_vol               :*/100,
	/*bgm_vol_count         :*/10,
	/*se_vol                :*/100,
	/*se_vol_count          :*/10,
	/*skip_speed            :*/100,
	/*skip_speed_count      :*/10,
	/*auto_speed            :*/100,
	/*auto_speed_count      :*/10,
	/*string_speed          :*/100,
	/*string_speed_count    :*/10,
	/*soundnovel_winownovel :*/0,
	/*mouse_key_move        :*/1
};
static int Config = 0;

//スクリーンショット用変数
static int SCREENSHOT_COUNT = 0;

//ショートカットキー用変数
static short SHORTCUT_KEY_FLAG = 0;

//セーブ・ロード関連
//クイックセーブデータ
struct alignas(4) QuickSaveData_t {
	std::int32_t ENDFLAG;    //ENDFLAG
	std::int32_t SP;			//行数
	std::int32_t CP;			//文字位置
	std::int32_t CHAR;		//立ち絵情報
	std::int32_t BG;			//背景画像情報
	std::int32_t BGM;		//BGM情報
	std::int32_t SAVE_CHOICE;//選択肢画面でのセーブ情報
	std::int32_t LapisLazuliPoint; //瑠璃の好感度
	std::int32_t EventFlag;
};

//コンティニューセーブデータ
struct alignas(4) ContinueSaveData_t {
	std::int32_t ENDFLAG;    //ENDFLAG
	std::int32_t SP;			//行数
	std::int32_t CP;			//文字位置
	std::int32_t CHAR;		//立ち絵情報
	std::int32_t BG;			//背景画像情報
	std::int32_t BGM;		//BGM情報
	std::int32_t SAVE_CHOICE;//選択肢画面でのセーブ情報
	std::int32_t LapisLazuliPoint; //瑠璃の好感度
	std::int32_t EventFlag;
};

//
// function Definition
//
//各素材データ読込関数
void MATERIAL_LOAD() {

	//サウンドデータの読み込み形式
	SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMPRESS);

	//キャラクター画像読込
	LINKS_EXPECT_TRUE(charactor.load("DATA/CHARACTER/CHAR{0:02d}.png"));

	//背景画像読込
	LINKS_EXPECT_TRUE(background.load("DATA/BACKGROUND/BG{0:02d}.png"));

	//ＢＧＭ読込
	LINKS_EXPECT_TRUE(backgroundMusic.load("DATA/BACKGROUNDMUSIC/BGM{0:02d}.ogg"));

	//ＳＥ読込
	LINKS_EXPECT_TRUE(soundEffect.load("DATA/SOUNDEFFECT/SE{0:02d}.ogg"));

	//ゲームオーバー画面
	GAMEOVER = LoadGraph("DATA/BACKGROUND/GAMEOVER.png");

	// 白色の値を取得
	Cr = GetColor(255, 255, 255);

	//タイトルメニュー背景画像読込
	TITLE = LoadGraph("DATA/BACKGROUND/TITLE.png");
}

//スクリプト読込関数
int SCRIPT_READ() {
	const char* ScriptFileNames[] = {
		"DATA/STR/LINKS.txt",
		"DATA/STR/A.txt",
		"DATA/STR/B.txt",
		"DATA/STR/C.txt",
		"DATA/STR/D.txt",
		"DATA/STR/E.txt",
		"DATA/STR/F.txt",
		"DATA/STR/G.txt",
		"DATA/STR/H.txt",
		"DATA/STR/I.txt",
		"DATA/STR/J.txt",
		"DATA/STR/K.txt",
		"DATA/STR/L.txt",
		"DATA/STR/M.txt",
		"DATA/STR/N.txt"
	};
	if (0 < EndFlag && EndFlag <= countof(ScriptFileNames)) {
		// スクリプトファイルを開く
		String.clear();
		std::ifstream file(ScriptFileNames[EndFlag - 1], std::ios_base::in);
		for (std::string buf; std::getline(file, buf); ) if(!buf.empty()) String.emplace_back(std::move(buf));
	}
	return 0;
}

//各処理後のゲーム画面の描画(サウンドノベル風)
void SOUNDNOVEL() noexcept {

	if (ConfigData.soundnovel_winownovel == 0) {

		SCREEN_CLEAR();

		//背景の表示
		background.DrawGraph(0, 0, true);
		//立ち絵の表示
		charactor.DrawGraph(charactor_pos_x, charactor_pos_y, true);
		//ＢＧＭの再生
		backgroundMusic.play(DX_PLAYTYPE_LOOP);

		DrawPointY = 0;
		DrawPointX = 0;

		if (SP != 0)
			CP = 0;

		if (SP == 0) {
			SP = 0;
			CP = 0;
		}

		//選択肢ループの場合
		if (SAVE_CHOICE == 1)
			CP = 0;
	}
}

//矢印キー操作関数
int MoveKey(int (&KeyStateBuf)[256]) {

	//キー操作が有効な場合
	if (ConfigData.mouse_key_move == 0) {

		//キー入力用変数
		char GetHitKeyStateAll_Key[256];

		//キーの情報を変数へ
		GetHitKeyStateAll(GetHitKeyStateAll_Key);

		//キー入力の状況
		for (int i = 0; i < 256; i++) {
			KeyStateBuf[i] = (GetHitKeyStateAll_Key[i] == 1) ? KeyStateBuf[i] + 1 : 0;
		}
	}
	return 0;
}

//ショートカットキー処理後の描画
void SHORTCUT_KEY_DRAW() noexcept {

	if (SHORTCUT_KEY_FLAG == 1) {

		//サウンドノベル風時の処理
		SOUNDNOVEL();

		SHORTCUT_KEY_FLAG = 0;
	}
}

//タイトルメニューカーソル関数
void title(unsigned int color, int y) {

	//カーソル
	DrawString(menu_pos_x, y, "■", color);

	//各メニュー項目
	DrawString(menu_pos_x + cursor_move_unit, title_menu_game_start_pos_y, "START", color);
	DrawString(menu_pos_x + cursor_move_unit, title_menu_game_load_pos_y, "LOAD", color);
	DrawString(menu_pos_x + cursor_move_unit, title_menu_game_config_pos_y, "CONFIG", color);
	DrawString(menu_pos_x + cursor_move_unit, title_menu_quick_load_pos_y, "QUICKLOAD", color);
	DrawString(menu_pos_x + cursor_move_unit, title_menu_continue_pos_y, "CONTINUE", color);
	DrawString(menu_pos_x + cursor_move_unit, title_menu_game_quit_pos_y, "QUIT", color);
}

//ゲームメニューカーソル関数
void GAME_MENU_CURSOR(unsigned int color, int y) {
	DrawString(save_base_pos_x - (cursor_move_unit * 6), y, "■", color);
}

namespace {
	//マウス操作(タイトルメニュー)
	void Mouse_Move_TITLE(int MouseY) {
		//タイトルメニュー
		if (EndFlag == 99) {
			TitleMenuPosY = (MouseY <= 329) ? 300
				: (MouseY <= 359) ? 330
				: (MouseY <= 389) ? 360
				: (MouseY <= 419) ? 390
				: (MouseY <= 449) ? 420
				: 450;
		}
	}

	//マウス操作(ゲームメニュー)
	void Mouse_Move_GAME(int MouseY) {
		//ゲームメニュー
		if (EndFlag == 99 || EndFlag != 99 && false == GAMEMENU_COUNT && Config == 0) {
			GAME_y = (MouseY <= 59) ? 30
				: (MouseY <= 89) ? 60
				: (MouseY <= 119) ? 90
				: (MouseY <= 149) ? 120
				: (MouseY <= 179) ? 150
				: (MouseY <= 209) ? 180
				: (MouseY <= 239) ? 210
				: (MouseY <= 269) ? 240
				: (MouseY <= 299) ? 270
				: (MouseY <= 329) ? 300
				: (MouseY <= 359) ? 330
				: 360;
		}
	}

	//マウス操作(コンフィグ)
	void Mouse_Move_CONFIG(int MouseY) {

		//コンフィグ画面
		if (Config == 1) {
			GAME_y = (MouseY <= 59) ? 30
				: (MouseY <= 89) ? 60
				: (MouseY <= 119) ? 90
				: (MouseY <= 149) ? 120
				: (MouseY <= 179) ? 150
				: (MouseY <= 209) ? 180
				: (MouseY <= 239) ? 210
				: (MouseY <= 269) ? 240
				: 270;
		}
	}

	//マウス操作(選択肢画面)
	void Mouse_Move_Choice(int MouseY) {
		//選択肢画面
		if (EndFlag == 11) {
			ChoicePosY = (MouseY <= 149) ? choise_pos_y[0]
				: (MouseY <= 199) ? choise_pos_y[1]
				: choise_pos_y[2];
		}
	}
}

//マウス操作
int Mouse_Move() {

	//マウスの位置情報変数
	int MouseX, MouseY;

	//マウスの位置を取得
	GetMousePoint(&MouseX, &MouseY);

	if (ConfigData.mouse_key_move == 1) {

		//タイトルメニュー
		Mouse_Move_TITLE(MouseY);

		//ゲームメニュー
		Mouse_Move_GAME(MouseY);

		//セーブ画面関連
		Mouse_Move_SAVE(MouseY);

		//選択肢画面
		Mouse_Move_Choice(MouseY);

		//コンフィグ画面
		Mouse_Move_CONFIG(MouseY);
	}
	return 0;
}

//SKIP_READ LOAD関数
int SKIP_READ_LOAD()
{
	FILE *fp;
#ifdef LINKS_HAS_FOPEN_S
	const errno_t er = fopen_s(&fp, "DATA/SAVE/SKIP_READ.dat", "rb");
	if (0 != er || nullptr == fp) {
		return 0;
	}
#else
	fp = fopen("DATA/SAVE/SKIP_READ.dat", "rb");
	if (nullptr == fp) {
		return 0;
	}
#endif
	fread(&TextIgnoredFlags, sizeof(SkipData_t), 1, fp);
	fclose(fp);
	return 0;
}

//SKIP_READ SAVE関数
int SKIP_READ_SAVE()
{
	FILE *fp;
#ifdef LINKS_HAS_FOPEN_S
	const errno_t er = fopen_s(&fp, "DATA/SAVE/SKIP_READ.dat", "wb");
	if (0 != er || nullptr == fp) {
		return 0;
	}
#else
	fp = fopen("DATA/SAVE/SKIP_READ.dat", "wb");//バイナリファイルを開く
	if (nullptr == fp) {//エラーが起きたらnullptrを返す
		return 0;
	}
#endif
	fwrite(&TextIgnoredFlags, sizeof(SkipData_t), 1, fp); // SkipData_t構造体の中身を出力
	fclose(fp);
	return 0;
}

//CONFIG_SAVE関数
int CONFIG_SAVE()
{
	//設定データ保存
	FILE *fp;
#ifdef LINKS_HAS_FOPEN_S
	const errno_t er = fopen_s(&fp, "DATA/SAVE/Config.dat", "wb");
	if (0 != er || nullptr == fp) {
		return 0;
	}
#else
	fp = fopen("DATA/SAVE/Config.dat", "wb");//バイナリファイルを開く
	if (nullptr == fp) {//エラーが起きたらnullptrを返す
		return 0;
	}
#endif
	fwrite(&ConfigData, sizeof(ConfigData_t), 1, fp); // ConfigData_t構造体の中身を出力
	fclose(fp);
	return 0;
}

//CONFIG_LOAD関数
int CONFIG_LOAD()
{
	//設定データの読み込み
	FILE *fp;
#ifdef LINKS_HAS_FOPEN_S
	const errno_t er = fopen_s(&fp, "DATA/SAVE/Config.dat", "rb");
	if (0 != er || nullptr == fp) {
		return 0;
	}
#else
	fp = fopen("DATA/SAVE/Config.dat", "rb");
	if (nullptr == fp) {
		return 0;
	}
#endif
	fread(&ConfigData, sizeof(ConfigData_t), 1, fp);
	fclose(fp);
	return 0;
}

namespace {
	//クイックセーブ
	int QUICKSAVE_SAVE() {
		if (IDYES == MessageBoxYesNo("クイックセーブを実行しますか？")) {

			//クイックセーブデータの作成
			QuickSaveData_t Data = { EndFlag, SP, 0, charactor.activeResource(), background.activeResource(), backgroundMusic.activeResource(), SAVE_CHOICE, LapislazuliFav, EventFlag };
			FILE *fp;
#ifdef LINKS_HAS_FOPEN_S
			const errno_t er = fopen_s(&fp, "DATA/SAVE/QUICKSAVEDATA.dat", "wb");
			if (0 != er || nullptr == fp) {
				return 0;
			}
#else
			fp = fopen("DATA/SAVE/QUICKSAVEDATA.dat", "wb");//バイナリファイルを開く
			if (nullptr == fp) {//エラーが起きたらnullptrを返す
				return 0;
			}
#endif
			fwrite(&Data, sizeof(Data), 1, fp); // SaveData_t構造体の中身を出力
			fclose(fp);
			MessageBoxOk("セーブしました！");
		}

		return 0;

	}
}

//クイックロード
int QUICKSAVE_LOAD() {
	if (IDYES == MessageBoxYesNo("クイックロードを実行しますか？")) {

		//クイックセーブデータの読み込み
		QuickSaveData_t Data;
		FILE *fp;
#ifdef LINKS_HAS_FOPEN_S
		const errno_t er = fopen_s(&fp, "DATA/SAVE/QUICKSAVEDATA.dat", "rb");
		if (0 != er || nullptr == fp) {
			return 0;
		}
#else
		fp = fopen("DATA/SAVE/QUICKSAVEDATA.dat", "rb");
		if (nullptr == fp) {
			return 0;
		}
#endif
		fread(&Data, sizeof(Data), 1, fp);
		fclose(fp);
		EndFlag = Data.ENDFLAG;
		SP = Data.SP;
		CP = Data.CP;
		charactor.activeResource(Data.CHAR);
		background.activeResource(Data.BG);
		backgroundMusic.activeResource(Data.BGM);
		SAVE_CHOICE = Data.SAVE_CHOICE;
		LapislazuliFav = Data.LapisLazuliPoint;
		EventFlag = Data.EventFlag;

		GAMEMENU_COUNT = true;

		//サウンドノベル風描画時の処理
		SOUNDNOVEL();

		MessageBoxOk("ロードしました！");
	}
	return 0;
}

namespace {
	//コンティニュー用セーブ
	int CONTINUE_SAVE() {

		//クイックセーブデータの作成
		ContinueSaveData_t Data = { EndFlag, SP, 0, charactor.activeResource(), background.activeResource(), backgroundMusic.activeResource(), SAVE_CHOICE, LapislazuliFav, EventFlag };
		FILE *fp;
#ifdef LINKS_HAS_FOPEN_S
		const errno_t er = fopen_s(&fp, "DATA/SAVE/CONTINUESAVEDATA.dat", "wb");
		if (0 != er || nullptr == fp) {
			return 0;
		}
#else
		fp = fopen("DATA/SAVE/CONTINUESAVEDATA.dat", "wb");//バイナリファイルを開く
		if (nullptr == fp) {//エラーが起きたらnullptrを返す
			return 0;
		}
#endif
		fwrite(&Data, sizeof(Data), 1, fp); // SaveData_t構造体の中身を出力
		fclose(fp);//ファイルを閉じる

		return 0;
	}
}

//コンティニュー用ロード
int CONTINUE_LOAD() {
	if (IDYES == MessageBoxYesNo("前回遊んだところから再開しますか？")) {

		//コンティニューセーブデータの読み込み
		ContinueSaveData_t Data;
		FILE *fp;
#ifdef LINKS_HAS_FOPEN_S
		const errno_t er = fopen_s(&fp, "DATA/SAVE/CONTINUESAVEDATA.dat", "rb");
		if (0 != er || nullptr == fp) {
			return 0;
		}
#else
		fp = fopen("DATA/SAVE/CONTINUESAVEDATA.dat", "rb");
		if (nullptr == fp) {
			return 0;
		}
#endif
		fread(&Data, sizeof(Data), 1, fp);
		fclose(fp);
		EndFlag = Data.ENDFLAG;
		SP = Data.SP;
		CP = Data.CP;
		charactor.activeResource(Data.CHAR);
		background.activeResource(Data.BG);
		backgroundMusic.activeResource(Data.BGM);
		SAVE_CHOICE = Data.SAVE_CHOICE;
		LapislazuliFav = Data.LapisLazuliPoint;
		EventFlag = Data.EventFlag;

		GAMEMENU_COUNT = true;

		//サウンドノベル風描画時の処理
		SOUNDNOVEL();

		MessageBoxOk("ロードしました！");
	}
	return 0;
}

namespace {
	//コンフィグ(キー操作)
	void CONFIG_KEY_MOVE() {

		//キー操作関連
		if (Key[KEY_INPUT_DOWN] == 1) {
			GAME_y += game_menu_base_pos_y;
			if (GAME_y == (game_menu_base_pos_y * 10))
				GAME_y = game_menu_base_pos_y;
		}

		if (Key[KEY_INPUT_UP] == 1) {
			GAME_y -= game_menu_base_pos_y;
			if (GAME_y == (game_menu_base_pos_y - game_menu_base_pos_y))
				GAME_y = (game_menu_base_pos_y * 9);
		}
	}

	//コンフィグ(BGM音量調節)
	void BGM_VOL_CHANGE() {

		//ＢＧＭ音量調整
		if (GAME_y == game_menu_base_pos_y && CheckHitKey(KEY_INPUT_RIGHT) == 1 || GAME_y == game_menu_base_pos_y && ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {

			WaitTimer(300);

			ConfigData.bgm_vol += 10;
			ConfigData.bgm_vol_count += 1;

			if (ConfigData.bgm_vol_count >= 10) {
				ConfigData.bgm_vol = 100;
				ConfigData.bgm_vol_count = 10;
			}
		}

		if (GAME_y == game_menu_base_pos_y && CheckHitKey(KEY_INPUT_LEFT) == 1 || GAME_y == game_menu_base_pos_y && ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0)) {

			WaitTimer(300);

			ConfigData.bgm_vol -= 10;
			ConfigData.bgm_vol_count -= 1;

			if (ConfigData.bgm_vol_count <= 0) {
				ConfigData.bgm_vol = 0;
				ConfigData.bgm_vol_count = 0;
			}
		}

	}

	//コンフィグ(SE音量調整)
	void SE_VOL_CHANGE() {

		//ＳＥ音量調整
		if (GAME_y == game_menu_base_pos_y * 2 && CheckHitKey(KEY_INPUT_RIGHT) == 1 || GAME_y == game_menu_base_pos_y * 2 && ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {

			WaitTimer(300);

			ConfigData.se_vol += 10;
			ConfigData.se_vol_count += 1;

			if (ConfigData.se_vol_count >= 10) {
				ConfigData.se_vol = 100;
				ConfigData.se_vol_count = 10;
			}
		}

		if (GAME_y == game_menu_base_pos_y * 2 && CheckHitKey(KEY_INPUT_LEFT) == 1 || GAME_y == game_menu_base_pos_y * 2 && ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0)) {

			WaitTimer(300);

			ConfigData.se_vol -= 10;
			ConfigData.se_vol_count -= 1;

			if (ConfigData.se_vol_count <= 0) {
				ConfigData.se_vol = 0;
				ConfigData.se_vol_count = 0;
			}
		}
	}

	//コンフィグ(オート速度調整)
	void AUTO_SPEED_CHANGE() {

		//オート速度調整
		if (GAME_y == game_menu_base_pos_y * 3 && CheckHitKey(KEY_INPUT_RIGHT) == 1 || GAME_y == game_menu_base_pos_y * 3 && ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {

			WaitTimer(300);

			ConfigData.auto_speed += 10;
			ConfigData.auto_speed_count += 1;

			if (ConfigData.auto_speed_count >= 10) {
				ConfigData.auto_speed = 100;
				ConfigData.auto_speed_count = 10;
			}
		}

		if (GAME_y == game_menu_base_pos_y * 3 && CheckHitKey(KEY_INPUT_LEFT) == 1 || GAME_y == game_menu_base_pos_y * 3 && ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0)) {

			WaitTimer(300);

			ConfigData.auto_speed -= 10;
			ConfigData.auto_speed_count -= 1;

			if (ConfigData.auto_speed_count <= 0) {
				ConfigData.auto_speed = 0;
				ConfigData.auto_speed_count = 0;
			}
		}
	}

	//コンフィグ(スキップ速度調整)
	void SKIP_SPEED_CHANGE() {

		//スキップ速度調整
		if (GAME_y == game_menu_base_pos_y * 4 && CheckHitKey(KEY_INPUT_RIGHT) == 1 || GAME_y == game_menu_base_pos_y * 4 && ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {

			WaitTimer(300);

			ConfigData.skip_speed += 10;
			ConfigData.skip_speed_count += 1;

			if (ConfigData.skip_speed_count >= 10) {
				ConfigData.skip_speed = 100;
				ConfigData.skip_speed_count = 10;
			}
		}

		if (GAME_y == game_menu_base_pos_y * 4 && CheckHitKey(KEY_INPUT_LEFT) == 1 || GAME_y == game_menu_base_pos_y * 4 && ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0)) {

			WaitTimer(300);

			ConfigData.skip_speed -= 10;
			ConfigData.skip_speed_count -= 1;

			if (ConfigData.skip_speed_count <= 0) {
				ConfigData.skip_speed = 0;
				ConfigData.skip_speed_count = 0;
			}

		}
	}

	//コンフィグ(文字描画)
	void STRING_SPEED_CHANGE() {

		//文字描画速度調整
		if (GAME_y == game_menu_base_pos_y * 5 && CheckHitKey(KEY_INPUT_RIGHT) == 1 || GAME_y == game_menu_base_pos_y * 5 && ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {

			WaitTimer(300);

			ConfigData.string_speed += 10;
			ConfigData.string_speed_count += 1;

			if (ConfigData.string_speed_count >= 10) {
				ConfigData.string_speed = 100;
				ConfigData.string_speed_count = 10;
			}
		}

		if (GAME_y == game_menu_base_pos_y * 5 && CheckHitKey(KEY_INPUT_LEFT) == 1 || GAME_y == game_menu_base_pos_y * 5 && ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0)) {

			WaitTimer(300);

			ConfigData.string_speed -= 10;
			ConfigData.string_speed_count -= 1;

			if (ConfigData.string_speed_count <= 0) {
				ConfigData.string_speed = 0;
				ConfigData.string_speed_count = 0;
			}
		}
	}

	//コンフィグ(サウンドノベル風とウインドウ風)
	void SOUNDNOVEL_WINDOWNOVEL_CHANGE() {

		//サウンドノベル風とウインドウ風の切り替え
		if (GAME_y == game_menu_base_pos_y * 6 && CheckHitKey(KEY_INPUT_RIGHT) == 1 || GAME_y == game_menu_base_pos_y * 6 && ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {

			WaitTimer(300);
			ConfigData.soundnovel_winownovel = 0;
		}

		if (GAME_y == game_menu_base_pos_y * 6 && CheckHitKey(KEY_INPUT_LEFT) == 1 || GAME_y == game_menu_base_pos_y * 6 && ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0)) {

			WaitTimer(300);
			ConfigData.soundnovel_winownovel = 1;
		}
	}

	//非アクティブ時の処理設定
	void WINDOWACTIVE() {

		//非アクティブ時の処理の切り替え
		if (GAME_y == game_menu_base_pos_y * 7 && CheckHitKey(KEY_INPUT_RIGHT) == 1 || GAME_y == game_menu_base_pos_y * 7 && ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {

			WaitTimer(300);
			WindowActive = false;

			//非アクティブ状態ではゲームを実行しない
			SetAlwaysRunFlag(WindowActive);
		}

		if (GAME_y == game_menu_base_pos_y * 7 && CheckHitKey(KEY_INPUT_LEFT) == 1 || GAME_y == game_menu_base_pos_y * 7 && ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0)) {

			WaitTimer(300);
			WindowActive = true;

			//非アクティブ状態でもゲームを実行
			SetAlwaysRunFlag(WindowActive);
		}
	}

	//コンフィグ(マウス/キー操作)
	void MOUSE_KEY_MOVE() {

		//マウス操作を有効に
		if (GAME_y == game_menu_base_pos_y * 8 && CheckHitKey(KEY_INPUT_RIGHT) == 1 || GAME_y == game_menu_base_pos_y * 8 && ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {

			WaitTimer(300);

			ConfigData.mouse_key_move = 1;
		}

		//キー操作を有効に
		if (GAME_y == game_menu_base_pos_y * 8 && CheckHitKey(KEY_INPUT_LEFT) == 1 || GAME_y == game_menu_base_pos_y * 8 && ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0)) {

			WaitTimer(300);

			ConfigData.mouse_key_move = 0;
		}
	}

	//タイトルに戻る
	void GAMEMENU_TITLE_BACK() {
		if (IDYES == MessageBoxYesNo("タイトル画面に戻りますか？")) {

			ClearDrawScreen();

			if (SHORTCUT_KEY_FLAG == 1) backgroundMusic.stop();

			GAMEMENU_COUNT = true;
			EndFlag = 99;
			TitleMenuPosY = menu_init_pos_y;
			disableSkip();
			charactor.reset();
			background.reset();
			backgroundMusic.reset();
		}
	}

	//ゲームに戻る
	void GAMEMENU_GAME_BACK() {
		if (IDYES == MessageBoxYesNo("ゲームに戻りますか？")) {

			GAMEMENU_COUNT = true;

			//サウンドノベル風描画時の処理
			SOUNDNOVEL();
		}
	}

	//ゲーム終了
	void GAMEMENU_GAME_FINISH() {
		if (IDYES == MessageBoxYesNo("終了しますか？")) {

			//コンティニュー用セーブ
			CONTINUE_SAVE();

			EndFlag = 99999;

			GAMEMENU_COUNT = true;
		}
	}
}

//終了ウインドウ
int GAME_FINISH() {

	if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {
		if (IDYES == MessageBoxYesNo("終了しますか？")) {

			//コンティニュー用セーブ
			CONTINUE_SAVE();

			EndFlag = 99999;

			if (false == GAMEMENU_COUNT)
				GAMEMENU_COUNT = true;
		}
		WaitTimer(300);
	}

	return 0;
}

namespace {
	//各種設定情報描画
	void CONFIG_MENU() {
		static constexpr const char* saveDataName[] = {
			"ＢＧＭ音量", "ＳＥ音量", "オート速度", "スキップ速度", "文字描画速度", "描画方法", "非アクティブ時", "マウス/キー操作",
			"戻る"
		};
		for (std::size_t i = 0; i < countof(saveDataName); ++i) {
			DrawString(save_name_pos_x, game_menu_base_pos_y * (i + 1), saveDataName[i], Cr);
		}
		DrawFormatString(save_name_pos_x + cursor_move_unit * 5, game_menu_base_pos_y, Cr, "%d", ConfigData.bgm_vol);
		DrawFormatString(save_name_pos_x + cursor_move_unit * 5, game_menu_base_pos_y * 2, Cr, "%d", ConfigData.se_vol);
		DrawFormatString(save_name_pos_x + cursor_move_unit * 5, game_menu_base_pos_y * 3, Cr, "%d", ConfigData.auto_speed);
		DrawFormatString(save_name_pos_x + cursor_move_unit * 5, game_menu_base_pos_y * 4, Cr, "%d", ConfigData.skip_speed);
		DrawFormatString(save_name_pos_x + cursor_move_unit * 5, game_menu_base_pos_y * 5, Cr, "%d", ConfigData.string_speed);
		DrawString(save_name_pos_x + cursor_move_unit * 6, game_menu_base_pos_y * 6, ((1 == ConfigData.soundnovel_winownovel) ? "ウインドウ風" : "サウンドノベル風"), Cr);
		DrawString(save_name_pos_x + cursor_move_unit * 7, game_menu_base_pos_y * 7, ((WindowActive) ? "処理" : "未処理"), Cr);
		DrawString(save_name_pos_x + cursor_move_unit * 8, game_menu_base_pos_y * 8, ((1 == ConfigData.mouse_key_move) ? "マウス操作" : "キー操作"), Cr);
	}

	//コンフィグ(タイトル/ゲームメニューへ戻る)
	void CONFIG_TITLE_BACK() {

		//タイトルに戻る/ゲームメニューに戻る
		if (GAME_y == game_menu_base_pos_y * 9 && CheckHitKey(KEY_INPUT_RETURN) == 1 || GAME_y == game_menu_base_pos_y * 9 && ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)) {
			if (IDYES == MessageBoxYesNo("戻りますか？")) {

				ClearDrawScreen();
				GAME_y = game_menu_base_pos_y;
				Config = 0;
			}
			WaitTimer(300);
		}
	}
}

//コンフィグ
void CONFIG() {
	if (IDYES == MessageBoxYesNo("設定を変更しますか？")) {

		Config = 1;

		GAME_y = game_menu_base_pos_y;

		ClearDrawScreen();

		WaitTimer(300);//キー判定消去待ち目的ではない(CONFIG画面描画の遅延処理)

		while (ProcessMessage() == 0 && MoveKey(Key) == 0 && Config == 1) {

			GAME_MENU_CURSOR(Cr, GAME_y);

			//各種設定情報描画
			CONFIG_MENU();

			//BGM音量調節
			BGM_VOL_CHANGE();

			//SE音量調整
			SE_VOL_CHANGE();

			//オート速度調整
			AUTO_SPEED_CHANGE();

			//スキップ速度調整
			SKIP_SPEED_CHANGE();

			//文字列描画速度
			STRING_SPEED_CHANGE();

			//サウンドノベル風とウインドウ風描画設定
			SOUNDNOVEL_WINDOWNOVEL_CHANGE();

			//非アクティブ時の処理設定
			WINDOWACTIVE();

			//マウス操作とキー操作設定
			MOUSE_KEY_MOVE();

			//タイトルに戻る
			CONFIG_TITLE_BACK();

			//マウス操作関連
			Mouse_Move();

			//コンフィグ(キー操作)
			CONFIG_KEY_MOVE();

			//画面クリア処理
			SCREEN_CLEAR();
		}

		//ショートカットキー時の事後処理
		SHORTCUT_KEY_DRAW();
	}
}

//選択肢機能
void sentakusi(unsigned int color, int y) {

	//カーソルの描画
	DrawString(choise_pos_x, y, "■", color);

	//選択肢の描画
	for (std::size_t i : {0, 1}) {
		DrawString(choise_pos_x + cursor_move_unit, choise_pos_y[i], ChoiceStrings[i].c_str(), color);
	}
}

// 改行関数
int Kaigyou()
{
	if (ConfigData.soundnovel_winownovel == 0) {

		int TempGraph;

		// 描画行位置を一つ下げる
		DrawPointY++;

		// 描画列を最初に戻す
		DrawPointX = 0;

		// もし画面からはみ出るなら画面をスクロールさせる
		if (DrawPointY * font_size + font_size > 480)
		{
			// テンポラリグラフィックの作成
			TempGraph = MakeGraph(640, 480);

			// 画面の内容を丸々コピーする
			GetDrawScreenGraph(0, 0, 640, 480, TempGraph);

			// 一行分上に貼り付ける
			DrawGraph(0, -font_size, TempGraph, FALSE);

			// 一番下の行の部分を黒で埋める
			DrawBox(0, 480 - font_size, 640, 480, 0, TRUE);

			// 描画行位置を一つあげる
			DrawPointY--;

			// グラフィックを削除する
			DeleteGraph(TempGraph);
		}
	}

	if (ConfigData.soundnovel_winownovel == 1) {

		// 描画行位置を一つ下げる
		DrawPointY += 20;

		// 描画列を最初に戻す
		DrawPointX = 0;
	}

	// 終了
	return 0;
}

//選択後の分岐処理関数
void ChoiceSelect(int num) {

	//マウスの左クリック判定
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) == 1) {

		//バックログ取得関数
		BACKLOG_GET();

		switch (ChoicePosY) {

			//一番上の選択肢を選んだ際の処理
			case choise_pos_y[0]:

				EndFlag = (num == 0) ? 2 : 5;
				LapislazuliFav += (num == 0) ? 2 : 1;
				break;

			//真ん中の選択肢を選んだ際の処理
			case choise_pos_y[1]:

				EndFlag = (num == 0) ? 3 : 6;
				LapislazuliFav += (num == 0) ? 1 : 2;
				break;

			//一番下の選択肢を選んだ際の処理
			case choise_pos_y[2]:

				EndFlag = (num == 0) ? 4 : 7;
				LapislazuliFav += (num == 0) ? 1 : 1;
				break;
		}
	}
}

//選択肢描画関連
void ChoiceDraw(int bg, unsigned int windowcolor, int Lapislazuli, unsigned int color) {

	//背景と選択肢部分の黒塗り
	DxLib::DrawGraph(0, 0, bg, true);
	DxLib::DrawBox(0, 0, 250, 480, windowcolor, TRUE);

	// 読みこんだグラフィックを画面左上に描画し、その下を黒塗り
	DxLib::DrawGraph(250, 0, Lapislazuli, true);
	DxLib::DrawBox(0, 350, 640, 480, windowcolor, TRUE);

	//瑠璃のセリフ
	DxLib::DrawString(300, 350, "何をしましょうか……？", color);
}

void LapislazuliFavBranchTask() {

	//中間イベントへの分岐判定
	if (3 <= LapislazuliFav && LapislazuliFav <= 4 && EventFlag == 0)
		EndFlag = 8;

	//BADエンドへの分岐判定
	if (6 <= LapislazuliFav && LapislazuliFav <= 7)
		EndFlag = 9;

	//GOODエンドへの分岐判定
	if (LapislazuliFav == 8)
		EndFlag = 10;
}

//選択肢描画ループ
void GameLoopTypeChoice() {

	const char * Choice[][3] = {
		{ "読書", "ペーパークラフト", "御茶" },
		{ "ボードゲーム", "映画", "掃除" }
	};

	int num = (LapislazuliFav >= 3) ? 1 : 0;
	int Lapislazuli = LoadGraph("DATA/CHARACTER/CHAR01.png", 0);
	int bg = LoadGraph("DATA/BACKGROUND/BG01.png");
	int windowcolor = GetColor(0, 0, 0);
	unsigned int color = GetColor(0, 0, 255);

	int TempEndFlag = EndFlag;

	SP = CP = 0;

	//瑠璃の好感度による分岐処理
	LapislazuliFavBranchTask();

	if (EndFlag == 8)
		EventFlag = 1;

	WaitTimer(300);

	//ループ
	while (ProcessMessage() == 0 && TempEndFlag == EndFlag && EndFlag != 99 && EndFlag != 99999) {

		//マウス操作
		Mouse_Move();

		//ゲーム終了
		GAME_FINISH();

		//描画関連
		ChoiceDraw(bg, windowcolor, Lapislazuli, color);

		//選択肢の表示
		for (int i = 0; i < 3; i++)
			DrawString(50, 100 + (i * 50), Choice[num][i], color);

		//選択肢カーソル
		sentakusi(color, ChoicePosY);

		//選択後の分岐処理
		ChoiceSelect(num);

		//ショートカットキー処理
		SHORTCUT_KEY();
	}
}


namespace {
	//スクリプトタグ処理(立ち絵描画)
	void SCRIPT_OUTPUT_CHARACTER_DRAW() {

		//サウンドノベル風時の処理
		if (ConfigData.soundnovel_winownovel == 0) {
			//背景画像を切り抜き、立ち絵の上にペースト
			const int charactorDummy = background.DerivationGraph(charactor_pos_x, charactor_pos_y, character_graph_size_x, character_graph_size_y);
			DxLib::DrawGraph(charactor_pos_x, charactor_pos_y, charactorDummy, true);
			DxLib::DeleteGraph(charactorDummy);
			// 読みこんだグラフィックを画面左上に描画
			charactor.DrawGraph(charactor_pos_x, charactor_pos_y, true);
		}

		//文字を進める
		CP++;
	}

	//スクリプトタグ処理(背景描画)
	void SCRIPT_OUTPUT_BACKGROUND() {

		// 読みこんだグラフィックを画面左上に描画
		background.DrawGraph(0, 0, true);

		//文字を進める
		CP++;

	}

	//スクリプトタグ処理(BGM再生)
	void SCRIPT_OUTPUT_BACKGROUNDMUSIC() {
		backgroundMusic.changeVolume(255 * ConfigData.bgm_vol / 100);
		backgroundMusic.play(DX_PLAYTYPE_LOOP);
		//文字を進める
		CP++;
	}

	//スクリプトタグ処理(SE再生)
	void SCRIPT_OUTPUT_SOUNDEFFECT() {
		soundEffect.stop();
		soundEffect.changeVolume(255 * ConfigData.se_vol / 100);
		soundEffect.play(DX_PLAYTYPE_BACK);
		//文字を進める
		CP++;
	}

	//スクリプトタグ処理(ゲーム画面のクリア処理)
	void SCRIPT_OUTPUT_SCREENCLEAR() {

		SetDrawScreen(DX_SCREEN_BACK);

		incrementBackLogCount();

		//バックログ取得関数
		BACKLOG_GET();

		// 画面を初期化して描画文字位置を初期位置に戻すおよび参照文字位置を一つ進める
		ClearDrawScreen();
		DrawPointY = 0;
		DrawPointX = 0;
		charactor.reset();
		background.reset();
		CP++;

		SetDrawScreen(DX_SCREEN_FRONT);

	}

	//スクリプトタグ処理(ゲームオーバー)
	void SCRIPT_OUTPUT_GAMEOVER() {
		background.activeResource(GAMEOVER);
		background.DrawGraph(0, 0, true);
		if (ConfigData.soundnovel_winownovel == 1) {
			static const auto windowColor = GetColor(0, 0, 0);
			DrawBox(0, 400, 640, 480, windowColor, TRUE);
		}
		CP++;
	}

	//スクリプトタグ処理(エンディング)
	void SCRIPT_OUTPUT_ENDING() {

		PlayMovie("DATA/MOVIE/ENDING.wmv", 1, DX_MOVIEPLAYTYPE_NORMAL);
		CP++;
	}

	//スクリプトタグ処理(BGM再生終了)
	void SCRIPT_OUTPUT_BGMSTOP() {
		backgroundMusic.stop();
		backgroundMusic.reset();
		CP++;
	}

	//スクリプトタグ処理(SE再生終了)
	void SCRIPT_OUTPUT_SESTOP() {
		soundEffect.stop();
		CP++;
	}

	//スクリプトタグ処理(終了文字)
	void SCRIPT_OUTPUT_END() {
		SkipDataConv* conv = reinterpret_cast<SkipDataConv*>(&TextIgnoredFlags);
		if (1 <= EndFlag && EndFlag <= countof(conv->arr)) {
			conv->arr[EndFlag] = 1;
		}
		SKIP_READ_SAVE();
		// 終了フラグを立てるおよび参照文字位置を一つ進める
		EndFlag = 99999;
		CP++;
	}

	//立ち絵クリア処理
	void SCRIPT_OUTPUT_CHARACTER_REMOVE() {
		//サウンドノベル風時の処理
		if (ConfigData.soundnovel_winownovel == 0) {
			background.DrawRectGraph(charactor_pos_x, charactor_pos_y, charactor_pos_x, charactor_pos_y, character_graph_size_x, character_graph_size_y, true);
		}

		CP++;
	}

	//キャラクター名描画処理
	void SCRIPT_OUTPUT_CHARACTER_NAME() {
			SP++;
	}

	//文字列の描画
	void SCRIPT_OUTPUT_STRING_DRAW() {
		//TODO: https://github.com/S-H-GAMELINKS/Tear.of.Lapislazuli/issues/3
		assert(std::size_t(CP + 1) <= String[SP].size());
		// １文字分抜き出す
		OneMojiBuf[0] = String[SP][CP];
		OneMojiBuf[1] = String[SP][CP + 1];
		OneMojiBuf[2] = '\0';
		static const auto charColor = GetColor(255, 255, 255);
		if (ConfigData.soundnovel_winownovel == 0) {
			// １文字描画
			DrawString(DrawPointX * font_size, DrawPointY * font_size, OneMojiBuf, charColor);
		}

		if (ConfigData.soundnovel_winownovel == 1) {
			if (DrawPointY <= 399) DrawPointY = 400;
			// １文字描画
			DrawString(DrawPointX * font_size, DrawPointY, OneMojiBuf, charColor);
		}

		// 参照文字位置を２バイト勧める
		CP += 2;

		// カーソルを一文字文進める
		DrawPointX++;
	}

	//文字列の改行
	void SCRIPT_OUTPUT_STRING_KAIGYO() {
		// 画面からはみ出たら改行する
		if (DrawPointX * font_size + font_size > 640) Kaigyou();
	}

	//サウンドノベル風時の改ページ処理
	void SCRIPT_OUTPUT_STRING_PAGE_CLEAR_SOUNDNOVEL() {

		//サウンドノベル風時の改ページ処理
		if (ConfigData.soundnovel_winownovel == 0) {

			if (DrawPointY * font_size + font_size > charactor_pos_y + font_size) {

				SetDrawScreen(DX_SCREEN_BACK);

				incrementBackLogCount();

				//バックログ取得
				BACKLOG_GET();

				// 画面を初期化して描画文字位置を初期位置に戻すおよび参照文字位置を一つ進める
				ClearDrawScreen();
				DrawPointY = 0;
				DrawPointX = 0;
				charactor.reset();
				background.reset();
				CP++;

				SetDrawScreen(DX_SCREEN_FRONT);

				WaitTimer(300);//キー判定消去待ち目的ではない(自動改ページの遅延処理)
				ClearDrawScreen();
				DrawPointY = 0;
				DrawPointX = 0;

				background.DrawGraph(0, 0, true);
				charactor.DrawGraph(charactor_pos_x, charactor_pos_y, true);
			}
		}
	}

	//動画再生処理
	void MOVIE_START() noexcept {
		assert(std::size_t(CP + 1) <= String[SP].size());
		if (isdigit(String[SP][CP]) && isdigit(String[SP][CP + 1])) {
			const size_t CharactorNumber = (ctoui(String[SP][CP]) * 10) + ctoui(String[SP][CP + 1]) - 1;
			if (99 <= CharactorNumber) return;
			try {
				DxLib::PlayMovie(
					fmt::format("DATA/MOVIE/MOVIE{0:c}{1:c}.wmv", String[SP][CP], String[SP][CP + 1]).c_str(),
					1,
					DX_MOVIEPLAYTYPE_BCANCEL
				);
			}
#if defined(_MSC_VER) && defined(_DEBUG)
			catch (const std::exception& er) {
				//例外が投げられた場合特にできることがないので握りつぶす
				OutputDebugStringA(er.what());
			}
#endif
			catch (...) {}
			CP += 2;
		}
	}

	//コメント処理
	void COMMENT() {

		switch (String[SP][CP]) {

		case '/':

			SP++;
			CP = 0;
			break;
		}
	}
}

//スクリプトタグ処理(メイン)関数
int SCRIPT_OUTPUT() {
	assert(std::size_t(CP + 1) <= String[SP].size());
	switch (String[SP][CP])
	{

		//キャラクター描画処理
	case 'C':
		CP++;
		if(charactor.select(String[SP][CP], String[SP][CP + 1])) CP++;
		//キャラクター描画
		SCRIPT_OUTPUT_CHARACTER_DRAW();
		break;

		//背景描画処理
	case 'B':
		CP++;
		if (background.select(String[SP][CP], String[SP][CP + 1])) CP++;
		//背景描画
		SCRIPT_OUTPUT_BACKGROUND();
		break;

		//BGM再生処理
	case 'M':
		CP++;
		backgroundMusic.stop();
		if (backgroundMusic.select(String[SP][CP], String[SP][CP + 1])) CP++;
		//BGM再生処理
		SCRIPT_OUTPUT_BACKGROUNDMUSIC();
		break;

		//SE再生処理
	case 'S':
		CP++;
		soundEffect.stop();
		if (soundEffect.select(String[SP][CP], String[SP][CP + 1])) CP++;
		//SE再生処理
		SCRIPT_OUTPUT_SOUNDEFFECT();
		break;

		//ムービー再生処理
	case 'V':

		CP++;

		MOVIE_START();
		break;

		// 改行文字
	case 'L':

		// 改行処理および参照文字位置を一つ進める
		Kaigyou();
		CP++;
		break;

		// ボタン押し待ち文字
	case 'P':

		//クリック待ち処理
		SCRIPT_UTPUT_KEYWAIT();
		break;

		// クリア文字
	case 'R':

		//ゲーム画面のクリア処理
		SCRIPT_OUTPUT_SCREENCLEAR();
		break;

		//少し待つ
	case 'W':

		//スクリプトタグ処理(少し待つ)
		SCRIPT_OUTPUT_WAIT();
		break;

		//ゲームオーバー
	case 'G':

		//ゲームオーバー画面処理
		SCRIPT_OUTPUT_GAMEOVER();
		break;

		//エンディング
	case 'F':

		//エンディング再生
		SCRIPT_OUTPUT_ENDING();
		break;

		//BGMの再生を止める
	case 'O':

		//BGMの再生を止める
		SCRIPT_OUTPUT_BGMSTOP();
		break;

		//SEの再生を止める
	case 'Q':

		//SEの再生を止める
		SCRIPT_OUTPUT_SESTOP();
		break;

		//選択肢の表示
	case 'D':

		if (1 <= EndFlag && EndFlag <= 4)
			EndFlag = 11;

		if (EndFlag == 8)
			EndFlag = 11;

		if (5 <= EndFlag && EndFlag <= 7)
			EndFlag = 11;

		break;

		// 終了文字
	case 'E':

		//スクリプトタグ処理(終了文字)
		SCRIPT_OUTPUT_END();
		break;

		//立ち絵消しタグ
	case '@':

		//立ち絵クリア処理
		SCRIPT_OUTPUT_CHARACTER_REMOVE();
		break;

		//ウインドウ風キャラクター名描画タグ
	case '#':

		//キャラクター名描画処理
		SCRIPT_OUTPUT_CHARACTER_NAME();
		break;

		//コメントタグ
	case '/' :

		CP++;

		//コメントタグ処理
		COMMENT();
		break;

	case ' ':
		CP++;
		break;

	default:	// その他の文字

				//文字列の描画処理
		SCRIPT_OUTPUT_STRING_DRAW();

		//文字列の描画速度
		SCRIPT_OUTPUT_STRING_DRAW_SPEED();

		//文字列の描画速度
		SCRIPT_OUTPUT_STRING_KAIGYO();

		//サウンドノベル風時の改ページ処理
		SCRIPT_OUTPUT_STRING_PAGE_CLEAR_SOUNDNOVEL();

		break;
	}
	return 0;
}

//参照文字列処理
void WORD_FORMAT() {

	// 参照文字列の終端まで行っていたら参照文字列を進める
	if (0 < CP && String[SP].size() == std::size_t(CP))
	{
		SP++;
		CP = 0;
	}
}

//初期化
int FORMAT() {

	// 描画位置の初期位置セット（横）
	DrawPointX = 0;
	DrawPointY = 0;

	// 参照文字位置をセット
	SP = 0;	// １行目の
	CP = 0;	// ０文字
	return 0;
}

//スクリーンショット機能
int SCREENSHOT() {
	if (1 != CheckHitKey(KEY_INPUT_F12)) return 0;

	if (SCREENSHOT_COUNT < 0 || 9 < SCREENSHOT_COUNT) {
		MessageBoxOk("これ以上スクリーンショットを取得できません");
	}
	else {
		static constexpr const char* const ScreenShotFileNames[] = {
			"DATA/SCREENSHOT/SCREENSHOT01.png",
			"DATA/SCREENSHOT/SCREENSHOT02.png",
			"DATA/SCREENSHOT/SCREENSHOT03.png",
			"DATA/SCREENSHOT/SCREENSHOT04.png",
			"DATA/SCREENSHOT/SCREENSHOT05.png",
			"DATA/SCREENSHOT/SCREENSHOT06.png",
			"DATA/SCREENSHOT/SCREENSHOT07.png",
			"DATA/SCREENSHOT/SCREENSHOT08.png",
			"DATA/SCREENSHOT/SCREENSHOT09.png",
			"DATA/SCREENSHOT/SCREENSHOT10.png",
		};
		static constexpr const char* const ScreenShotMessages[] = {
			"スクリーンショット０１を取得しました！",
			"スクリーンショット０２を取得しました！",
			"スクリーンショット０３を取得しました！",
			"スクリーンショット０４を取得しました！",
			"スクリーンショット０５を取得しました！",
			"スクリーンショット０６を取得しました！",
			"スクリーンショット０７を取得しました！",
			"スクリーンショット０８を取得しました！",
			"スクリーンショット０９を取得しました！",
			"スクリーンショット１０を取得しました！",
		};
		static_assert(countof(ScreenShotFileNames) == countof(ScreenShotMessages), "invalid array size.");
		SaveDrawScreenToPNG(0, 0, 640, 480, ScreenShotFileNames[SCREENSHOT_COUNT], 0);
		MessageBoxOk(ScreenShotMessages[SCREENSHOT_COUNT]);
		++SCREENSHOT_COUNT;
	}
	WaitTimer(300);
	return 0;
}

//各種F1～F11キー
void SHORTCUT_KEY() {

	//セーブ
	if (EndFlag != 99 && CheckHitKey(KEY_INPUT_F1) == 1) {
		SHORTCUT_KEY_FLAG = 1;
		GAMEMENU_COUNT = false;
		SAVEDATA_SAVE();
	}

	//ロード
	if (EndFlag != 99 && CheckHitKey(KEY_INPUT_F2) == 1) {
		SHORTCUT_KEY_FLAG = 1;
		GAMEMENU_COUNT = false;
		SAVEDATA_LOAD();
	}

	//セーブデータ削除
	if (EndFlag != 99 && CheckHitKey(KEY_INPUT_F3) == 1) {
		SHORTCUT_KEY_FLAG = 1;
		GAMEMENU_COUNT = false;
		SAVEDATA_DELETE();
	}

	//既読スキップ
	if (EndFlag != 99 && CheckHitKey(KEY_INPUT_F4) == 1) {
		SHORTCUT_KEY_FLAG = 1;
		GAMEMENU_COUNT = false;
		SKIP_READ_LOAD();
		SKIP_READ_CHECK();
	}

	//スキップ
	if (EndFlag != 99 && CheckHitKey(KEY_INPUT_F5) == 1) {
		SHORTCUT_KEY_FLAG = 1;
		GAMEMENU_COUNT = false;
		SKIP_START();
	}

	//オート
	if (EndFlag != 99 && CheckHitKey(KEY_INPUT_F6) == 1) {
		SHORTCUT_KEY_FLAG = 1;
		GAMEMENU_COUNT = false;
		AUTO_START();
	}

	//スキップ&オート停止
	if (EndFlag != 99 && CheckHitKey(KEY_INPUT_F7) == 1) {
		SHORTCUT_KEY_FLAG = 1;
		GAMEMENU_COUNT = false;
		AUTO_SKIP_STOP();
	}

	//バックログ
	if (EndFlag != 99 && CheckHitKey(KEY_INPUT_F8) == 1) {
		SHORTCUT_KEY_FLAG = 1;
		GAMEMENU_COUNT = false;
		BACKLOG_DRAW();
	}

	//設定
	if (EndFlag != 99 && CheckHitKey(KEY_INPUT_F9) == 1) {
		SHORTCUT_KEY_FLAG = 1;
		GAMEMENU_COUNT = false;
		CONFIG();
	}

	//クイックセーブ
	if (EndFlag != 99 && CheckHitKey(KEY_INPUT_F10) == 1) {
		SHORTCUT_KEY_FLAG = 1;
		GAMEMENU_COUNT = false;
		QUICKSAVE_SAVE();
	}
}
