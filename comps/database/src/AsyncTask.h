#pragma once

#include "IDB.h"

enum class AsyncTaskType
{
    None,
    Query,       //��ѯ������¼
    ExecuteSql,  //ִ��sql
};

/**
 * �첽�����װ
 */
class AsyncTask
{
public:
    AsyncTask()
        : taskType(AsyncTaskType::None)
    {}
    virtual ~AsyncTask() = default;

    virtual void Invoke() = 0;

public:
    LLBC_String sql;
    AsyncTaskType taskType;
};

/**
 * ��ѯ������¼
 */
struct __TaskQueryRecordset
{
    MODE mode;
    IRecordset *result = nullptr;
    AsyncQueryCB cb;
};

/**
 * ִ��sql
 */
struct __TaskQuerySql
{
    bool result = false;
    AsyncSqlCB cb;
};

/**
* �첽����ģ��
*/
template <typename TaskInfo>
class AcyncTaskTemplate : public AsyncTask
{
public:
    virtual void Invoke() override
    {
        std::invoke(task.cb, task.result);
    }

public:
    TaskInfo task;
};

using TaskQuery = AcyncTaskTemplate<__TaskQueryRecordset>;
using TaskQuerySql = AcyncTaskTemplate<__TaskQuerySql>;

