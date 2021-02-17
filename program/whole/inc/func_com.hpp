#ifndef FUNC_COM
#define FUNC_COM

/* ProcParamCom */
void com_pwm_control(std::string path, std::string name);
void com_rev_control(std::string path, std::string name);
void com_rot_control(std::string path, std::string name);
void com_odmetry_control(std::string path, std::string name);
void com_limit(std::string path, std::string name);
void com_chassis_f(std::string path, std::string name);
void com_chassis_b(std::string path, std::string name);
void com_switch(std::string path, std::string name);
void com_imu(std::string path, std::string name);
void com_controller(std::string path, std::string name);

#endif