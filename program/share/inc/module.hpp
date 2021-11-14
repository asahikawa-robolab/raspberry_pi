/* Last updated : 2020/10/05, 19:36 */
#ifndef MODULE_HPP
#define MODULE_HPP
#include "_thread.hpp"
#include "_serial_communication.hpp"
#include <opencv2/opencv.hpp>
#include "urg_sensor.h"
#include "urg_utils.h"

/*-----------------------------------------------
 *
 * SwitchData
 *
-----------------------------------------------*/
class SwitchData
{
private:
	static const std::size_t m_TOGGLE_NUM = 7;
	jibiki::ShareVar<uint8_t> m_push_l;
	jibiki::ShareVar<uint8_t> m_push_r;
	jibiki::ShareVar<uint8_t> m_toggle[m_TOGGLE_NUM];
	jibiki::ShareVar<uint8_t> m_lu;
	jibiki::ShareVar<uint8_t> m_ld;
	jibiki::ShareVar<uint8_t> m_ru;
	jibiki::ShareVar<uint8_t> m_rd;
	jibiki::ShareVar<uint8_t> m_slide; /* スライドポテンショメータ */

public:
	void set(jibiki::ParamCom& com); /* 受信データをメンバにセットする */
	uint8_t push_l(void) { return m_push_l.read(); }
	uint8_t push_r(void) { return m_push_r.read(); }
	uint8_t toggle(std::size_t index);
	uint8_t lu(void) { return m_lu.read(); }
	uint8_t ld(void) { return m_ld.read(); }
	uint8_t ru(void) { return m_ru.read(); }
	uint8_t rd(void) { return m_rd.read(); }
	uint8_t slide(void) { return m_slide.read(); }
};

inline uint8_t SwitchData::toggle(std::size_t index)
{
	/* 範囲チェック */
	if (m_TOGGLE_NUM <= index)
	{
		jibiki::print_err(__PRETTY_FUNCTION__, "out_of_range");
		throw std::out_of_range(""); /* エラー発生 */
	}
	return m_toggle[index].read();
}

/*-----------------------------------------------
 *
 * Imu
 *
-----------------------------------------------*/
class Imu
{
private:
	jibiki::ShareVar<double> m_raw_data;
	jibiki::ShareVar<double> m_offset;

public:
	Imu(void) noexcept : m_raw_data(0), m_offset(0) {}
	void write_offset(double angle);      /* read の結果が angle になるようにオフセットを設定する */
	void write_raw_data(double raw_data); /* 生データを書き込む */
	double read(void);                    /* 角度データを読み出す */
};
inline void Imu::write_offset(double angle) { m_offset = angle - m_raw_data.read(); }
inline void Imu::write_raw_data(double raw_data) { m_raw_data = raw_data; }
inline double Imu::read(void) { return m_raw_data.read() + m_offset.read(); }

/*-----------------------------------------------
 *
 * Controller
 *
-----------------------------------------------*/
class Controller
{
public:
	typedef enum /* 左スティックか右スティックか */
	{
		MODE_L, /* 左スティック */
		MODE_R, /* 右スティック */
	} Mode;
	typedef enum /* 方向の分割数 */
	{
		DIR_INF, /* 全方向 */
		DIR_8,   /* ８方向 */
		DIR_4,   /* ４方向 */
	} DirNum;

private: /* コンストラクタでしか変更操作が行われないため排他制御不要 */
	size_t m_calc_period_ms;

private:
	jibiki::ShareVar<double> m_speed;            /* スティックの倒し具合 */
	jibiki::ShareVar<double> m_theta[2];         /* スティックを倒している向き */
	jibiki::ShareVar<jibiki::time_point> m_time; /* 実行周期の管理 */

	jibiki::ShareVar<bool> m_l_cross_l;
	jibiki::ShareVar<bool> m_l_cross_r;
	jibiki::ShareVar<bool> m_l_cross_u;
	jibiki::ShareVar<bool> m_l_cross_d;
	jibiki::ShareVar<bool> m_r_cross_l;
	jibiki::ShareVar<bool> m_r_cross_r;
	jibiki::ShareVar<bool> m_r_cross_u;
	jibiki::ShareVar<bool> m_r_cross_d;
	jibiki::ShareVar<bool> m_l_switch_d;
	jibiki::ShareVar<bool> m_l_switch_m;
	jibiki::ShareVar<bool> m_r_switch_d;
	jibiki::ShareVar<bool> m_r_switch_m;
	jibiki::ShareVar<bool> m_l_lever_l;
	jibiki::ShareVar<bool> m_l_lever_r;
	jibiki::ShareVar<bool> m_r_lever_l;
	jibiki::ShareVar<bool> m_r_lever_r;
	jibiki::ShareVar<uint8_t> m_l_analog_stick_h;
	jibiki::ShareVar<uint8_t> m_l_analog_stick_v;
	jibiki::ShareVar<uint8_t> m_r_analog_stick_h;
	jibiki::ShareVar<uint8_t> m_r_analog_stick_v;
	jibiki::ShareVar<bool> m_l_slide;
	jibiki::ShareVar<bool> m_r_slide;
	jibiki::ShareVar<bool> m_l_switch_u;
	jibiki::ShareVar<bool> m_r_switch_u;
	jibiki::ShareVar<bool> m_tact_lu;
	jibiki::ShareVar<bool> m_tact_mu;
	jibiki::ShareVar<bool> m_tact_ru;
	jibiki::ShareVar<bool> m_tact_ld;
	jibiki::ShareVar<bool> m_tact_md;
	jibiki::ShareVar<bool> m_tact_rd;

private:
	double my_atan(double y, double x, DirNum dir_num) const;
	void convt(Mode mode, DirNum dir_num); /* アナログスティックのデータを大きさ，向きに変換 */

public:
	Controller(std::string json_path = "setting.json");
	void set(jibiki::ParamCom& com);
	bool l_cross_l(void) { return m_l_cross_l.read(); }
	bool l_cross_r(void) { return m_l_cross_r.read(); }
	bool l_cross_u(void) { return m_l_cross_u.read(); }
	bool l_cross_d(void) { return m_l_cross_d.read(); }
	bool r_cross_l(void) { return m_r_cross_l.read(); }
	bool r_cross_r(void) { return m_r_cross_r.read(); }
	bool r_cross_u(void) { return m_r_cross_u.read(); }
	bool r_cross_d(void) { return m_r_cross_d.read(); }
	bool l_switch_d(void) { return m_l_switch_d.read(); }
	bool l_switch_m(void) { return m_l_switch_m.read(); }
	bool r_switch_d(void) { return m_r_switch_d.read(); }
	bool r_switch_m(void) { return m_r_switch_m.read(); }
	bool l_lever_l(void) { return m_l_lever_l.read(); }
	bool l_lever_r(void) { return m_l_lever_r.read(); }
	bool r_lever_l(void) { return m_r_lever_l.read(); }
	bool r_lever_r(void) { return m_r_lever_r.read(); }
	uint8_t l_analog_stick_h(void) { return m_l_analog_stick_h.read(); }
	uint8_t l_analog_stick_v(void) { return m_l_analog_stick_v.read(); }
	uint8_t r_analog_stick_h(void) { return m_r_analog_stick_h.read(); }
	uint8_t r_analog_stick_v(void) { return m_r_analog_stick_v.read(); }
	bool l_slide(void) { return m_l_slide.read(); }
	bool r_slide(void) { return m_r_slide.read(); }
	bool l_switch_u(void) { return m_l_switch_u.read(); }
	bool r_switch_u(void) { return m_r_switch_u.read(); }
	bool tact_lu(void) { return m_tact_lu.read(); }
	bool tact_mu(void) { return m_tact_mu.read(); }
	bool tact_ru(void) { return m_tact_ru.read(); }
	bool tact_ld(void) { return m_tact_ld.read(); }
	bool tact_md(void) { return m_tact_md.read(); }
	bool tact_rd(void) { return m_tact_rd.read(); }
	/* アナログスティック */
	double speed(Mode mode, DirNum dir_num);
	double theta(Mode mode, DirNum dir_num);
};
inline double Controller::speed(Mode mode, DirNum dir_num)
{
	convt(mode, dir_num);
	return m_speed.read();
}
inline double Controller::theta(Mode mode, DirNum dir_num)
{
	convt(mode, dir_num);
	return m_theta[mode].read();
}

/*-----------------------------------------------
 *
 * Chassis（動作未確認）
 *
-----------------------------------------------*/
class Chassis
{
public:
	typedef enum
	{
		TURN_CW,       /* 時計回り */
		TURN_CCW,      /* 反時計回り */
		TURN_SHORTEST, /* 最短方向 */
	} TurnMode;

private: /* コンストラクタでしか変更操作が行われないため排他制御不要 */
	size_t m_channel_fr, m_channel_fl, m_channel_br, m_channel_bl;
	bool m_inverse_fr, m_inverse_fl, m_inverse_br, m_inverse_bl;
	double m_max_rpm;
	double m_rotate_max, m_rotate_min, m_rotate_kp;
	Imu* m_imu;
	std::string m_json_path;
	size_t m_calc_period_ms;

private:
	jibiki::ShareVar<double> m_fr, m_fl, m_br, m_bl; /* 回転数目標値 */
	jibiki::ShareVar<double> m_raw_rpm[4];           /* 回転数目標値（入れ替え，反転なし） */
	jibiki::ShareVar<jibiki::time_point> m_time;     /* calc 用 */

private:
	void load_json(void);
	void calc(void);
	double calc_rotate(void);
	double calc_angle_diff(double subed, double sub, TurnMode turn_mode);

public:
	jibiki::ShareVar<double> m_speed;
	jibiki::ShareVar<double> m_theta;
	jibiki::ShareVar<double> m_spin;
	jibiki::ShareVar<TurnMode> m_turn_mode;

public:
	Chassis(Imu& imu, std::string json_path = "setting.json");
	void stop(void);
	double fr(void);
	double fl(void);
	double br(void);
	double bl(void);
	double raw_fr(void);
	double raw_fl(void);
	double raw_br(void);
	double raw_bl(void);
	double max_rpm(void) const noexcept; /* JSON ファイルから読み込んだ max_rpm を返す */
};
/* 足回りを停止させる */
inline void Chassis::stop(void)
{
	m_speed = 0;
	m_spin = m_imu->read();
}
/* 回転数目標値を返す（FR） */
inline double Chassis::fr(void)
{
	calc();
	return m_fr.read();
}
/* 回転数目標値を返す（FL） */
inline double Chassis::fl(void)
{
	calc();
	return m_fl.read();
}
/* 回転数目標値を返す（BR） */
inline double Chassis::br(void)
{
	calc();
	return m_br.read();
}
/* 回転数目標値を返す（BL） */
inline double Chassis::bl(void)
{
	calc();
	return m_bl.read();
}
/* 生の回転数目標値を返す（FR） */
inline double Chassis::raw_fr(void)
{
	calc();
	return m_raw_rpm[0].read();
}
/* 生の回転数目標値を返す（FL） */
inline double Chassis::raw_fl(void)
{
	calc();
	return m_raw_rpm[1].read();
}
/* 生の回転数目標値を返す（BR） */
inline double Chassis::raw_br(void)
{
	calc();
	return m_raw_rpm[2].read();
}
/* 生の回転数目標値を返す（BL） */
inline double Chassis::raw_bl(void)
{
	calc();
	return m_raw_rpm[3].read();
}
/* JSON ファイルから読み込んだ max_rpm を返す */
inline double Chassis::max_rpm(void) const noexcept { return m_max_rpm; }


/*-----------------------------------------------
 *
 * LRF
 *
-----------------------------------------------*/

#define LRF_DATA_SIZE 682
/*-----------------------------------------------
パラメータ
-----------------------------------------------*/
#define MEASURE_MAX_DISTANCE 3500   /* センサで測定する最大距離 */
#define PROCESS_IMG_SCALE 0.1       /* 処理する画像サイズの倍率．ここを変更したら DetectLine のパラメータを変更する必要有 */
#define PROCESS_IMG_SIZE cv::Size(MEASURE_MAX_DISTANCE * 2 * PROCESS_IMG_SCALE, MEASURE_MAX_DISTANCE * 2 * PROCESS_IMG_SCALE)
/* detect_line */
#define DETECT_RHO 1
#define DETECT_THETA DEG_RAD(1)
#define DETECT_THRESHOLD 30
#define DETECT_MIN_LINELENGTH (300 * PROCESS_IMG_SCALE)
#define DETECT_MAX_LINEGAP (100 * PROCESS_IMG_SCALE)
#define DETECT_PLOT_THICKNESS 1			/* img にプロットする円の大きさ．PROCESS_IMG_SCALE = 1 のときは 5 ぐらい */
/* combine_line */
#define COMBINE_RAD DEG_RAD(10)
#define COMBINE_DISTANCE 50
#define COMBINE_THRESHOLD 0
/*detect_corner*/
#define DETECT_CORNER_ORTHOGONAL_ANGLE DEG_RAD(15)	/* ２線分が直交しているとみなす角度の誤差 */
#define DETECT_CORNER_ORTHOGONAL_DISTANCE 100       /* ２線分が交わっているとみなす最低の距離 */
#define DETECT_CORNER_SEPARATION_DISTANCE 100	    /* 別の角とみなす最低の距離 */
/* detect_side */
#define DETECT_SIDE_SIZE 50					/* 線分を四角形の辺だと判断するのに許容する誤差 */
#define DETECT_SIDE_ANGLE DEG_RAD(10)       /* 水平もしくは鉛直から許容する線分の角度の誤差 */
#define DETECT_SIDE_SEPARATION_DISTANCE 50

/*-----------------------------------------------
マクロ
-----------------------------------------------*/
#define ROBOT_WINDOW(p) ((p + cv::Point(MEASURE_MAX_DISTANCE, MEASURE_MAX_DISTANCE)) * PROCESS_IMG_SCALE)   /* ロボット座標系からウィンドウ座標系へ変換する */
#define WINDOW_ROBOT(p) (p / PROCESS_IMG_SCALE - cv::Point(MEASURE_MAX_DISTANCE, MEASURE_MAX_DISTANCE))     /* ウィンドウ座標系からロボット座標系へ変換する */
#ifndef BETWEEN
#define BETWEEN(smaller, input, bigger) ((smaller <= input) && (input < bigger))                             /* smaller < input < bigger を満たすかどうかを判断する */
#else
#error "BETWEEN has already defined in _LRF.h"
#endif
#ifndef CALC_SEC
#define CALC_SEC(s, e) (e.tv_sec - s.tv_sec + (e.tv_nsec - s.tv_nsec) * 1E-9)
#else
#error "CALC_SEC has already defined in _LRF.h"
#endif
#ifndef DEG_RAD
#define DEG_RAD(x) ((x) / 180.0 * M_PI)
#else
#error "DEG_RAD has already defined in _LRF.h"
#endif
#ifndef RAD_DEG
#define RAD_DEG(x) ((x) / M_PI * 180.0)
#else
#error "RAD_DEG has already defined in _LRF.h"
#endif

class Line
{
public:
	cv::Point   s, e;
	double      rad, len;
	int         group[2];
	void        calc(void);                 /* 線分の角度と長さを求める */
	double      get_line_dis(Line line);    /* 与えられた線分のクラス内の線分の中点同士の距離を返す */
	Line(void)
	{
		group[0] = group[1] = -1;
	}
};

class Box
{
public:
	cv::Point   s[2], e[2]; /* 直交しているとみなされた２線分の端点 */
	cv::Point   corner;     /* 直交しているとみなされた２線分の交点 */
};



class LRF
{
public:
	LRF(std::string json_path = "setting.json");
	~LRF(void);

	cv::Point               m_points[LRF_DATA_SIZE];                                             /* 測域センサからのデータ*/
	std::vector<Line>       m_lines;                                                             /* 測域センサのデータから検出すされた線分 */
	std::vector<Line>       m_combined_lines;                                                    /* 測域センサのデータを，近い線分同士で合成した線分 */

	void                    get_data(cv::Rect area = cv::Rect(0, 0, 0, 0));						 /* 測域センサからデータを取得する */
	void                    get_drawdata(cv::Mat& plot1, cv::Mat& plot2, cv::Size window_size);	 /* LRFの描画データの用意する*/
	std::vector<cv::Point>  detect_corner(cv::Rect ValidArea = cv::Rect(0, 0, 0, 0));            /* 四角い形状の角を検出する */
	std::vector<cv::Point>  detect_side(int length, cv::Rect ValidArea = cv::Rect(0, 0, 0, 0));  /* 四角い形状の辺を検出する */

private:
	urg_t	    m_urg;
	long*	    m_length_data;
	char  	    m_connect_device[30];
	long  	    m_connect_baudrate;

	std::vector<Line>       detect_line();					/* 測域センサのデータから線分を検出する */
	std::vector<Line>       combine_line();				    /* 測域センサーからの線分を，近い線分同士で合成する */
	double                  calc_rad();                     /*測域センサーからの線分を近い線分同士で合成された線分を代表する角度をラジアンで求める */
	Line        combine_by_average(std::vector<Line> in);   /* 与えられたグループの線分の座標の平均値で合成した直線を返す */
	double      get_dis(cv::Point a, cv::Point b);          /* 与えられた２点間の距離を返す */
	double      get_dis2(cv::Point point);                  /* 原点から与えられた点までの距離を返す */
	cv::Point   get_mid_point(cv::Point p1, cv::Point p2);  /* 与えられた２点の中点の座標を返す */
	void        SwapP(cv::Point* a, cv::Point* b);          /* 与えられた２点の座標を入れ替える */
};



#endif