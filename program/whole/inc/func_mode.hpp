#ifndef FUNC_COM_HPP
#define FUNC_COM_HPP
#include "../../share/inc/_thread.hpp"

void test(jibiki::ProcOperateAuto *control,
                 std::vector<std::string> param,
                 size_t seq[]);
void pwm(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[]);
void rev(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[]);
void rot(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[]);
void odometry(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[]);
void limit(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[]);
          
#endif