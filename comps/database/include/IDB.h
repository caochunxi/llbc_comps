#ifndef _IDB_H_
#define _IDB_H_

#include <string>
#include <functional>

enum class MODE
{
    MODE_NONE,
    MODE_READ,  //ֻ��
    MODE_EDIT   //��д
};

class IRecord
{
public:
    virtual ~IRecord() = default;

    /**
     * ͨ������λ��������ȡֵ
     */
    virtual int64_t GetInt(uint32_t idx) const = 0;
    virtual double GetDouble(uint32_t idx) const = 0;
    virtual const std::string &GetStr(uint32_t idx) const = 0;
    /**
     * ͨ�����������ƻ�ȡֵ
     */
    virtual int64_t GetInt(const char *name) const = 0;
    virtual double GetDouble(const char *name) const = 0;
    virtual const std::string &GetStr(const char *name) const = 0;

    virtual void SetInt(uint32_t idx, int64_t val) = 0;
    virtual void SetDouble(uint32_t idx, double val) = 0;
    virtual void SetStr(uint32_t idx, const char *val) = 0;
    virtual void SetBlob(uint32_t nIdx, const char *val, uint32_t len) = 0;

    virtual void SetInt(const char *name, int64_t val) = 0;
    virtual void SetDouble(const char *name, double val) = 0;
    virtual void SetStr(const char *name, const char *val) = 0;
    virtual void SetBlob(const char *name, const char *val, uint32_t len) = 0;
};

/**
 * ��ѯ�������
 */
class IRecordset
{
public:
    virtual ~IRecordset() = default;

    //�Ӽ��ϳ��ȡһ����¼(������Ȼ����Record�����ڴ�)
    virtual IRecord *GetRecord(uint32_t idx) = 0;

    //�Ӽ��ϴӻ�ȡһ����¼(���ϲ��ٹ���Record�����ڴ�)
    virtual IRecord *PopRecord(uint32_t idx) = 0;

    //��ü��ϼ�¼����
    virtual uint32_t CountRecord() const = 0;

    virtual IRecord &operator[](uint32_t idx) = 0;
};

/**
 * �첽ִ��sql�ص�
 */
using DBAsyncRecordCB = std::function<void(IRecord *)>;
using DBAsyncRecordsetCB = std::function<void(IRecordset *)>;
using DBAsyncSqlCB = std::function<void(bool)>;

/**
 * ���ݿ����ӿ�
 */
class IDatabase
{
public:
    virtual ~IDatabase() = default;

    // ��ʼ��database
    virtual bool Init(const std::string &ip, int port, const std::string &user, const std::string &passwd, const std::string &dbName,
                      int asyncConnNum) = 0;
    // �������ݿ�����
    virtual void Destroy() = 0;

    // �ȴ���������ˢ��δ���������
    virtual void Flush() = 0;

    // ��ʱ���ã������첽����ص�
    virtual void OnUpdate() = 0;

public:
    // ͬ����ѯ������¼
    virtual IRecord *QueryRecord(const char *sql, MODE mode) = 0;

    // ͬ����ѯ������¼
    virtual IRecordset *QueryRecordset(const char *sql, MODE mode) = 0;

    // ͬ��ִ��sql
    virtual bool Query(const char *sql) = 0;

public:
    // �첽��ѯ������¼
    virtual void QueryRecordAsync(uint64_t key, const char *sql, MODE mode, DBAsyncRecordCB cb) = 0;

    // �첽��ѯ������¼
    virtual void QueryRecordsetAsync(uint64_t key, const char *sql, MODE mode, DBAsyncRecordsetCB cb) = 0;

    // �첽ִ��sql
    virtual void QueryAsync(uint64_t key, const char *sql, DBAsyncSqlCB cb) = 0;

public:
    // ͬ������ָ����Ĭ�ϼ�¼
    virtual IRecord *MakeDefRecord(const char *tableName) = 0;

    // ͬ�������¼
    virtual bool InsertRecord(IRecord *record) = 0;

    // ͬ�����¼�¼
    virtual bool UpdateRecord(IRecord *record) = 0;

    // ͬ��ɾ����¼
    virtual bool DeleteRecord(IRecord *record) = 0;
};

extern "C"  IDatabase *CreateDB(const std::string &ip, int port, const std::string &user, const std::string &passwd,
                                           const std::string &dbName,
                                int asyncConnNum);

#endif
