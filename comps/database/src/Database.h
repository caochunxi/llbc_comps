#ifndef _Database_H_
#define _Database_H_

#include <mysql_version.h>
#include "mysql.h"
#include <mutex>
#include <vector>
#include <map>
#include <memory>

#include "IDB.h"
#include "AsyncTask.h"
#include "AsyncTaskQueue.h"

class MysqlConnect;
struct DBFieldInfo;


class MysqlDB : public IDatabase
{
public:
    MysqlDB();

    // ��ʼ��database
    virtual bool Init(const std::string &ip, int port, const std::string &user, const std::string &passwd, const std::string &dbName,
                      int acyncConnNum) override;
    // �������ݿ�����
    virtual void Destroy() override;

    // �ȴ���������ˢ��δ���������
    virtual void Flush() override;

    // ��ʱ���ã������첽����ص�
    virtual void OnUpdate() override;

public:
    // ͬ����ѯ������¼
    virtual IRecord *QueryRecord(const char *sql, MODE mode) override;

    // ͬ����ѯ������¼
    virtual IRecordset *QueryRecordset(const char *sql, MODE mode) override;

    // ͬ��ִ��sql
    virtual bool Query(const char *sql) override;

public:
    // �첽��ѯ������¼
    virtual void QueryRecordAsync(uint64_t key, const char *sql, MODE mode, DBAsyncRecordCB cb) override;

    // �첽��ѯ������¼
    virtual void QueryRecordsetAsync(uint64_t key, const char *sql, MODE mode, DBAsyncRecordsetCB cb) override;

    // �첽ִ��sql
    virtual void QueryAsync(uint64_t key, const char *sql, DBAsyncSqlCB cb) override;

public:
    //��������Ϣ
    const DBFieldInfo *QueryDBFieldInfo(const MYSQL_FIELD &field);

    // ͬ������ָ����Ĭ�ϼ�¼
    virtual IRecord *MakeDefRecord(const char *tableName) override;

    // ͬ�������¼
    virtual bool InsertRecord(IRecord *record) override;

    // ͬ�����¼�¼
    virtual bool UpdateRecord(IRecord *record) override;

    // ͬ��ɾ����¼
    virtual bool DeleteRecord(IRecord *record) override;

private:
    // ����첽����
    void AddAsyncTask(uint64_t key, AsyncTask *task);

    // �첽�̺߳���
    void AsyncWorkFunc(uint32_t threadIdx);

    // ִ���첽����
    void AsyncDoWork(MysqlConnect *conn, AsyncTask *task);

private:
    bool _inited;
    std::thread::id _asyncThreadId;
    uint32_t _asyncConnNum;
    
    std::atomic_bool _stoping; //�������
    std::atomic_bool _flushing; //ˢ�±��
    std::atomic_int _stopedNum; //��ֹͣ�첽�߳�����

    std::unique_ptr<MysqlConnect> _syncConn;
    std::vector<std::unique_ptr<MysqlConnect>> _asyncConns;
    std::vector<std::unique_ptr<std::thread>> _asyncThreads;
    std::vector<std::unique_ptr<AsyncTaskQueue>> _taskQueues;
    std::vector<std::unique_ptr<std::atomic_bool>> _flushFlag;

    std::mutex _finishLock;
    std::queue<AsyncTask *> _finishTasks;

    std::mutex _fieldLock;
    std::map<std::string, DBFieldInfo *> _fieldInfos;
};

#endif  // !_Database_H_
