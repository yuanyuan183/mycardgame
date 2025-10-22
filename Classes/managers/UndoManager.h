#pragma once
#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "../models/UndoModel.h"
#include <vector>

/**
 * ����������
 * ���������ܣ����������ʷ��¼
 */
class UndoManager
{
public:
    UndoManager() {}
    ~UndoManager()
    {
        reset();
    }

    /**
     * ��ʼ��������
     */
    void setup()
    {
        reset();
    }

    /**
     * ��ӳ�����¼
     * @param record ��������ģ��
     */
    void addUndoRecord(UndoModel* record)
    {
        if (record) {
            _history.push_back(record);
            CCLOG("UndoManager: Added undo record, history size: %d", _history.size());
        }
    }

    /**
     * �Ƴ�����������ĳ�����¼
     * @return ��������ģ�ͣ����û�м�¼����nullptr
     */
    UndoModel* removeUndoRecord()
    {
        if (_history.empty()) {
            CCLOG("UndoManager: No records to undo");
            return nullptr;
        }

        UndoModel* record = _history.back();
        _history.pop_back();
        CCLOG("UndoManager: Removed undo record, history size: %d", _history.size());
        return record;
    }

    /**
     * ����Ƿ��пɳ����Ĳ���
     * @return �Ƿ��пɳ����Ĳ���
     */
    bool hasUndoableActions() const
    {
        bool result = !_history.empty();
        CCLOG("UndoManager: hasUndoableActions=%d, history size=%d", result, _history.size());
        return result;
    }

    /**
     * ������г�����¼
     */
    void reset()
    {
        for (std::vector<UndoModel*>::iterator it = _history.begin(); it != _history.end(); ++it) {
            delete* it;
        }
        _history.clear();
        CCLOG("UndoManager: All undo records cleared");
    }

private:
    std::vector<UndoModel*> _history; // ������ʷ��¼
};

#endif // UNDO_MANAGER_H
