/*******************************************************************************
 * Project:  Nebula
 * @file     Session.hpp
 * @brief 
 * @author   Bwar
 * @date:    2016年8月12日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_ACTOR_SESSION_SESSION_HPP_
#define SRC_ACTOR_SESSION_SESSION_HPP_

#include <queue>
#include "labor/Worker.hpp"
#include "actor/DynamicCreator.hpp"
#include "SessionModel.hpp"

namespace neb
{

class Session: public SessionModel
{
public:
    Session(uint32 ulSessionId, ev_tstamp dSessionTimeout = 60.0);
    Session(const std::string& strSessionId, ev_tstamp dSessionTimeout = 60.0);
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    virtual ~Session();

    /**
     * @brief 会话超时回调
     */
    virtual E_CMD_STATUS Timeout() = 0;

    /**
     * @brief 检查Session内数据是否已加载
     * @note 为满足数据共享并确保同一数据在同一个Worker内只需从
     * 外部存储中加载一次，提供了IsReady()，SetReady()，IsLoading()，
     * SetLoading()四个方法。如果一个或若干个Step获取到一个已创建好
     * 的Session，则需先调用IsReady()判断数据是否就绪，若就绪则直接
     * 从Session中读取，若未就绪则调用IsLoading()判断数据是否正在加
     * 载，若正在加载则直接return(CMD_STATUS_RUNNING)（框架会在数据
     * 加载完毕后调用该Step的Callback），否则加载数据并且SetLoading()，
     * 数据加载完毕后SetReady()。
     * @param pStep 调用IsReady()方法的调用者Step指针，用于记录
     * 哪些Step依赖于Session的数据，在数据就绪时由框架主动调用
     * 依赖这些数据的Step回调而不需要等到超时才回调。
     */
    bool IsReady(Step* pStep);

    void SetReady();
    bool IsLoading();
    void SetLoading(); 

protected:
    template <typename ...Targs> void Logger(int iLogLevel, const char* szFileName, unsigned int uiFileLine, const char* szFunction, Targs... args);
    template <typename ...Targs> std::shared_ptr<Step> MakeSharedStep(const std::string& strStepName, Targs... args);
    template <typename ...Targs> std::shared_ptr<Session> MakeSharedSession(const std::string& strSessionName, Targs... args);
    template <typename ...Targs> std::shared_ptr<Cmd> MakeSharedCmd(const std::string& strCmdName, Targs... args);
    template <typename ...Targs> std::shared_ptr<Module> MakeSharedModule(const std::string& strModuleName, Targs... args);

private:
    uint32 PopWaitingStep();

private:
    friend class WorkerImpl;
    bool m_bDataReady;
    bool m_bDataLoading;
    std::queue<uint32> m_vecWaitingStep;
};

template <typename ...Targs>
void Session::Logger(int iLogLevel, const char* szFileName, unsigned int uiFileLine, const char* szFunction, Targs... args)
{
    m_pWorker->Logger(m_strTraceId, iLogLevel, szFileName, uiFileLine, szFunction, std::forward<Targs>(args)...);
}

template <typename ...Targs>
std::shared_ptr<Step> Session::MakeSharedStep(const std::string& strStepName, Targs... args)
{
    return(m_pWorker->MakeSharedStep(this, strStepName, std::forward<Targs>(args)...));
}

template <typename ...Targs>
std::shared_ptr<Session> Session::MakeSharedSession(const std::string& strSessionName, Targs... args)
{
    return(m_pWorker->MakeSharedSession(this, strSessionName, std::forward<Targs>(args)...));
}

template <typename ...Targs>
std::shared_ptr<Cmd> Session::MakeSharedCmd(const std::string& strCmdName, Targs... args)
{
    return(m_pWorker->MakeSharedStep(this, strCmdName, std::forward<Targs>(args)...));
}

template <typename ...Targs>
std::shared_ptr<Module> Session::MakeSharedModule(const std::string& strModuleName, Targs... args)
{
    return(m_pWorker->MakeSharedSession(this, strModuleName, std::forward<Targs>(args)...));
}

} /* namespace neb */

#endif /* SRC_ACTOR_SESSION_SESSION_HPP_ */
