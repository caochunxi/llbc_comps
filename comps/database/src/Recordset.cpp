#include "Recordset.h"

Recordset::Recordset(uint32_t recordNum)
    : _recordNum(recordNum)
{
    _records.resize(recordNum);
}

//�Ӽ��ϳ��ȡһ����¼(������Ȼ����Record�����ڴ�)
IRecord *Recordset::GetRecord(uint32_t idx)
{
    return _records[idx].get();
}

//�Ӽ��ϴӻ�ȡһ����¼(���ϲ��ٹ���Record�����ڴ�)
IRecord *Recordset::PopRecord(uint32_t idx)
{
    return _records[idx].release();
}

//��ü��ϼ�¼����
uint32_t Recordset::CountRecord() const
{
    return _recordNum;
}

IRecord &Recordset::operator[](uint32_t idx)
{
    return *(_records[idx]);
}

void Recordset::WriteRecord(uint32_t idx, IRecord *record)
{
    _records[idx].swap(std::unique_ptr<IRecord>(record));
}