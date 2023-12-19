#ifndef GLOBALSETTING_H
#define GLOBALSETTING_H
#include <QString>

class GlobalSetting
{
public:
    static GlobalSetting&instance();
private:
    GlobalSetting();

public:
    QString m_app_dir;

};

#endif // GLOBALSETTING_H
