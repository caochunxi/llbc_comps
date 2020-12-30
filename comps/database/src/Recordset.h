#ifndef _RECORDSET_H_
#define _RECORDSET_H_

#include "IDB.h"

#include <vector>
#include <memory>

class Record;

class Recordset : public IRecordset
{
public:
    Recordset(uint32_t recordNum);
    virtual ~Recordset() = default;

public:
    //�Ӽ��ϳ��ȡһ����¼(������Ȼ����Record�����ڴ�)
    virtual IRecord *GetRecord(uint32_t idx) override;

    //�Ӽ��ϴӻ�ȡһ����¼(���ϲ��ٹ���Record�����ڴ�)
    virtual IRecord *PopRecord(uint32_t idx) override;

    //��ü��ϼ�¼����
    virtual uint32_t CountRecord() const override;

    virtual IRecord &operator[](uint32_t idx) override;

public:
    void WriteRecord(uint32_t idx, IRecord *record);

private:
    uint32_t _recordNum;
    std::vector<std::unique_ptr<IRecord>> _records;
};

#endif  // _RECORDSET_H_