#pragma once

#include "IDB.h"

#include <vector>
#include <memory>

class Record;

class Recordset : public IRecordset
{
public:
    Recordset(uint32 recordNum);
    virtual ~Recordset() = default;

public:
    //�Ӽ��ϳ��ȡһ����¼(������Ȼ����Record�����ڴ�)
    virtual IRecord *GetRecord(uint32 idx) override;

    //�Ӽ��ϴӻ�ȡһ����¼(���ϲ��ٹ���Record�����ڴ�)
    virtual IRecord *PopRecord(uint32 idx) override;

    //��ü��ϼ�¼����
    virtual uint32 GetSize() const override;

    virtual IRecord &operator[](uint32 idx) override;

public:
    void WriteRecord(uint32 idx, IRecord *record);

private:
    uint32 _recordNum;
    std::vector<std::unique_ptr<IRecord>> _records;
};

