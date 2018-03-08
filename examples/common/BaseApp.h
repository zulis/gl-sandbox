#ifndef PROJECT_BASE_APP_H
#define PROJECT_BASE_APP_H

class BaseApp
{
public:
    BaseApp();
    virtual ~BaseApp() = default;

    void run();
    void quit();

private:
    bool m_running;
};


#endif //PROJECT_BASE_APP_H
