﻿#ifndef LINKS_DEF_H_
#define LINKS_DEF_H_

#include <cstddef>

// 文字のサイズ
constexpr int font_size = 21;

//スクリプト読込数
constexpr std::size_t script_line_num_lim = 500000;
constexpr std::size_t script_line_string_len_lim = 1024;

//タイトルメニュー位置
constexpr int title_menu_game_start_pos_y = 300;
constexpr int title_menu_game_load_pos_y = 330;
constexpr int title_menu_game_config_pos_y = 360;
constexpr int title_menu_quick_load_pos_y = 390;
constexpr int title_menu_continue_pos_y = 420;
constexpr int title_menu_game_quit_pos_y = 450;

constexpr int menu_init_pos_y = 330;
constexpr int menu_pos_x = 400;

//選択肢
constexpr int choise_pos1_y = 200;
constexpr int choise_pos2_y = 230;
constexpr int choise_init_pos_y = choise_pos1_y;
constexpr int choise_pos_x = 100;

//カーソルの移動量
constexpr int cursor_move_unit = 30;

//キャラクターの位置
constexpr int charactor_pos_x = 150;
constexpr int charactor_pos_y = 130;

//キャラクター画像サイズ
constexpr int character_graph_size_x = 300;
constexpr int character_graph_size_y = 400;

//セーブ・ロード画面関係
constexpr std::size_t save_max_num = 3;
constexpr int save_base_pos_x = 400;
constexpr int save_base_pos_y = 100;
constexpr int save_move_unit = 100;
constexpr int save_buttom_y = save_base_pos_y + save_move_unit * save_max_num;
constexpr int saved_snap_draw_pos_x = 150;
constexpr int save_name_pos_x = 250;

//ゲームメニュー位置関係
constexpr int game_menu_base_pos_y = 30;

#endif //LINKS_DEF_H_
