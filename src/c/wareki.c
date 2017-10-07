/*
 WAREKI
 Displey Japanese Year calendar 
 
 ahiru studio
 
 2014/01/25 Ver.1.0.0
 2014/10/02 Ver.1.1  10gatsu -> 0gatsu :  Bug
 2014/10/02 Ver.1.2  SDK 2.6.1 
 2015/03/01 Ver.1.3  SDK 2 add YAMA no Hi at 2016/08/11 
 2015/03/09 Ver.2.0  SDK 3 
 2015/11/14 Ver.2.1  SDK 3
 	カラー化
 2015/12/17 Ver.2.2  SDK 3.8 Battery state , Round (APLITE FW3.8)
	バッテリー表示を追加 Round対応

 カラーと白黒では画像の載せ方が違うから別にした方がいいかな？
 -> 2015/12 Pebble ClassicもFW3.8になったので同じになった
 
 2016/09/04 Ver.3.0  SDK 4
 	官報より2017年祝日を設定
  	SDK4 pebble2 対応
	初代pebble は対象外とする。
 
  2016/09/05 Ver.3.1  SDK 4
	バッテリーチェックの値の変更
  2016/09/11 Ver.3.2  SDK 4
	QuickViewの最中にTLを操作したりした時に表示が乱れる対応。
  2016/10/29 Ver.3.3  SDK 4
	アプリアイコン(メニューアイコン)を25x25に変更
	バッテリーのpluggedに対応。
  2016/10/30 Ver.3.4  SDK 4
	充電中にバージョンを表示する。デバッグ用。
	
  2017/10/08 Ver.3.5  SDK 4
	官報より2018年祝日を設定
 */

// Standard includes
#include <pebble.h>
#include "wareki.h"
#include "holiday.h"

static bool mydebug = false ; /* true | false */ 
static char appversion[] = "Ver.3.5";

#ifdef PBL_ROUND  
// Pebble Time Round ( 180x180 )用の座標系        
static int   x1 = 26 + 4;
static int   x2 = 26 + 26;
static int   x3 = 26 + 32;
static int   x4 = 26 + 26;
//static int   y = 16 + 3;  
#else
// Pebble Time ( 144x160 )用の座標系        
static int   x1 = 26;
static int   x2 = 26;
static int   x3 = 26;
static int   x4 = 26;
//static int   y = 16;
#endif

// Background color : GColorCeleste / GColorWhite
// Time Bar color   : GColorOxfordBlue / GColorBlack

// バッテリー表示の無駄な更新を減らすためのフラグ
// 10:充電中 0-9:バッテリー残量 初期値9
static int   last_battery_status = 9;

//########################################
//# destroy year layer
//# 平成28年
//########################################
static void destroy_year_layers(){
	layer_remove_from_parent(bitmap_layer_get_layer(year10_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(year01_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(heisei_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(year_layer));
	bitmap_layer_destroy(year10_layer);
	bitmap_layer_destroy(year01_layer);
	bitmap_layer_destroy(heisei_layer);
	bitmap_layer_destroy(year_layer);
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_year_layers 1.");
	}
	gbitmap_destroy(year10_image);
	gbitmap_destroy(year01_image);
//	gbitmap_destroy(heisei_image);
//	gbitmap_destroy(year_image);
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_year_layers END.");
	}
}
//########################################
//# create year layer
//# 平成28年
//########################################
static void create_year_layers(int line_y){
	heisei_layer  = bitmap_layer_create(GRect( 8+x1,line_y,48,22));
	year10_layer  = bitmap_layer_create(GRect(58+x1,line_y,14,22));
	year01_layer  = bitmap_layer_create(GRect(74+x1,line_y,14,22));
	year_layer    = bitmap_layer_create(GRect(90+x1,line_y,22,22));
	bitmap_layer_set_bitmap(heisei_layer,heisei_image);
	bitmap_layer_set_bitmap(year10_layer,year10_image);
	bitmap_layer_set_bitmap(year01_layer,year01_image);
	bitmap_layer_set_bitmap(year_layer,  year_image);
	bitmap_layer_set_alignment(year10_layer,GAlignLeft);
	bitmap_layer_set_alignment(year01_layer,GAlignLeft);
	bitmap_layer_set_alignment(heisei_layer,GAlignLeft);
	bitmap_layer_set_alignment(year_layer,  GAlignLeft);
	bitmap_layer_set_compositing_mode(heisei_layer,GCompOpSet);
	bitmap_layer_set_compositing_mode(year10_layer,GCompOpSet);
	bitmap_layer_set_compositing_mode(year01_layer,GCompOpSet);
	bitmap_layer_set_compositing_mode(year_layer,  GCompOpSet);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(heisei_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(year10_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(year01_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(year_layer));
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "create_year_layers END.");
	}
}

//########################################
//# destroy date layer ex: 11月14日
//########################################
static void destroy_date_layers(){
	layer_remove_from_parent(bitmap_layer_get_layer(month10_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(month01_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(day10_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(day01_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(month_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(day_layer));
	bitmap_layer_destroy(month10_layer);
	bitmap_layer_destroy(month01_layer);
	bitmap_layer_destroy(day10_layer);
	bitmap_layer_destroy(day01_layer);
	bitmap_layer_destroy(month_layer);
	bitmap_layer_destroy(day_layer);
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_date_layers 1.");
	}
	gbitmap_destroy(month10_image);
	gbitmap_destroy(month01_image);
	gbitmap_destroy(day10_image);
	gbitmap_destroy(day01_image);
//	gbitmap_destroy(month_image);
//	gbitmap_destroy(day_image);
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_date_layers END.");
	}
}
//########################################
//# create date layer ex: 11月14日
//########################################
static void create_date_layers(int line_y){
	month10_layer = bitmap_layer_create(GRect( 2+x2,line_y,14,22));
	month01_layer = bitmap_layer_create(GRect(18+x2,line_y,14,22));
	month_layer   = bitmap_layer_create(GRect(32+x2,line_y,22,22));
	day10_layer   = bitmap_layer_create(GRect(58+x2,line_y,14,22));
	day01_layer   = bitmap_layer_create(GRect(74+x2,line_y,14,22));
	day_layer     = bitmap_layer_create(GRect(92+x2,line_y,22,22));
	bitmap_layer_set_bitmap(month10_layer, month10_image);
	bitmap_layer_set_bitmap(month01_layer, month01_image);
	bitmap_layer_set_bitmap(day10_layer,   day10_image);
	bitmap_layer_set_bitmap(day01_layer,   day01_image);
	bitmap_layer_set_bitmap(month_layer,   month_image);
	bitmap_layer_set_bitmap(day_layer,     day_image);
	bitmap_layer_set_alignment(month10_layer, GAlignLeft);
	bitmap_layer_set_alignment(month01_layer, GAlignLeft);
	bitmap_layer_set_alignment(day10_layer,   GAlignLeft);
	bitmap_layer_set_alignment(day01_layer,   GAlignLeft);
	bitmap_layer_set_alignment(month_layer,   GAlignLeft);
	bitmap_layer_set_alignment(day_layer,     GAlignLeft);
	bitmap_layer_set_compositing_mode(month10_layer, GCompOpSet);
	bitmap_layer_set_compositing_mode(month01_layer, GCompOpSet);
	bitmap_layer_set_compositing_mode(day10_layer,   GCompOpSet);
	bitmap_layer_set_compositing_mode(day01_layer,   GCompOpSet);
	bitmap_layer_set_compositing_mode(month_layer,   GCompOpSet);
	bitmap_layer_set_compositing_mode(day_layer,     GCompOpSet);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(month10_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(month01_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(day10_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(day01_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(month_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(day_layer));
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "create_date_layers END.");
	}
}

//########################################
//# destroy wday layer  ex: 土曜日
//########################################
static void destroy_wday_layers(){
	layer_remove_from_parent(bitmap_layer_get_layer(wday_layer));
	bitmap_layer_destroy(wday_layer);
	gbitmap_destroy(wday_image);
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_wday_layers END.");
	}
}
//########################################
//# create wday layer  ex: 土曜日
//########################################
static void create_wday_layers(int line_y){
	wday_layer    = bitmap_layer_create(GRect(40+x3, line_y,70,22)); // 60+y
	bitmap_layer_set_bitmap(wday_layer,    wday_image);
	bitmap_layer_set_alignment(wday_layer,    GAlignLeft);
	bitmap_layer_set_compositing_mode(wday_layer,    GCompOpSet);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(wday_layer));
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "create_wday_layers END.");
	}
}


//########################################
//# destroy time layer  ex: 20:45
//########################################
static void destroy_time_layers(){
	layer_remove_from_parent(bitmap_layer_get_layer(colon_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(HH10_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(HH01_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(MM10_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(MM01_layer));
	bitmap_layer_destroy(colon_layer);
	bitmap_layer_destroy(HH10_layer);
	bitmap_layer_destroy(HH01_layer);
	bitmap_layer_destroy(MM10_layer);
	bitmap_layer_destroy(MM01_layer);
	gbitmap_destroy(HH10_image);
	gbitmap_destroy(HH01_image);
	gbitmap_destroy(MM10_image);
	gbitmap_destroy(MM01_image);
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_time_layers 1.");
//  ここに問題あり。
//	gbitmap_destroy(colon_image);
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_time_layers END.");
	}
}
//########################################
//# create time layer
//########################################
static void create_time_layers(int line_y){
	HH10_layer    = bitmap_layer_create(GRect(42+x4,line_y,14,22));
	HH01_layer    = bitmap_layer_create(GRect(58+x4,line_y,14,22));
	colon_layer   = bitmap_layer_create(GRect(72+x4,line_y,8,22));
	MM10_layer    = bitmap_layer_create(GRect(82+x4,line_y,14,22));
	MM01_layer    = bitmap_layer_create(GRect(98+x4,line_y,14,22));
	bitmap_layer_set_bitmap(HH10_layer, HH10_image);
	bitmap_layer_set_bitmap(HH01_layer, HH01_image);
	bitmap_layer_set_bitmap(colon_layer,colon_image);
	bitmap_layer_set_bitmap(MM10_layer, MM10_image);
	bitmap_layer_set_bitmap(MM01_layer, MM01_image);
	bitmap_layer_set_alignment(HH10_layer,  GAlignLeft);
	bitmap_layer_set_alignment(HH01_layer,  GAlignLeft);
	bitmap_layer_set_alignment(colon_layer, GAlignLeft);
	bitmap_layer_set_alignment(MM10_layer,  GAlignLeft);
	bitmap_layer_set_alignment(MM01_layer,  GAlignLeft);
	bitmap_layer_set_compositing_mode(HH10_layer,  GCompOpSet);
	bitmap_layer_set_compositing_mode(HH01_layer,  GCompOpSet);
	bitmap_layer_set_compositing_mode(colon_layer, GCompOpSet);
	bitmap_layer_set_compositing_mode(MM10_layer,  GCompOpSet);
	bitmap_layer_set_compositing_mode(MM01_layer,  GCompOpSet);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(HH10_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(HH01_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(colon_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(MM10_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(MM01_layer));
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "create_time_layers END.");
	}
}

//########################################
//# handle_battery バッテリーステイタスの表示
//########################################
static void destroy_battery_layer(){
	layer_remove_from_parent(bitmap_layer_get_layer(battery_layer));
	bitmap_layer_destroy(battery_layer);
	gbitmap_destroy(battery_image);    
}

static void create_battery_layer(){
//	battery_layer  = bitmap_layer_create(GRect(120,3,16,8)); // 右上
	battery_layer  = bitmap_layer_create(GRect(10,3,16,8));  //左上
	#ifdef PBL_ROUND    
	battery_layer  = bitmap_layer_create(GRect(82,3,16,8));
	#endif  
	layer_set_hidden(bitmap_layer_get_layer(battery_layer),false);
	bitmap_layer_set_bitmap(battery_layer, battery_image);
	bitmap_layer_set_alignment(battery_layer,GAlignLeft);
	bitmap_layer_set_compositing_mode(battery_layer, GCompOpSet);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(battery_layer));
}

static void handle_battery(BatteryChargeState charge_state) {
	// last_battery_status (min:0 - max:100) との違いをチェック
	// 変化があったら書き換える(毎回書き換えしないように)
	// sはバッテリー残量。下一桁は切る。
	// 大まかな残量しか興味がないので厳密にチェックしない。
	int s = 0;
	int batt50 = 50 ;  // 50%
	int batt30 = 30 ;  // 30% 
	if (mydebug) {
		batt50 = 90 ;
		batt30 = 70 ;
	}
	s = charge_state.charge_percent / 10 * 10 ; // 下一桁は切る。
	if ( s == last_battery_status ) {
 		// 画面書き換え無し
		;
	} else {
		// 画面書き換え有り
		destroy_battery_layer();
		battery_image   = gbitmap_create_with_resource(RESOURCE_ID_NULL);    
		if ( s < batt50 ){ 
			battery_image   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_50);
		}
		if ( s < batt30 ){ 
			battery_image   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_30);
		}
		create_battery_layer();
	} 
	layer_set_hidden(text_layer_get_layer(ver_layer),true);
	if (charge_state.is_charging){	// charging...
		destroy_battery_layer();
		battery_image   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGING);
		create_battery_layer();
		layer_set_hidden(text_layer_get_layer(ver_layer),false);
	}
	if (charge_state.is_plugged){	// charging...
		destroy_battery_layer();
		battery_image   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGING);
		create_battery_layer();
		layer_set_hidden(text_layer_get_layer(ver_layer),false);
	}
	last_battery_status = s; // deinit... 
}

//########################################
//# displey_holiday 祝日の表示
//########################################
static void display_holiday(struct tm *tick_time) {
	uint32_t today8 ;
	int      dbgtoday;
	// today を 8 文字で表示  ex: 20141224 <- 2014/12/24
	today8 = ((tick_time->tm_year) + 1900) *10000 
			+ ((tick_time->tm_mon) + 1 )*100 
			+ (tick_time->tm_mday); 

	// まずはレイヤーを隠す
	layer_set_hidden(bitmap_layer_get_layer(holidayN_layer),true); 
	layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),true); 

	if (mydebug) { /* debug */
		today8 = 20250505;
		dbgtoday = today8;
	}

	uint32_t holidaylist_Num  = sizeof holidaylist /sizeof holidaylist[0];  // 配列の要素数
	uint32_t holidayImage_Num = sizeof HOLIDAY_IMG_IDS /sizeof HOLIDAY_IMG_IDS[0];  // 配列の要素数

	for( uint32_t i = 0; i < holidaylist_Num; i++ ) { // リストの数だけまわす
		if ( today8 == ( holidaylist[i] / 100 ) )  {
			if (( holidaylist[i] % 100 ) < holidayImage_Num)  // 正常な範囲内のとき
			{
				holidayN_image  = gbitmap_create_with_resource(HOLIDAY_IMG_IDS[( holidaylist[i] % 100 )]);
				bitmap_layer_set_bitmap(holidayN_layer, holidayN_image);
				bitmap_layer_set_compositing_mode(holidayN_layer, GCompOpSet);
				layer_set_hidden(bitmap_layer_get_layer(holidayN_layer),false); // リストにあるので祝日名表示
				if ( ( ( holidaylist[i] % 100 ) == 0 ) || ( ( holidaylist[i] % 100 ) == 16 ) ) {
					layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),true); // 振替休日など祝日じゃない場合。
				}else{
					layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),false);
				}; 
			}else{  // データを作り間違った時。。。
				// 該当する祝日が無いのでとりあえず何も表示しないでおく。
				layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),true);        
				layer_set_hidden(bitmap_layer_get_layer(holidayN_layer),true);
				APP_LOG(APP_LOG_LEVEL_DEBUG, "display_holiday ERROR no data : %d", dbgtoday );
			}
			if(mydebug){
				layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),false);
				APP_LOG(APP_LOG_LEVEL_DEBUG, "display_holiday   date : %d", dbgtoday );
			}
			break; // リストにあったら終了。
		}
	}
}


//########################################
//# rewrite_display_date_layer 
//########################################
static void rewrite_display_date_layer(GRect bounds,struct tm *tick_time){
	// --------------------------------------------------------
  	// year_layer 年の表示の行
  	// date_layer 月日の表示の行
  	// wday_layer 曜日の表示の行
	// --------------------------------------------------------
	// Sat Jan 18 , 2014  (footer : debug)
	if(mydebug){
		strftime(date_text, sizeof(date_text), "%a %b %d , %Y ", tick_time);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rewrite_display_date_layer Start. : %s",date_text);
	}
	
	destroy_wday_layers(); // ok
	destroy_date_layers(); //
	destroy_year_layers();
	if(mydebug){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rewrite_display_date_layer : destroy layers END.");
	}		
	// 数字以外のもの描画
 	// Date [ 平成 年 月 日 ]
	
	year10_image   = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_year) - 88) / 10]); 
	year01_image   = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_year) - 88) % 10]); 
	month10_image  = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_mon)  + 1 ) / 10]); 
	month01_image  = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_mon)  + 1 ) % 10]); 
	day10_image    = gbitmap_create_with_resource(NUM_IMG_IDS[ (tick_time->tm_mday)       / 10]); 
	day01_image    = gbitmap_create_with_resource(NUM_IMG_IDS[ (tick_time->tm_mday)       % 10]); 
	wday_image     = gbitmap_create_with_resource(WDAY_IMG_IDS[tick_time->tm_wday]); 

	if ((tick_time->tm_mon + 1 ) < 10){
		month10_image  = gbitmap_create_with_resource(RESOURCE_ID_NULL); 
	}
	if  (tick_time->tm_mday      < 10){
		day10_image    = gbitmap_create_with_resource(RESOURCE_ID_NULL); 
	}

	#ifdef PBL_ROUND
	if (bounds.size.h == 180){
		create_year_layers(19); // full PBL_ROUND
		create_date_layers(49); // full PBL_ROUND
		create_wday_layers(79); // full PBL_ROUND
	}else{
		create_year_layers(bounds.size.h-24-24-24-24-24); // shrink 		
		create_date_layers(bounds.size.h-24-24-24-24); // shrink 		
		create_wday_layers(bounds.size.h-24-24-24); // shrink 		
	}
	#else
	if (bounds.size.h == 168){
		create_year_layers(16); // full 
		create_date_layers(46); // full 
		create_wday_layers(76); // full 
	}else{
		create_year_layers(bounds.size.h-24-24-24-24-24); // shrink 		
		create_date_layers(bounds.size.h-24-24-24-24); // shrink 		
		create_wday_layers(bounds.size.h-24-24-24); // shrink 		
	}
  	#endif
	if(mydebug){		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rewrite_display_date_layer END.");
	}
}

static void rewrite_display_time_layer(GRect bounds,struct tm *tick_time){
	// --------------------------------------------------------
  	// time_layer メインの時刻表示 12:34
	// --------------------------------------------------------
	if(mydebug){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rewrite_display_time_layer Start.");
	}

	destroy_time_layers();

	// APP_LOG(APP_LOG_LEVEL_DEBUG, "rewrite_display_time_layer 1");

	// set images
	if (clock_is_24h_style()) {
    // 24 
		HH10_image     = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_hour) / 10]); 
		HH01_image     = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_hour) % 10]); 
		strftime(dbg_text, sizeof(dbg_text), "%H:%M:%S", tick_time); // for Debug
	}else{
    // 12
    	int t;
		t = (tick_time->tm_hour) % 12;
		if ( t == 0 ){
			t = 12;
		}
		if ( t > 9 ){
			HH10_image     = gbitmap_create_with_resource(NUM_IMG_IDS[1]);
		} else {
			HH10_image     = gbitmap_create_with_resource(RESOURCE_ID_NULL);
		} 
		HH01_image     = gbitmap_create_with_resource(NUM_IMG_IDS[t % 10]); 
		strftime(dbg_text, sizeof(dbg_text), "%I:%M:%S", tick_time); // for Debug
	}
	//
	MM10_image     = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_min)  / 10]); 
	MM01_image     = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_min)  % 10]); 
	if(mydebug){
		snprintf(dbg_buffer, 40, "%s %d%%", dbg_text,last_battery_status); // debug
		text_layer_set_text(dbg_layer,dbg_buffer); // debug
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rewrite_display_time_layer : %s",dbg_buffer);
	}

	#ifdef PBL_ROUND
	if (bounds.size.h == 180){
		create_time_layers(111); // full 
	}else{
		create_time_layers(bounds.size.h-24-24); // shrink 		
	}
	#else
	if (bounds.size.h == 168){
		create_time_layers(108); // full 
	}else{
		create_time_layers(bounds.size.h-24-24); // shrink 		
	}
  	#endif
	if(mydebug){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rewrite_display_time_layer END.");
	}

}

//########################################
//# destroy date_layer Footer 一番下に表示する日付バー
//########################################
static void destroy_footer_date_layer(){
	text_layer_destroy(date_layer);
	if(mydebug){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_footer_date_layer END.");
	}
}

static void rewrite_footer_date_layer(GRect bounds,struct tm *tick_time){
	// --------------------------------------------------------
  	// date_layer Footer 一番下に表示する日付バー
  	// --------------------------------------------------------  
	// destroy_footer_date_layer();
	#ifdef PBL_ROUND
	if (bounds.size.h == 180){
	  	date_layer = text_layer_create(GRect(0,bounds.size.h - 42,bounds.size.w,42));
	}else{
	  	date_layer = text_layer_create(GRect(0,bounds.size.h - 24,bounds.size.w,24));
	}
	#else
	date_layer = text_layer_create(GRect(0,bounds.size.h - 24,bounds.size.w,24));
  	#endif

	text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
	text_layer_set_text_alignment(date_layer,GTextAlignmentCenter);
	text_layer_set_text_color(date_layer, GColorWhite);
	text_layer_set_background_color(date_layer, GColorBlack);
	#ifdef PBL_COLOR
	text_layer_set_text_color(date_layer, GColorCeleste );
	text_layer_set_background_color(date_layer, GColorOxfordBlue );
	#endif
	// Sat Jan 18 , 2014  (footer)
	strftime(date_text, sizeof(date_text), "%a %b %d , %Y ", tick_time);
	text_layer_set_text(date_layer,date_text);
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rewrite_footer_date_layer END.");
	}
}
//################################################
//# handle_minute_tick ( Called once per minute ) 
//# 毎分00秒に呼び出される処理。ここで画面の書き換えをする 
//################################################
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {	
	if (mydebug){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "---- handle_minute_tick Start.----");
	}
	GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));
	// time
	rewrite_display_time_layer(bounds,tick_time);
	
	if (mydebug) { // debug用に毎分祝日処理
		APP_LOG(APP_LOG_LEVEL_DEBUG, "-- debug -- handle_minute_tick / rewrite_display_date_layer.");
		rewrite_display_date_layer(bounds,tick_time);
		strftime(date_text, sizeof(date_text), "%a %b %d , %Y ", tick_time);
		text_layer_set_text(date_layer,date_text);
		display_holiday(tick_time);
	}


	// calendar update ( per day ) 0時0分処理
	if( (tick_time->tm_hour == 0 ) && (tick_time->tm_min == 0 ) ) {
		rewrite_display_date_layer(bounds,tick_time);
		// rewrite_footer_date_layer(bounds,tick_time); //レイヤーの再作成は不要
		strftime(date_text, sizeof(date_text), "%a %b %d , %Y ", tick_time);
		text_layer_set_text(date_layer,date_text);
		display_holiday(tick_time);
	}

	// battery status update
	handle_battery(battery_state_service_peek());
	if (mydebug){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_minute_tick END.");
	}
}


//########################################
//# prv_unobstructed_will_change
//# prv_unobstructed_did_change
//# Quick Viewによる画面サイズの変更
//########################################
static void prv_unobstructed_will_change(GRect final_unobstructed_screen_area,void *context) {
	//　変更開始 特に何もしない
	// Get the full size of the screen
	// GRect full_bounds = layer_get_bounds(s_window_layer);
	if (mydebug) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "prv_unobstructed_will_change");
	}
}

static void prv_unobstructed_did_change(void *context) {
	// 画面サイズ変更終了
	// Get the full size of the screen
	GRect full_bounds = layer_get_bounds(window_get_root_layer(window));
	// Get the total available screen real-estate
	GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));

  	// --------------------------------------------------------
  	// 時刻 now 
  	// --------------------------------------------------------
  	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);

	if ( bounds.size.h <  full_bounds.size.h ){
		// shrink size  Quick View ダイアログが出て画面が小さくなっている時
		if (mydebug) {
			APP_LOG(APP_LOG_LEVEL_DEBUG, "prv_unobstructed_did_change SMALL");
		}
		GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));
		destroy_footer_date_layer();
		rewrite_footer_date_layer(bounds,tick_time);
		rewrite_display_time_layer(bounds,tick_time);
		rewrite_display_date_layer(bounds,tick_time);


	} else {
		// 画面がフルサイズ
		if (mydebug) {
			APP_LOG(APP_LOG_LEVEL_DEBUG, "prv_unobstructed_did_change FULL");
		}
		GRect bounds = layer_get_bounds(window_get_root_layer(window));
		destroy_footer_date_layer();
		rewrite_footer_date_layer(bounds,tick_time);
		rewrite_display_time_layer(bounds,tick_time);
		rewrite_display_date_layer(bounds,tick_time);
	}
}

//########################################
//# do_init  初期化
//# GColorCeleste = 薄い青
//# GColorDukeBlue  = 濃い青 GColorOxfordBlue 
//# GColorPastelYellow = 薄い黄色
//# GColorArmyGreen = 濃いグリーン
//########################################
static void do_init() {
	// --------------------------------------------------------
	// Main Window
	// メインのウィンドウ
	// --------------------------------------------------------
	if (mydebug){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "do_init START.----------------");
	}
	window = window_create();
	window_stack_push(window, true);
	window_set_background_color(window, GColorWhite);   
	#ifdef PBL_COLOR
	window_set_background_color(window, GColorCeleste  );
	#endif

	// unobstructed_bounds Timeline Quick View 対応
	// サンプルのまま。。。
	// s_window_layer = window_get_root_layer(window)
	GRect full_bounds = layer_get_bounds(window_get_root_layer(window));
	UnobstructedAreaHandlers handlers = {
		.will_change = prv_unobstructed_will_change,
    	.did_change  = prv_unobstructed_did_change
  	};
  	unobstructed_area_service_subscribe(handlers, NULL);

	// --------------------------------------------------------
  	// 時刻 now 
  	// --------------------------------------------------------
  	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);

	// --------------------------------------------------------
	// バージョン表示初期処理
	// 初期処理で書いたレイヤーを非表示にしておく。
	// 充電中にのみ表示する。デバッグ時に確認するための機能。
	// --------------------------------------------------------
	#ifdef PBL_ROUND  
	ver_layer = text_layer_create(GRect(20,122,100,16));
	#else
	ver_layer = text_layer_create(GRect(5,128,100,16));
	#endif
	text_layer_set_font(ver_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(ver_layer));
	text_layer_set_text_color(ver_layer, GColorBlack);
	text_layer_set_background_color(ver_layer, GColorClear);
	layer_set_hidden(text_layer_get_layer(ver_layer),true);
	text_layer_set_text(ver_layer,appversion);

	// --------------------------------------------------------
	// デバッグ用の文字をクリアで書く。
	// デバッグの時は黒にすると 
	// --------------------------------------------------------
	#ifdef PBL_ROUND  
	dbg_layer = text_layer_create(GRect(12,125,140,14));
	#else
	dbg_layer = text_layer_create(GRect(2,125,140,14));
	#endif
	text_layer_set_font(dbg_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(dbg_layer));
	text_layer_set_text_color(dbg_layer, GColorWhite);
	text_layer_set_background_color(dbg_layer, GColorWhite);
	#ifdef PBL_COLOR
	text_layer_set_text_color(dbg_layer, GColorCeleste);
	text_layer_set_background_color(dbg_layer, GColorCeleste);
	#endif
	layer_set_hidden(text_layer_get_layer(dbg_layer),true);
  	if (mydebug) { // GColorBlack
    	text_layer_set_text_color(dbg_layer, GColorBlack);
    	layer_set_hidden(text_layer_get_layer(dbg_layer),false);
  	}
	strftime(dbg_text, sizeof(dbg_text), "%H:%M:%S", tick_time); // for Debug
	text_layer_set_text(dbg_layer,dbg_text); // debug
	APP_LOG(APP_LOG_LEVEL_DEBUG, "do_init     : [ %s ]",dbg_text);

	//
	//
	// Holiday Flag , Holiday Name
	#ifdef PBL_ROUND
	holidayF_layer = bitmap_layer_create(GRect(14,68+19,52,31));
	holidayN_layer = bitmap_layer_create(GRect(14,102+19,52,8));
	#else
	holidayF_layer = bitmap_layer_create(GRect(4,68+16,52,31));
	holidayN_layer = bitmap_layer_create(GRect(4,102+16,52,8));
	#endif
	layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),true); //
	layer_set_hidden(bitmap_layer_get_layer(holidayN_layer),true); // true 
	bitmap_layer_set_background_color(holidayF_layer, GColorClear);
	bitmap_layer_set_background_color(holidayN_layer, GColorClear);
	bitmap_layer_set_alignment(holidayF_layer,GAlignTop);
	bitmap_layer_set_alignment(holidayN_layer,GAlignTop);
  	bitmap_layer_set_compositing_mode(holidayN_layer, GCompOpSet);
  	bitmap_layer_set_compositing_mode(holidayF_layer, GCompOpSet);

	// bitmap のセット
	holidayN_image = gbitmap_create_with_resource(HOLIDAY_IMG_IDS[0]);
	holidayF_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY);

 	bitmap_layer_set_bitmap(holidayN_layer, holidayN_image);
 	bitmap_layer_set_bitmap(holidayF_layer, holidayF_image);
  
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(holidayN_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(holidayF_layer));


	// --------------------------------------------------------
	// 表示の初期化 init 
	// --------------------------------------------------------
	battery_image  = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGING);
	heisei_image   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEISEI);
	year_image     = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_YEAR); 
	year10_image   = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_year) - 88) / 10]); 
	year01_image   = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_year) - 88) % 10]); 
	month_image    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MONTH); 
	month10_image  = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_mon)  + 1 ) / 10]); 
	month01_image  = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_mon)  + 1 ) % 10]); 
	day_image      = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DAY); 
	day10_image    = gbitmap_create_with_resource(NUM_IMG_IDS[ (tick_time->tm_mday)       / 10]); 
	day01_image    = gbitmap_create_with_resource(NUM_IMG_IDS[ (tick_time->tm_mday)       % 10]); 
	if ((tick_time->tm_mon + 1 ) < 10){
		month10_image  = gbitmap_create_with_resource(RESOURCE_ID_NULL); 
	}
	if  (tick_time->tm_mday      < 10){
		day10_image    = gbitmap_create_with_resource(RESOURCE_ID_NULL); 
	}
	wday_image     = gbitmap_create_with_resource(WDAY_IMG_IDS[tick_time->tm_wday]); 	
	if (clock_is_24h_style()) {
    // 24 
		HH10_image = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_hour) / 10]); 
		HH01_image = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_hour) % 10]); 
	}else{
    // 12
    	int t;
		t = (tick_time->tm_hour) % 12;
		if ( t == 0 ){
			t = 12;
		}
		if ( t > 9 ){
			HH10_image     = gbitmap_create_with_resource(NUM_IMG_IDS[1]);
		} else {
			HH10_image     = gbitmap_create_with_resource(RESOURCE_ID_NULL);
		} 
		HH01_image     = gbitmap_create_with_resource(NUM_IMG_IDS[t % 10]); 
	}
	MM10_image     = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_min)  / 10]); 
	MM01_image     = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_min)  % 10]); 
	colon_image    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_COLON); 


	// 数字の部分の描画 初期化 (必要！！)
  	#ifdef PBL_ROUND  
	int linex = 19;
 	#else
	int linex = 16;
  	#endif
	create_year_layers(linex);
	create_date_layers(linex + 30);
	create_wday_layers(linex + 30 + 30);
	create_time_layers(linex + 30 + 30 + 30 + 2);
	create_battery_layer();

	// Get the total available screen real-estate Quick View 
	GRect now_bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));

  	// --------------------------------------------------------
  	// date_layer Footer 一番下に表示する日付バー(初期値)
  	// --------------------------------------------------------  
	//rewrite_footer_date_layer(full_bounds,tick_time);
	rewrite_footer_date_layer(now_bounds,tick_time);

  	// --------------------------------------------------------
	// 時刻日付の表示
  	// --------------------------------------------------------
	rewrite_display_time_layer(now_bounds,tick_time);
	rewrite_display_date_layer(now_bounds,tick_time);
	display_holiday(tick_time);// full_bounds only

	// 画面書き換え処理 秒表示が無いなら上のでよい
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);  // 毎分0秒の処理 
	//  tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);  // 毎秒の処理。

	// Subscribe to the Battery State Service
	// バッテリーチェック
	battery_state_service_subscribe(handle_battery);
	
	if (mydebug){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "do_init END.");
	}
}

//########################################
//# do_deinit 終了処理
//########################################
static void do_deinit() {
	destroy_time_layers();
	destroy_wday_layers();
	destroy_date_layers();
	destroy_battery_layer();

	layer_remove_from_parent(bitmap_layer_get_layer(holidayF_layer));
	layer_remove_from_parent(bitmap_layer_get_layer(holidayN_layer));
	gbitmap_destroy(holidayN_image);
	gbitmap_destroy(holidayF_image);
	text_layer_destroy(date_layer);
	text_layer_destroy(dbg_layer);
  	bitmap_layer_destroy(holidayN_layer);
	bitmap_layer_destroy(holidayF_layer);

	window_destroy(window);
		
	if (mydebug){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "do_deinit END.");
	}


}
//########################################
//# main 
//########################################
int main(void) {
	do_init();
	app_event_loop();
	do_deinit();
}
//########################################
//# END
//########################################