#ifndef _ASYNC_TASK_H_
#define _ASYNC_TASK_H_

#include "IDB.h"

enum class AsyncTaskType
{
    None,
    QueryRecord,     //��ѯ������¼
    QueryRecordset,  //��ѯ������¼
    QuerySql,        //ִ��sql
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
    std::string sql;
    AsyncTaskType taskType;
};

/**
* ��ѯ������¼
*/
struct __TaskQueryRecord
{
    MODE mode;
    IRecord *result = nullptr;
    DBAsyncRecordCB cb;
};

/**
 * ��ѯ������¼
 */
struct __TaskQueryRecordset
{
    MODE mode;
    IRecordset *result = nullptr;
    DBAsyncRecordsetCB cb;
};

/**
 * ִ��sql
 */
struct __TaskQuerySql
{
    bool result = false;
    DBAsyncSqlCB cb;
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

using TaskQueryRecord = AcyncTaskTemplate<__TaskQueryRecord>;
using TaskQueryRecordset = AcyncTaskTemplate<__TaskQueryRecordset>;
using TaskQuerySql = AcyncTaskTemplate<__TaskQuerySql>;

#endif  // !_ASYNC_TASK_H_
