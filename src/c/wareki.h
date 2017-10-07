/*
 WAREKI
 Displey Japanese Year calendar 
 
 ahiru studio
 
 2014/01/25 Ver.1.0.0
 */
#include <pebble.h>
//
//
//struct tm *gtick_time;

static Window *window;

static TextLayer *date_layer;
static TextLayer *dbg_layer;
static char date_text[] = "Sat Nov 29 , 2015 ";
static char dbg_text[] = "00:00:00";
static char dbg_buffer[] = "00:00:00 100% BT ";

static TextLayer *ver_layer;

static GBitmap *day_image;
static GBitmap *month_image;
static GBitmap *year_image;
static GBitmap *battery_image;
static GBitmap *heisei_image;
static GBitmap *year10_image;
static GBitmap *year01_image;
static GBitmap *month10_image;
static GBitmap *month01_image;
static GBitmap *day10_image;
static GBitmap *day01_image;
static GBitmap *wday_image;
static GBitmap *HH10_image;
static GBitmap *HH01_image;
static GBitmap *colon_image;
static GBitmap *MM10_image;
static GBitmap *MM01_image;
static GBitmap *holidayN_image;
static GBitmap *holidayF_image;

static BitmapLayer *day_layer;
static BitmapLayer *month_layer;
static BitmapLayer *year_layer;
static BitmapLayer *battery_layer;
static BitmapLayer *heisei_layer;
static BitmapLayer *year10_layer;
static BitmapLayer *year01_layer;
static BitmapLayer *month10_layer;
static BitmapLayer *month01_layer;
static BitmapLayer *day10_layer;
static BitmapLayer *day01_layer;
static BitmapLayer *wday_layer;
static BitmapLayer *HH10_layer;
static BitmapLayer *HH01_layer;
static BitmapLayer *colon_layer;
static BitmapLayer *MM10_layer;
static BitmapLayer *MM01_layer;
static BitmapLayer *holidayN_layer;
static BitmapLayer *holidayF_layer;

const int NUM_IMG_IDS[] = {
  RESOURCE_ID_0,
  RESOURCE_ID_1,
  RESOURCE_ID_2,
  RESOURCE_ID_3,
  RESOURCE_ID_4,
  RESOURCE_ID_5,
  RESOURCE_ID_6,
  RESOURCE_ID_7,
  RESOURCE_ID_8,
  RESOURCE_ID_9
};
const int WDAY_IMG_IDS[] = {
  RESOURCE_ID_SUN,
  RESOURCE_ID_MON,
  RESOURCE_ID_TUE,
  RESOURCE_ID_WED,
  RESOURCE_ID_THU,
  RESOURCE_ID_FRI,
  RESOURCE_ID_SAT
};
const int HOLIDAY_IMG_IDS[] = {
  RESOURCE_ID_00HURIKAE,
  RESOURCE_ID_01GANJITSU,
  RESOURCE_ID_02SEIJIN,
  RESOURCE_ID_03KENKOKU,
  RESOURCE_ID_04SHUNBUN,
  RESOURCE_ID_05SHOWA,
  RESOURCE_ID_06KENPOU,
  RESOURCE_ID_07MIDORI,
  RESOURCE_ID_08KODOMO,
  RESOURCE_ID_09UMI,
  RESOURCE_ID_10KEIROU,
  RESOURCE_ID_11SHUBUN,
  RESOURCE_ID_12TAIIKU,
  RESOURCE_ID_13BUNKA,
  RESOURCE_ID_14KINROU,
  RESOURCE_ID_15TENNOU,
  RESOURCE_ID_16KOKUMIN,
  RESOURCE_ID_17YAMA
};
