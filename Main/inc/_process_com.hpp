#ifndef _PROCESS_COM_HPP
#define _PROCESS_COM_HPP

#include <vector>
#include <string>

typedef void (*ComFunc)(std::vector<std::string> paths);
class FuncCom
{
    public:
    FuncCom(ComFunc fp, std::string name, std::vector<std::string> paths)
    {
        m_fp = fp;
        m_name = name;
        m_paths = paths;
    }
    ComFunc m_fp;
    std::string m_name;
    std::vector<std::string> m_paths;
};

#endif