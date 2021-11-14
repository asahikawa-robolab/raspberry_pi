/* Last updated : 2020/10/04, 20:36 */
#define _USE_MATH_DEFINES
#include <sstream>
#include <opencv2/opencv.hpp>
#include "../inc/module.hpp"
#include "../inc/_std_func.hpp"

/*-----------------------------------------------
*
* SwitchData
*
-----------------------------------------------*/
/* 受信データをメンバにセットする */
void SwitchData::set(jibiki::ParamCom& com)
{
	m_push_l = (com.rx(0) >> 0) & 0b1;
	m_push_r = (com.rx(0) >> 1) & 0b1;
	m_toggle[0] = (com.rx(0) >> 2) & 0b1;
	m_toggle[1] = (com.rx(0) >> 3) & 0b1;
	m_toggle[2] = (com.rx(0) >> 4) & 0b1;
	m_toggle[3] = (com.rx(0) >> 5) & 0b1;
	m_toggle[4] = (com.rx(0) >> 6) & 0b1;
	m_toggle[5] = (com.rx(0) >> 7) & 0b1;
	m_toggle[6] = (com.rx(1) >> 0) & 0b1;
	m_lu = (com.rx(1) >> 1) & 0b1;
	m_ld = (com.rx(1) >> 2) & 0b1;
	m_ru = (com.rx(1) >> 3) & 0b1;
	m_rd = (com.rx(1) >> 4) & 0b1;
	m_slide = com.rx(2);
}

/*-----------------------------------------------
*
* Controller
*
-----------------------------------------------*/
Controller::Controller(std::string json_path)
	: m_speed(0), m_theta{}, m_time(jibiki::get_time()),
	m_l_cross_l(false), m_l_cross_r(false),
	m_l_cross_u(false), m_l_cross_d(false),
	m_r_cross_l(false), m_r_cross_r(false),
	m_r_cross_u(false), m_r_cross_d(false),
	m_l_switch_d(false), m_l_switch_m(false),
	m_r_switch_d(false), m_r_switch_m(false),
	m_l_lever_l(false), m_l_lever_r(false),
	m_r_lever_l(false), m_r_lever_r(false),
	m_l_analog_stick_h(127), m_l_analog_stick_v(127),
	m_r_analog_stick_h(127), m_r_analog_stick_v(127),
	m_l_slide(false), m_r_slide(false),
	m_l_switch_u(false), m_r_switch_u(false),
	m_tact_lu(false), m_tact_mu(false),
	m_tact_ru(false), m_tact_ld(false),
	m_tact_md(false), m_tact_rd(false)
{
	/* JSON の読み込み */
	try
	{
		using picojson::object;
		picojson::value json_val = jibiki::load_json_file(json_path);
		m_calc_period_ms = (size_t)json_val.get<object>()["system"]
			.get<object>()["calc_period_ms"]
			.get<double>();
	}
	catch (const std::exception& e)
	{
		std::stringstream sstr;
		sstr << json_path << " 中の system の書式が不適切です．";
		jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
		throw std::runtime_error(""); /* エラー発生 */
	}
	/* エラーチェック */
	if (m_calc_period_ms <= 0)
	{
		std::stringstream sstr;
		sstr << json_path << "中の system/calc_period_ms の値が不適切です．\n"
			<< "正の値を指定してください．";
		jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
		throw std::runtime_error(""); /* エラー発生 */
	}
}
/* 受信データをメンバにセットする */
void Controller::set(jibiki::ParamCom& com)
{
	/* 値を読み込む */
	m_l_cross_l = (com.rx(0) >> 0) & 0b1;
	m_l_cross_r = (com.rx(0) >> 1) & 0b1;
	m_l_cross_u = (com.rx(0) >> 2) & 0b1;
	m_l_cross_d = (com.rx(0) >> 3) & 0b1;
	m_r_cross_l = (com.rx(0) >> 4) & 0b1;
	m_r_cross_r = (com.rx(0) >> 5) & 0b1;
	m_r_cross_u = (com.rx(0) >> 6) & 0b1;
	m_r_cross_d = (com.rx(0) >> 7) & 0b1;
	m_l_switch_d = (com.rx(1) >> 0) & 0b1;
	m_l_switch_m = (com.rx(1) >> 1) & 0b1;
	m_r_switch_d = (com.rx(1) >> 2) & 0b1;
	m_r_switch_m = (com.rx(1) >> 3) & 0b1;
	m_l_lever_l = (com.rx(1) >> 4) & 0b1;
	m_l_lever_r = (com.rx(1) >> 5) & 0b1;
	m_r_lever_l = (com.rx(1) >> 6) & 0b1;
	m_r_lever_r = (com.rx(1) >> 7) & 0b1;
	m_l_analog_stick_h = com.rx(2);
	m_l_analog_stick_v = com.rx(3);
	m_r_analog_stick_h = com.rx(4);
	m_r_analog_stick_v = com.rx(5);
	m_l_slide = (com.rx(6) >> 0) & 0b1;
	m_r_slide = (com.rx(6) >> 1) & 0b1;
	m_l_switch_u = (com.rx(6) >> 6) & 0b1;
	m_r_switch_u = (com.rx(6) >> 7) & 0b1;
	m_tact_lu = (com.rx(7) >> 0) & 0b1;
	m_tact_mu = (com.rx(7) >> 1) & 0b1;
	m_tact_ru = (com.rx(7) >> 2) & 0b1;
	m_tact_ld = (com.rx(7) >> 3) & 0b1;
	m_tact_md = (com.rx(7) >> 4) & 0b1;
	m_tact_rd = (com.rx(7) >> 5) & 0b1;
}
/* アナログスティックの値から角度を計算 */
double Controller::my_atan(double y, double x, DirNum dir_num) const
{
	const double M_PI_6 = M_PI / 6;
	const double M_PI_3 = M_PI / 3;
	double theta = std::atan2(y, x);
	if (dir_num == DIR_8)
	{
		if (jibiki::between(-M_PI, theta, -M_PI_6 * 5))
			theta = -M_PI;
		else if (jibiki::between(-M_PI_6 * 5, theta, -M_PI_3 * 2))
			theta = -M_PI_4 * 3;
		else if (jibiki::between(-M_PI_3 * 2, theta, -M_PI_3))
			theta = -M_PI_2;
		else if (jibiki::between(-M_PI_3, theta, -M_PI_6))
			theta = -M_PI_4;
		else if (jibiki::between(-M_PI_6, theta, M_PI_6))
			theta = 0;
		else if (jibiki::between(M_PI_6, theta, M_PI_3))
			theta = M_PI_4;
		else if (jibiki::between(M_PI_3, theta, M_PI_3 * 2))
			theta = M_PI_2;
		else if (jibiki::between(M_PI_3 * 2, theta, M_PI_6 * 5))
			theta = M_PI_4 * 3;
		else if (jibiki::between2(M_PI_6 * 5, theta, M_PI))
			theta = M_PI;
	}
	else if (dir_num == DIR_4)
	{
		if (jibiki::between(-M_PI, theta, -M_PI_4 * 3))
			theta = -M_PI;
		else if (jibiki::between(-M_PI_4 * 3, theta, -M_PI_4))
			theta = -M_PI_2;
		else if (jibiki::between(-M_PI_4, theta, M_PI_4))
			theta = 0;
		else if (jibiki::between(M_PI_4, theta, M_PI_4 * 3))
			theta = M_PI_2;
		else if (jibiki::between2(M_PI_4 * 3, theta, M_PI))
			theta = M_PI;
	}
	return theta;
}

/* アナログスティックのデータを大きさ，向きに変換 */
void Controller::convt(Mode mode, DirNum dir_num)
{
	/*-----------------------------------------------
	前回実行してから m_calc_period_ms 経過していなかったら終了
	-----------------------------------------------*/
	double passed_time = jibiki::calc_sec(m_time.read(), jibiki::get_time());
	if (passed_time < m_calc_period_ms * 1E-3)
		return;
	else
		m_time = jibiki::get_time();

	/*-----------------------------------------------
	データを用意
	-----------------------------------------------*/
	/* mode に対応した生データを読み込む */
	double horizontal = (mode == MODE_L)
		? m_l_analog_stick_h.read()
		: m_r_analog_stick_h.read();
	double vertical = (mode == MODE_L)
		? m_l_analog_stick_v.read()
		: m_r_analog_stick_v.read();
	/* 0～255 を -127.5～127.5 に変更 */
	double x = horizontal - 127.5;
	double y = vertical - 127.5;

	/*-----------------------------------------------
	スピード [0%～100%]
	-----------------------------------------------*/
	const double INSENSIBLE_FIELD_SIZE = 30;                                /* 不感領域の大きさ（原点中心の正方形の辺長 ÷ 2） */
	double speed = sqrt(x * x + y * y);                                     /* 大きさ（原点からの距離）を求める */
	if (fabs(x) < INSENSIBLE_FIELD_SIZE && fabs(y) < INSENSIBLE_FIELD_SIZE) /* 不感領域にいる場合 0 にする */
		speed = 0;
	speed *= 100 / 127.5;                /* 百分率に変換 */
	speed = (speed > 100) ? 100 : speed; /* 100 以下に収める */

	/*-----------------------------------------------
	向き [rad]
	-----------------------------------------------*/
	double theta = my_atan(y, x, dir_num);

	/*-----------------------------------------------
	値を返す
	-----------------------------------------------*/
	m_speed = speed;
	m_theta[mode] = (speed != 0) ? theta : m_theta[mode].read(); /* 不感領域では theta を変更しない */
}

/*-----------------------------------------------
 *
 * Chassis
 *
-----------------------------------------------*/
/* コンストラクタ */
Chassis::Chassis(Imu& imu, std::string json_path)
	: m_imu(&imu),
	m_json_path(json_path),
	m_time(jibiki::get_time()),
	m_speed(0),
	m_theta(jibiki::deg_rad(90)),
	m_spin(0),
	m_turn_mode(TURN_SHORTEST)
{
	try
	{
		/* JSON ファイルから設定を読み込む */
		load_json();
	}
	catch (const std::exception& e)
	{
		jibiki::print_err(__PRETTY_FUNCTION__);
		throw; /* 仲介 */
	}
}

/* JSON ファイルを読み込む */
void Chassis::load_json(void)
{
	using picojson::object;
	double c1, c2, c3, c4; /* channel */

	/*-----------------------------------------------
	ファイルを読み込む
	-----------------------------------------------*/
	try
	{
		/* chassis */
		picojson::value json_value = jibiki::load_json_file(m_json_path);
		object& obj = json_value.get<object>()["chassis"].get<object>();
		/* max_rpm */
		m_max_rpm = obj["max_rpm"].get<double>();
		/* channel */
		c1 = obj["channel"].get<object>()["fr"].get<double>();
		c2 = obj["channel"].get<object>()["fl"].get<double>();
		c3 = obj["channel"].get<object>()["br"].get<double>();
		c4 = obj["channel"].get<object>()["bl"].get<double>();
		m_channel_fr = (size_t)c1;
		m_channel_fl = (size_t)c2;
		m_channel_br = (size_t)c3;
		m_channel_bl = (size_t)c4;
		/* inverse */
		m_inverse_fr = obj["inverse"].get<object>()["fr"].get<bool>();
		m_inverse_fl = obj["inverse"].get<object>()["fl"].get<bool>();
		m_inverse_br = obj["inverse"].get<object>()["br"].get<bool>();
		m_inverse_bl = obj["inverse"].get<object>()["bl"].get<bool>();
		/* rotate */
		m_rotate_min = obj["rotate"].get<object>()["min"].get<double>();
		m_rotate_max = obj["rotate"].get<object>()["max"].get<double>();
		m_rotate_kp = obj["rotate"].get<object>()["kp"].get<double>();
	}
	catch (const std::exception& e)
	{
		std::stringstream sstr;
		sstr << m_json_path << " 中の chassis の書式が不適切です．";
		jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
		throw std::runtime_error(""); /* エラー発生 */
	}
	/*-----------------------------------------------
	エラーチェック
	-----------------------------------------------*/
	try
	{
		bool v_fr = c1 == 0 || c1 == 1 || c1 == 2 || c1 == 3;
		bool v_fl = c2 == 0 || c2 == 1 || c2 == 2 || c2 == 3;
		bool v_br = c3 == 0 || c3 == 1 || c3 == 2 || c3 == 3;
		bool v_bl = c4 == 0 || c4 == 1 || c4 == 2 || c4 == 3;
		if ((v_fr & v_fl & v_br & v_bl) == false)
			throw std::string("chassis/channel の値が不適切です．\n"
				" 0, 1, 2, 3 のいずれかを指定してください．");
		bool eq1 = c1 == c2 || c1 == c3 || c1 == c4;
		bool eq2 = c2 == c3 || c2 == c4;
		bool eq3 = c3 == c4;
		if (eq1 | eq2 | eq3)
			throw std::string("chassis/channel の値が不適切です．\n"
				"重複しない相異なる値を指定してください．"); /* エラー発生 */
		if (m_max_rpm <= 0)
			throw std::string("chassis/max_rpm の値が不適切です．\n"
				"正の値を指定してください．"); /* エラー発生 */
		if (m_rotate_min <= 0)
			throw std::string("chassis/rotate/min の値が不適切です．\n"
				"正の値を指定してください．"); /* エラー発生 */
		if (m_rotate_max <= 0)
			throw std::string("chassis/rotate/max の値が不適切です．\n"
				"正の値を指定してください．"); /* エラー発生 */
		if (m_rotate_kp <= 0)
			throw std::string("chassis/rotate/kp の値が不適切です．\n"
				"正の値を指定してください．"); /* エラー発生 */
	}
	catch (std::string err)
	{
		std::stringstream sstr;
		sstr << m_json_path << " 中の " << err;
		jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
		throw std::runtime_error(""); /* エラー発生 */
	}
	/*-----------------------------------------------
	system_calc_period_ms
	-----------------------------------------------*/
	try
	{
		picojson::value json_val = jibiki::load_json_file(m_json_path);
		m_calc_period_ms = (size_t)json_val.get<object>()["system"]
			.get<object>()["calc_period_ms"]
			.get<double>();
	}
	catch (const std::exception& e)
	{
		std::stringstream sstr;
		sstr << m_json_path << " 中の system の書式が不適切です．";
		jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
		throw std::runtime_error(""); /* エラー発生 */
	}
	/*-----------------------------------------------
	エラーチェック
	-----------------------------------------------*/
	if (m_calc_period_ms <= 0)
	{
		std::stringstream sstr;
		sstr << m_json_path << " 中の system/calc_period_ms の値が不適切です．\n"
			"正の値を指定してください．";
		jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
		throw std::runtime_error(""); /* エラー発生 */
	}
}
/* 各モータの回転数目標値を計算 */
void Chassis::calc(void)
{
	/*-----------------------------------------------
	前回実行してから m_calc_period_ms 経過していなかったら終了
	-----------------------------------------------*/
	double passed_time = jibiki::calc_sec(m_time.read(), jibiki::get_time());
	if (passed_time < 50E-3)
		return;
	else
		m_time = jibiki::get_time();

	/*-----------------------------------------------
	エラーチェック
	-----------------------------------------------*/
	if (!jibiki::between2(0.0, m_speed.read(), m_max_rpm))
	{
		std::stringstream sstr;
		sstr << "Chassis::m_speed の値 (" << m_speed.read() << ") が不適切です．\n "
			<< m_json_path << " の chassis/max_rpm の値 (" << max_rpm()
			<< ") を超えないようにしてください．";
		jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
		throw std::runtime_error(""); /* エラー発生 */
	}

	double rotate = calc_rotate();       /* 回転量を計算 */
	double current_spin = m_imu->read(); /* 現在の回転角を取得 */

	/*-----------------------------------------------
	目標回転数を計算
	-----------------------------------------------*/
	double val_1 = m_speed.read() * sin(m_theta.read() - current_spin - M_PI_4);
	double val_2 = m_speed.read() * sin(m_theta.read() - current_spin + M_PI_4);
	m_raw_rpm[0] = val_1 + rotate; /* FR */
	m_raw_rpm[1] = val_2 - rotate; /* FL */
	m_raw_rpm[2] = val_2 + rotate; /* BR */
	m_raw_rpm[3] = val_1 - rotate; /* BL */

	/*-----------------------------------------------
	rotate も含めて m_max_rpm を超えないようにする
	-----------------------------------------------*/
	/* m_raw_rpm の中で絶対値が最大のものを求める */
	double max_val = 0;
	for (size_t i = 0; i < 4; ++i)
		if (fabs(m_raw_rpm[i].read()) > max_val)
			max_val = fabs(m_raw_rpm[i].read());

	/* rotate を加減算して m_max_rpm を超える場合 */
	if (max_val > m_max_rpm)
	{
		/* rotate を修正 */
		double diff = m_max_rpm - std::max(fabs(val_1), fabs(val_2));
		rotate = jibiki::get_signal(rotate) * diff;
		/* 目標回転数を再計算 */
		m_raw_rpm[0] = val_1 + rotate; /* FR */
		m_raw_rpm[1] = val_2 - rotate; /* FL */
		m_raw_rpm[2] = val_2 + rotate; /* BR */
		m_raw_rpm[3] = val_1 - rotate; /* BL */
	}

	/*-----------------------------------------------
	m_channel_x に従って値を入れ替える
	-----------------------------------------------*/
	m_fr = m_raw_rpm[m_channel_fr].read();
	m_fl = m_raw_rpm[m_channel_fl].read();
	m_br = m_raw_rpm[m_channel_br].read();
	m_bl = m_raw_rpm[m_channel_bl].read();

	/*-----------------------------------------------
	m_inverse_x に従って極性を反転する
	-----------------------------------------------*/
	if (m_inverse_fr)
		m_fr *= -1;
	if (m_inverse_fl)
		m_fl *= -1;
	if (m_inverse_br)
		m_br *= -1;
	if (m_inverse_bl)
		m_bl *= -1;
}
/* 回転量を計算 */
double Chassis::calc_rotate(void)
{
	/* 角度偏差を計算 */
	double diff =
		calc_angle_diff(m_spin.read(), m_imu->read(), m_turn_mode.read());

	/* 偏差が小さくなったら最短モードに変更 */
	/* （オーバーシュートしたときにもう一回転してしまうから） */
	if (fabs(diff) < jibiki::deg_rad(90))
		m_turn_mode = TURN_SHORTEST;

	/* 偏差に応じて rotate を決定 */
	double rotate;
	if (fabs(diff) < jibiki::deg_rad(2))
	{
		rotate = 0;
	}
	else
	{
		rotate = diff * m_rotate_kp;
		if (fabs(rotate) < m_rotate_min)
			rotate = m_rotate_min * jibiki::get_signal(rotate);
		else if (m_rotate_max < fabs(rotate))
			rotate = m_rotate_max * jibiki::get_signal(rotate);
	}

	return rotate;
}
/* -pi ~ pi の不連続部に影響されない角度偏差（subed - sub）を返す */
double Chassis::calc_angle_diff(double subed, double sub, TurnMode turn_mode)
{
	/* 角度を -pi ~ pi にする */
	double subed_limited = jibiki::limit_angle(subed);
	double sub_limited = jibiki::limit_angle(sub);
	double angle_diff = 0;

	switch (turn_mode)
	{
	case TURN_CCW:
		if (subed_limited - sub_limited >= 0)
			angle_diff = subed_limited - sub_limited;
		else
			angle_diff = subed_limited - sub_limited + 2 * M_PI;
		break;
	case TURN_CW:
		if (subed_limited - sub_limited > 0)
			angle_diff = subed_limited - sub_limited - 2 * M_PI;
		else
			angle_diff = subed_limited - sub_limited;
		break;
	case TURN_SHORTEST:
		if (subed_limited - sub_limited > M_PI)
			angle_diff = (subed_limited - sub_limited) - 2 * M_PI;
		else if (subed_limited - sub_limited < -M_PI)
			angle_diff = (subed_limited - sub_limited) + 2 * M_PI;
		else
			angle_diff = subed_limited - sub_limited;
		break;
	}
	return angle_diff;
}

/* CombineLine */
#define TILT 0
#define DISTANCE 1
/* CalcRad */
#define _M90_M60 0  /* M : マイナス，P : プラス */
#define _M60_M30 1
#define _M30_0 2
#define _0_P30 3
#define _P30_P60 4
#define _P60_P90 5
/* detect_corner */
#define DETECT_CORNER_S0 0
#define DETECT_CORNER_S1 1
#define DETECT_CORNER_E0 2
#define DETECT_CORNER_E1 3
/*-----------------------------------------------
*
* Line
*
-----------------------------------------------*/
/*-----------------------------------------------
線分の角度と長さを求める
-----------------------------------------------*/
void Line::calc(void)
{
	double 	x = e.x - s.x,
		y = e.y - s.y;

	rad = atan(y / x);
	len = sqrt(x * x + y * y);
}
/*-----------------------------------------------
与えられた線分のクラス内の線分の中点同士の距離を返す
-----------------------------------------------*/
double Line::get_line_dis(Line line)
{
	cv::Point2d myself, another;

	myself.x = (s.x + e.x) / 2;
	myself.y = (s.y + e.y) / 2;
	another.x = (line.s.x + line.e.x) / 2;
	another.y = (line.s.y + line.e.y) / 2;

	double x = another.x - myself.x,
		y = another.y - myself.y;

	return (sqrt(x * x + y * y));
}


/*-----------------------------------------------
*
* _LRF
*
-----------------------------------------------*/

LRF::LRF(std::string json_path)
{
	printf("LRF constructed.\n");
	snprintf(m_connect_device, 30, "/dev/ttyACM0");
	m_connect_baudrate = 115200;
	int ret = urg_open(&m_urg, URG_SERIAL, m_connect_device, m_connect_baudrate);		/* センサに接続 */
	/* エラーチェック */
	if (ret < 0)
	{
		printf("OpenError LRF.[%s]\n", m_connect_device);
		std::stringstream sstr;
		sstr << " LRF[" << m_connect_device << "]が開けません．\n"
			"接続を確認してください。．";
		jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
		throw std::runtime_error(""); /* エラー発生 */
	}

	m_length_data = (long*)malloc(sizeof(long) * urg_max_data_size(&m_urg));	/* データを格納する領域を確保する */
}

LRF::~LRF()
{
	urg_close(&m_urg);    /* センサとの接続を閉じる */
	free(m_length_data);  /* データの領域を解放する */
	printf("LRF destructed.\n");
}

/*-----------------------------------------------
*
* 測域センサからデータを取得する
* cv::Point *points：センサから受け取るデータ
* 1. 測定
* 2. 極座標から直交座標に変換
* 3. センサの向きが分かりやすいように90°回転
*
-----------------------------------------------*/
void LRF::get_data(cv::Rect area)
{
	int length_data_size;
	urg_start_measurement(&m_urg, URG_DISTANCE, 1, 0);          		/* 距離データの計測開始 */
	length_data_size = urg_get_distance(&m_urg, m_length_data, NULL);	/* センサから距離データを取得する */

	for (int i = 0; i < length_data_size; i++)
	{
		double  radian;
		long    length;
		long    x, y, tmp;

		radian = urg_index2rad(&m_urg, i);
		length = m_length_data[i];

		/* 極座標から直交座標に変換 */
		x = (long)(length * cos(radian));
		y = (long)(length * sin(radian));

		/* データを90°回転 */
		tmp = x;
		x = -y;
		y = tmp;

		if (area == cv::Rect(0, 0, 0, 0))
			m_points[i] = cv::Point(x, y);
		else if (BETWEEN(area.x, x, area.x + area.width) && BETWEEN(area.y, y, area.y + area.height))
			m_points[i] = cv::Point(x, y);
	}

	/*計算データの更新*/
	m_lines = detect_line();
	m_combined_lines = combine_line();
	calc_rad();
}

/*-----------------------------------------------
*
* LRFの描画データを用意する
*
-----------------------------------------------*/
void LRF::get_drawdata(cv::Mat& plot1, cv::Mat& plot2, cv::Size window_size)
{
	char str[30];

	std::vector<cv::Point> corners = detect_corner(/* , cv::Rect(-500, 500, 500, 500) */);
	std::vector<cv::Point> sides = detect_side(500/* , cv::Rect(400, 800, 100, 100) */);


	/* 表示 */
	for (int i = 0; i < LRF_DATA_SIZE; i++)
		cv::circle(plot1, ROBOT_WINDOW(m_points[i]), 1, cv::Scalar(255, 255, 255), 2);
	for (size_t i = 0; i < m_lines.size(); i++)
		cv::line(plot1, ROBOT_WINDOW(m_lines[i].s), ROBOT_WINDOW(m_lines[i].e), cv::Scalar(0, 0, 255), 2);
	for (size_t i = 0; i < m_combined_lines.size(); i++)
		cv::line(plot1, ROBOT_WINDOW(m_combined_lines[i].s), ROBOT_WINDOW(m_combined_lines[i].e), cv::Scalar(0, 255, 0), 2);
	for (size_t i = 0; i < corners.size(); i++)
		cv::circle(plot1, ROBOT_WINDOW(corners[i]), 8, cv::Scalar(0xFF, 0xBF, 0x00), 2);
	for (size_t i = 0; i < sides.size(); i++)
		cv::circle(plot1, ROBOT_WINDOW(sides[i]), 8, cv::Scalar(0xFF, 0xBF, 0x00), 2);
	cv::resize(plot1, plot2, window_size);
	cv::flip(plot2, plot2, 0);
	snprintf(str, 30, "detected lines : %d", m_lines.size());
	cv::putText(plot2, str, cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255));
	snprintf(str, 30, "combined lines : %d", m_combined_lines.size());
	cv::putText(plot2, str, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255));
	snprintf(str, 30, "%.2lf %%", m_combined_lines.size() / (double)m_lines.size() * 100);
	cv::putText(plot2, str, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255));
}


/*-----------------------------------------------
*
* 測域センサのデータから線分を検出する
* std::vector<Line> DetectedLines	：戻り値．検出した線分のデータを返す
* 1. cv::HoughLinesP に渡す画像を作成
* 2. Hough 変換を行う
* 3. cv::HoughLinesP の出力データ（std::vector<cv::Vec4i>）を std::vector<Line> に変換する
*
-----------------------------------------------*/
std::vector<Line> LRF::detect_line()
{
	cv::Mat                 img = cv::Mat::zeros(PROCESS_IMG_SIZE, CV_8UC1);   /* cv::HoughLinesP に渡す画像 */
	std::vector<cv::Vec4i>  HoughResult;                                       /* cv::HoughLinesP の結果 */
	std::vector<Line>       DetectedLines;                                     /* 戻り値 */

	/* cv::HoughLinesP に渡す画像を作成 */
	{
		cv::Point tmp;
		for (int i = 0; i < LRF_DATA_SIZE; i++)
		{
			tmp = ROBOT_WINDOW(m_points[i]);
			cv::circle(img, tmp, DETECT_PLOT_THICKNESS, cv::Scalar(255, 255, 255));
		}
	}

	/* Hough 変換 */
	cv::HoughLinesP(img, HoughResult, DETECT_RHO, DETECT_THETA, DETECT_THRESHOLD, DETECT_MIN_LINELENGTH, DETECT_MAX_LINEGAP);

	/* std::vector<cv::Vec4i>（HoughResult）を std::vector<Line>（DetectedLines）に変換する */
	{
		Line DetectedLine;
		for (size_t i = 0; i < HoughResult.size(); i++)
		{
			DetectedLine.s = WINDOW_ROBOT(cv::Point(HoughResult[i][0], HoughResult[i][1]));
			DetectedLine.e = WINDOW_ROBOT(cv::Point(HoughResult[i][2], HoughResult[i][3]));
			DetectedLine.calc();
			DetectedLines.push_back(DetectedLine);
		}
	}
	return DetectedLines;
}

/*-----------------------------------------------
*
* 測域センサーからの線分を，近い線分同士で合成する
* std::vector<Line> CombinedLines	：戻り値．合成した線分を返す
* 1. 傾きが近い線分同士をいくつかのグループに分ける
* 2. 1 で分けたグループから，さらに距離が近い線分同士をいくつかのグループに分ける
* 3. 2 で分けたグループの線分を，グループ内の線分同士で一の線分に合成する
*
-----------------------------------------------*/
std::vector<Line> LRF::combine_line()
{
	/* 変数 */
	std::vector<Line> 				CombinedLines;	/* 合成した線分．戻り値． */
	std::vector<std::vector<Line>>	classed_tilt,	/* 傾きで分類された線分 */
		classed_dis;	/* 距離で分類された線分 */

/* 傾きで線分を分類し，classed_tilt を作成 */
	{
		int group = 0;
		std::vector<Line> tmp;

		for (size_t i = 0; i < m_lines.size(); i++)
		{
			/* もしグループ分けされていなかったら */
			if (m_lines[i].group[TILT] == -1)
			{
				for (size_t j = 0; j < m_lines.size(); j++)
				{
					if (m_lines[j].group[TILT] == -1)
					{
						if (fabs(m_lines[i].rad - m_lines[j].rad) < COMBINE_RAD)
						{
							m_lines[j].group[TILT] = group;
							tmp.push_back(m_lines[j]);
						}
					}
				}
				group++;
				classed_tilt.push_back(tmp);
				tmp.clear();
			}
		}
	}

	/* 距離で線分を分類し，classed_dis を作成 */
	{
		/* 距離で線分をグループ分けする */
		int NumGroup = 0;

		for (size_t k = 0; k < classed_tilt.size(); k++)
		{
			for (size_t i = 0; i < classed_tilt[k].size(); i++)
			{
				if (classed_tilt[k][i].group[DISTANCE] == -1)
				{
					classed_tilt[k][i].group[DISTANCE] = NumGroup;
					for (size_t j = 0; j < classed_tilt[k].size(); j++)
						if (i != j && classed_tilt[k][i].get_line_dis(classed_tilt[k][j]) < COMBINE_DISTANCE)
							classed_tilt[k][j].group[DISTANCE] = NumGroup;
					NumGroup++;
				}
			}
		}

		/* 距離による分類のグループで並べ替えて classed_dis を作成 */
		std::vector<Line> tmp;

		for (size_t k = 0; k < classed_tilt.size(); k++)
		{
			/* Num : 距離による分類のグループ数 */
			for (int j = 0; j < NumGroup + 1; j++)
			{
				for (size_t i = 0; i < classed_tilt[k].size(); i++)
					if (classed_tilt[k][i].group[DISTANCE] == j)
						tmp.push_back(classed_tilt[k][i]);
				if (!tmp.empty())
				{
					classed_dis.push_back(tmp);	/* classed_dis に追加 */
					tmp.clear();
				}
			}
		}
	}

	/* 線分を一つにまとめて CombinedLines を作成 */
	/* グループの線分の座標の平均値で直線を合成する */
	{
		Line tmp;

		for (size_t i = 0; i < classed_dis.size(); i++)
		{
			/* 同じグループの線分の数が COMBINE_THRESHOLD より多かったら CombinedLines に追加 */
			if (classed_dis[i].size() > COMBINE_THRESHOLD)
			{
				tmp = combine_by_average(classed_dis[i]);
				tmp.group[TILT] = classed_dis[i][0].group[TILT];
				tmp.group[DISTANCE] = classed_dis[i][0].group[DISTANCE];
				CombinedLines.push_back(tmp);	/* Outputに追加 */
			}
		}
		for (size_t i = 0; i < CombinedLines.size(); i++)
			CombinedLines[i].calc();	/* 線分の傾き，長さを計算 */
	}

	return CombinedLines;
}

/*-----------------------------------------------
*
* 測域センサーからの線分を近い線分同士で合成された線分を代表する角度をラジアンで求める
* double result					：戻り値．線分を代表する角度を返す．使用状況により return を記述を適宜変更する
* 1. 入力された線分を 30° 刻みでグループに分ける
* 2. グループに所属している線分の数がもっとも多いグループを探す
* 3. 2 で探したグループの所属している線分の角度の平均値を求める
*
-----------------------------------------------*/
double LRF::calc_rad()
{
	std::vector<double> group[6];
	double result;

	/* m_combined_lines の角度を 30°刻みに group[] に分類する */
	for (size_t i = 0; i < m_combined_lines.size(); i++)
	{
		if (BETWEEN(-90, RAD_DEG(m_combined_lines[i].rad), -60)) group[_M90_M60].push_back(m_combined_lines[i].rad);
		else if (BETWEEN(-60, RAD_DEG(m_combined_lines[i].rad), -30))	group[_M60_M30].push_back(m_combined_lines[i].rad);
		else if (BETWEEN(-30, RAD_DEG(m_combined_lines[i].rad), 0))	group[_M30_0].push_back(m_combined_lines[i].rad);
		else if (BETWEEN(0, RAD_DEG(m_combined_lines[i].rad), 30))	group[_0_P30].push_back(m_combined_lines[i].rad);
		else if (BETWEEN(30, RAD_DEG(m_combined_lines[i].rad), 60))	group[_P30_P60].push_back(m_combined_lines[i].rad);
		else if (BETWEEN(60, RAD_DEG(m_combined_lines[i].rad), 90))	group[_P60_P90].push_back(m_combined_lines[i].rad);
	}

	{
		size_t max = 0, max_group;
		double sum = 0;
		/* 分類された角度の数が最も多いグループを求める */
		for (int i = 0; i < 6; i++)
		{
			if (max <= group[i].size())
			{
				max = group[i].size();
				max_group = i;
			}
		}

		/* 分類された角度の数が最も多いグループに属している角度の平均値を求める */
		for (size_t i = 0; i < group[max_group].size(); i++)
			sum += group[max_group][i];
		result = sum / group[max_group].size();
	}

	/* センサの使用状況により適宜変更 */
	if (result < 0)	return -(-M_PI / 2.0 - result);
	else			return -(M_PI / 2.0 - result);
}

/*-----------------------------------------------
*
* 四角い形状の角を検出する
* cv::Rect ValidArea					：角を探す範囲の指定．省略可能
* std::vector<cv::Point> DetectedCorner	：検出した角の座標
* 1. 与えられた線分から直交している線分を探す
* 2. 1 の線分から角の座標（corner）を求める
* 3. 距離の近い corner を平均値で合成してまとめる
* 4. 3 までで検出した角が ValidArea で指定した領域内にあるかを判断する
*
-----------------------------------------------*/
std::vector<cv::Point> LRF::detect_corner(cv::Rect ValidArea)
{
	std::vector<std::vector<Box>> BoxCand;	/* BoxCandidate の略 */
	std::vector<cv::Point> DetectedCorner;

	/*-----------------------------------------------
	m_combined_lines が空だったらその時点で処理を終了する
	-----------------------------------------------*/
	if (m_combined_lines.empty() == true)
		return DetectedCorner;

	/*-----------------------------------------------
	直交している線分を検出して BoxCand を作成
	-----------------------------------------------*/
	{
		std::vector<Box> boxes;
		Box box;

		for (size_t j = 0; j < m_combined_lines.size(); j++)
		{
			box.s[0] = m_combined_lines[j].s;
			box.e[0] = m_combined_lines[j].e;
			for (size_t i = j; i < m_combined_lines.size(); i++)
			{
				/* ２線分の交わっている角度の 90°からの誤差が閾値以下か判断する */
				/* fabs(m_combined_lines[i].rad - m_combined_lines[j].rad)			：２直線の交角の大きさ */
				/* fabs(m_combined_lines[i].rad - m_combined_lines[j].rad) - M_PI_2	：２直線の交角の大きさの 90°からの差 */
				if (BETWEEN(-DETECT_CORNER_ORTHOGONAL_ANGLE, fabs(m_combined_lines[i].rad - m_combined_lines[j].rad) - M_PI_2, DETECT_CORNER_ORTHOGONAL_ANGLE))
				{
					/* 直交している線分の端点同士の距離を計算して閾値以下だったものはすべて採用 */
					if (get_dis(m_combined_lines[i].s, m_combined_lines[j].s) < DETECT_CORNER_ORTHOGONAL_DISTANCE
						|| get_dis(m_combined_lines[i].s, m_combined_lines[j].e) < DETECT_CORNER_ORTHOGONAL_DISTANCE
						|| get_dis(m_combined_lines[i].e, m_combined_lines[j].s) < DETECT_CORNER_ORTHOGONAL_DISTANCE
						|| get_dis(m_combined_lines[i].e, m_combined_lines[j].e) < DETECT_CORNER_ORTHOGONAL_DISTANCE)
					{
						box.s[1] = m_combined_lines[i].s;
						box.e[1] = m_combined_lines[i].e;
						boxes.push_back(box);
					}
				}
			}
			if (boxes.empty() == false)
				BoxCand.push_back(boxes);
			boxes.clear();
		}
	}

	/*-----------------------------------------------
	s[], e[] から corner を作成する
	s[0] と s[1] が端点同士の最短距離の組み合わせになるようにする
	corner と e[] の位置を比較し，外側から見た四角形の角かを判断する
	-----------------------------------------------*/
	for (size_t i = 0; i < BoxCand.size(); i++)
		for (size_t j = 0; j < BoxCand[i].size(); j++)
		{
			if (get_dis(BoxCand[i][j].s[0], BoxCand[i][j].s[1]) > get_dis(BoxCand[i][j].s[0], BoxCand[i][j].e[1]))
				SwapP(&BoxCand[i][j].s[1], &BoxCand[i][j].e[1]);
			if (get_dis(BoxCand[i][j].s[0], BoxCand[i][j].s[1]) > get_dis(BoxCand[i][j].e[0], BoxCand[i][j].s[1]))
				SwapP(&BoxCand[i][j].s[0], &BoxCand[i][j].e[0]);
			BoxCand[i][j].corner = get_mid_point(BoxCand[i][j].s[0], BoxCand[i][j].s[1]);
			/* 外側から見た角か内側から見た角かを判断する */
			if (get_dis2(BoxCand[i][j].e[0]) > get_dis2(BoxCand[i][j].corner) && get_dis2(BoxCand[i][j].e[1]) > get_dis2(BoxCand[i][j].corner))
				DetectedCorner.push_back(BoxCand[i][j].corner);
		}
	if (DetectedCorner.empty() == true)
		return DetectedCorner;

	{
		std::vector<cv::Point> tmp;
		/*-----------------------------------------------
		距離の近い corner を平均値で合成してまとめる
		-----------------------------------------------*/
		bool flag = 0;

		tmp.push_back(DetectedCorner[0]);
		for (size_t j = 1; j < DetectedCorner.size(); j++)
		{
			for (size_t i = 0; i < tmp.size(); i++)
			{
				if (get_dis(tmp[i], DetectedCorner[j]) < DETECT_CORNER_SEPARATION_DISTANCE)
				{
					tmp[i] = get_mid_point(tmp[i], DetectedCorner[j]);
					flag = 1;
				}
			}
			if (flag == 0) tmp.push_back(DetectedCorner[j]);
			flag = 0;
		}
		DetectedCorner.clear();

		/*-----------------------------------------------
		ValidArea 内にある corner のみを返す
		-----------------------------------------------*/
		/* ValidArea が指定されなかったら検出した corner をすべて返す */
		if (ValidArea == cv::Rect(0, 0, 0, 0))
			DetectedCorner = tmp;
		/* 指定された領域内にあるテーブルを探して返す */
		else
			for (size_t i = 0; i < tmp.size(); i++)
				if (BETWEEN(ValidArea.x, tmp[i].x, ValidArea.x + ValidArea.width) && BETWEEN(ValidArea.y - ValidArea.height, tmp[i].y, ValidArea.y))
					DetectedCorner.push_back(tmp[i]);
	}
	return DetectedCorner;
}

/*-----------------------------------------------
*
* 四角い形状の辺を検出する
* int length					：検出する辺の最大の長さ
* cv::Rect ValidArea			：辺の中点を探す範囲の指定．省略可能
* std::vector<cv::Point> Output	：検出した辺の中点の座標
* 1. 与えられた線分から角度が 0° もしくは 90° で， length で指定した長さの線分を探す
* 2. 距離の近い座標を平均値で合成してまとめる
* 3. 2 までで検出した座標が ValidArea で指定した領域内にあるかを判断する
*
-----------------------------------------------*/
std::vector<cv::Point> LRF::detect_side(int length, cv::Rect ValidArea)
{
	std::vector<cv::Point> Output;

	/*-----------------------------------------------
	m_combined_lines が空だったらその時点で処理を終了する
	-----------------------------------------------*/
	if (m_combined_lines.empty() == true)
		return Output;

	/*-----------------------------------------------
	テーブルの辺を検出して Output を作成
	---------------------------------------------- */
	{
		cv::Point tmp;
		for (size_t i = 0; i < m_combined_lines.size(); i++)
		{
			/* サイズ */
			if ((fabs(length - m_combined_lines[i].len) < DETECT_SIDE_SIZE)
				/* 角度 */
				&& (m_combined_lines[i].rad > M_PI_2 - DETECT_SIDE_ANGLE
					|| m_combined_lines[i].rad < -M_PI_2 + DETECT_SIDE_ANGLE))
			{
				tmp = get_mid_point(m_combined_lines[i].s, m_combined_lines[i].e);
				Output.push_back(tmp);
			}
		}
	}

	/*-----------------------------------------------
	Output が空だったらその時点で処理を終了する
	-----------------------------------------------*/
	if (Output.empty() == true)
		return Output;

	/*-----------------------------------------------
	距離の近い座標を平均値で合成してまとめる
	---------------------------------------------- */
	{
		std::vector<cv::Point> tmp;
		bool flag = 0;
		tmp.push_back(Output[0]);

		for (size_t j = 1; j < Output.size(); j++)
		{
			for (size_t i = 0; i < tmp.size(); i++)
			{
				if (get_dis(tmp[i], Output[j]) < DETECT_SIDE_SEPARATION_DISTANCE)
				{
					tmp[i] = get_mid_point(tmp[i], Output[j]);
					flag = 1;
				}
			}
			if (flag == 0)
				tmp.push_back(Output[j]);
			flag = 0;
		}
		Output.clear();

		/* 指定された領域内にあるか判断 */
		if (ValidArea == cv::Rect(0, 0, 0, 0))
			Output = tmp;
		else
			for (size_t i = 0; i < tmp.size(); i++)
				if (BETWEEN(ValidArea.x, tmp[i].x, ValidArea.x + ValidArea.width)
					&& BETWEEN(ValidArea.y - ValidArea.height, tmp[i].y, ValidArea.y))
					Output.push_back(tmp[i]);
	}
	return Output;
}

/*-----------------------------------------------
*
* CombineLine() で使用する関数
* 与えられたグループの線分の座標の平均値で合成した直線を返す
*
-----------------------------------------------*/
Line LRF::combine_by_average(std::vector<Line> in)
{
	double sum[4] = {};
	Line out;

	for (size_t i = 0; i < in.size(); i++)
	{
		sum[0] += in[i].s.x;
		sum[1] += in[i].s.y;
		sum[2] += in[i].e.x;
		sum[3] += in[i].e.y;
	}

	out.s.x = sum[0] / in.size();
	out.s.y = sum[1] / in.size();
	out.e.x = sum[2] / in.size();
	out.e.y = sum[3] / in.size();

	return out;
}
/*-----------------------------------------------
与えられた２点間の距離を返す
-----------------------------------------------*/
double LRF::get_dis(cv::Point a, cv::Point b)
{
	return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}
/*-----------------------------------------------
原点から与えられた点までの距離を返す
-----------------------------------------------*/
double LRF::get_dis2(cv::Point point)
{
	return sqrt(pow(point.x, 2) + pow(point.y, 2));
}
/*-----------------------------------------------
与えられた２点の中点の座標を返す
-----------------------------------------------*/
cv::Point LRF::get_mid_point(cv::Point p1, cv::Point p2)
{
	cv::Point tmp;
	tmp.x = (p1.x + p2.x) / 2;
	tmp.y = (p1.y + p2.y) / 2;
	return tmp;
}
/*-----------------------------------------------
与えられた２点の座標を入れ替える
-----------------------------------------------*/
void LRF::SwapP(cv::Point* a, cv::Point* b)
{
	cv::Point tmp = *a;
	*a = *b;
	*b = tmp;
}