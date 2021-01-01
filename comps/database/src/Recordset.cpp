#include "Recordset.h"

Recordset::Recordset(uint32 recordNum)
    : _recordNum(recordNum)
{
    _records.resize(recordNum);
}

//�Ӽ��ϳ��ȡһ����¼(������Ȼ����Record�����ڴ�)
IRecord *Recordset::GetRecord(uint32 idx)
{
    return _records[idx].get();
}

//�Ӽ��ϴӻ�ȡһ����¼(���ϲ��ٹ���Record�����ڴ�)
IRecord *Recordset::PopRecord(uint32 idx)
{
    return _records[idx].release();
}

//��ü��ϼ�¼����
uint32 Recordset::GetSize() const
{
    return _recordNum;
}

IRecord &Recordset::operator[](uint32 idx)
{
    return *(_records[idx]);
}

void Recordset::WriteRecord(uint32 idx, IRecord *record)
{
    _records[idx].swap(std::unique_ptr<IRecord>(record));
}