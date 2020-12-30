#include "MysqlConnect.h"
#include "Database.h"
#include "Record.h"
#include "Recordset.h"
#include "FieldFactory.h"

#include "llbc/core/thread/Guard.h"

MysqlConnect::MysqlConnect(MysqlDB &db, const std::string &ip, int port, const std::string &user, const std::string &passwd,
                           const std::string &dbName)
    : _db(db)
    , _ip(ip)
    , _port(port)
    , _user(user)
    , _pwd(passwd)
    , _dbName(dbName)
    , _lastErrorCode(0)
{}

MysqlConnect::~MysqlConnect()
{
    Disconnect();
}

bool MysqlConnect::Connect()
{
    MYSQL *hdbc = mysql_init(nullptr);
    if (!hdbc)
        return false;

    if (!mysql_real_connect(hdbc, _ip.c_str(), _user.c_str(), _pwd.c_str(), _dbName.c_str(), _port, nullptr, 0))
    {
        this->SetError();
        mysql_close(hdbc);
        return false;
    }

    char value = 1;
    mysql_options(hdbc, MYSQL_OPT_RECONNECT, (char *) &value);
    mysql_options(hdbc, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    _opened = true;
    _dbHandler = hdbc;
    return true;
}

bool MysqlConnect::Ping()
{
    if (!_dbHandler)
        return false;

    if (mysql_ping(_dbHandler))
        return false;

    return true;
}

bool MysqlConnect::IsConnect()
{
    return _opened;
}

void MysqlConnect::Disconnect()
{
    if (_opened && _dbHandler)
    {
        mysql_close(_dbHandler);
        _opened = false;
    }
}

MYSQL *MysqlConnect::GetHandler()
{
    return _dbHandler;
}

uint32_t MysqlConnect::GetLastErrorNo()
{
    return _lastErrorCode;
}

const std::string MysqlConnect::GetLastError()
{
    return _lastError;
}

Record *MysqlConnect::CreateRecord(MYSQL_ROW row, MYSQL_FIELD *dbFields, const unsigned long *colLens, uint32_t fieldNum, MODE mode)
{
    std::unique_ptr<Record> newRec(new Record(fieldNum, std::string(dbFields[0].org_table), mode));

    for (uint32_t i = 0; i < fieldNum; i++)
    {
        const MYSQL_FIELD &dbField = dbFields[i];
        const DBFieldInfo *fieldInfo = _db.QueryDBFieldInfo(dbField);

        if (fieldInfo == nullptr)
            return nullptr;

        if (dbField.flags & PRI_KEY_FLAG)
            newRec->SetKeyIdx(i);

        if (dbField.flags & AUTO_INCREMENT_FLAG)
            newRec->SetAutoIncIdx(i);

        BaseField *baseField = FieldFactory::CreateField(*fieldInfo);
        if (baseField == nullptr)
            return nullptr;

        baseField->SetValue(row ? row[i] : dbField.def, colLens ? colLens[i] : dbField.length);
        newRec->WriteField(i, baseField);
    }

    return newRec.release();
}

// ͬ����ѯ������¼
IRecord *MysqlConnect::QueryRecord(const char *sql, MODE mode)
{
    MYSQL_RES *res = nullptr;
    if (!Query(sql, &res))
        return nullptr;

    if (!res)
        return nullptr;

    //���������Զ��ͷ�MYSQL_RES
    std::shared_ptr<void> _resGuard((void *) 0, [res](void *) { mysql_free_result(res); });
    // ȡ��1����¼
    mysql_data_seek(res, 0);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (!row)
        return nullptr;

    // ȡ�ֶ���Ϣ
    MYSQL_FIELD *dbFields = mysql_fetch_fields(res);
    if (!dbFields)
        return nullptr;

    const unsigned long *colLens = mysql_fetch_lengths(res);
    const uint32_t fieldNum = mysql_num_fields(res);

    return CreateRecord(row, dbFields, colLens, fieldNum, mode);
}

// ͬ����ѯ������¼
IRecordset *MysqlConnect::QueryRecordset(const char *sql, MODE mode)
{
    MYSQL_RES *res = nullptr;
    if (!Query(sql, &res))
        return nullptr;

    if (!res)
        return nullptr;

    //���������Զ��ͷ�MYSQL_RES
    std::shared_ptr<void> _resGuard((void *) 0, [res](void *) { mysql_free_result(res); });
    mysql_data_seek(res, 0);

    // ȡ�ֶ���Ϣ
    MYSQL_FIELD *dbFields = mysql_fetch_fields(res);
    if (!dbFields)
        return nullptr;

    const unsigned long *colLens = mysql_fetch_lengths(res);
    const uint32_t fieldNum = mysql_num_fields(res);

    std::unique_ptr<Recordset> recSet = std::make_unique<Recordset>(static_cast<uint32_t>(mysql_num_rows(res)));

    MYSQL_ROW row;
    uint32_t idx = 0;
    while (row = mysql_fetch_row(res))
    {
        Record *newRec = CreateRecord(row, dbFields, colLens, fieldNum, mode);
        if (!newRec)
            return nullptr;

        recSet->WriteRecord(idx++, newRec);
    }

    assert(idx == recSet->CountRecord());
    return recSet.release();
}

// ͬ��ִ��sql
bool MysqlConnect::Query(const char *sql)
{
    return Query(sql, nullptr);
}

void MysqlConnect::SetError()
{
    _lastErrorCode = mysql_errno(_dbHandler);
    _lastError = mysql_error(_dbHandler);
}

bool MysqlConnect::Query(const char *sql, MYSQL_RES **res)
{
    if (!IsConnect())
        return false;

    try
    {
        int ret = mysql_real_query(_dbHandler, sql, (int) strlen(sql));
        if (ret != 0)
        {
            this->SetError();
            return false;
        }

        if (res)
            *res = mysql_store_result(_dbHandler);
    }
    catch (const std::exception &)
    {
        this->SetError();
        return false;
    }
    return true;
}

// ����ָ����Ĭ�ϼ�¼
IRecord *MysqlConnect::MakeDefRecord(const char *tableName)
{
    char sql[256] = { 0 };
    snprintf(sql, sizeof(sql) - 1, "select * from %s limit 0", tableName);

    MYSQL_RES *res = nullptr;
    if (!Query(sql, &res))
        return nullptr;

    if (!res)
        return nullptr;

    //���������Զ��ͷ�MYSQL_RES
    std::shared_ptr<void> _resGuard((void *) 0, [res](void *) { mysql_free_result(res); });

    // ȡ�ֶ���Ϣ
    MYSQL_FIELD *dbFields = mysql_fetch_fields(res);
    if (!dbFields)
        return nullptr;

    const unsigned long *colLens = mysql_fetch_lengths(res);
    const uint32_t fieldNum = mysql_num_fields(res);

    return CreateRecord(nullptr, dbFields, colLens, fieldNum, MODE::MODE_EDIT);
}