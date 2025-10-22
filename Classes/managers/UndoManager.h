#pragma once
#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "../models/UndoModel.h"
#include <vector>

/**
 * 撤销管理器
 * 处理撤销功能，管理操作历史记录
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
     * 初始化管理器
     */
    void setup()
    {
        reset();
    }

    /**
     * 添加撤销记录
     * @param record 撤销数据模型
     */
    void addUndoRecord(UndoModel* record)
    {
        if (record) {
            _history.push_back(record);
            CCLOG("UndoManager: Added undo record, history size: %d", _history.size());
        }
    }

    /**
     * 移除并返回最近的撤销记录
     * @return 撤销数据模型，如果没有记录返回nullptr
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
     * 检查是否有可撤销的操作
     * @return 是否有可撤销的操作
     */
    bool hasUndoableActions() const
    {
        bool result = !_history.empty();
        CCLOG("UndoManager: hasUndoableActions=%d, history size=%d", result, _history.size());
        return result;
    }

    /**
     * 清空所有撤销记录
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
    std::vector<UndoModel*> _history; // 操作历史记录
};

#endif // UNDO_MANAGER_H
