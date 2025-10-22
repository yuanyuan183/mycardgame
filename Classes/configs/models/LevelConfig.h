#pragma once
#ifndef STAGE_SETTINGS_H
#define STAGE_SETTINGS_H

#include "cocos2d.h"
#include "../../models/CardModel.h"
#include <vector>
#include <mutex>

/**
 * 阶段配置数据模型
 * 存储阶段的静态配置数据
 */
class LevelConfig
{
public:
    // 卡片设置项
    struct CardItem
    {
        CardFaceType cardValue;    // 卡片的面值
        CardSuitType cardSuit;     // 卡片的花色
        cocos2d::Vec2 cardPosition; // 卡片的位置
    };

    // 获取 LevelConfig 的单例实例
    static LevelConfig& getInstance()
    {
        static LevelConfig instance; // 延迟初始化
        return instance;
    }

    // 获取和设置主要区域的卡片设置
    const std::vector<CardItem>& getMainAreaCards() const { return _mainAreaCards; }
    void setMainAreaCards(const std::vector<CardItem>& cards) { _mainAreaCards = cards; }

    // 获取和设置备用区域的卡片设置
    const std::vector<CardItem>& getBackupAreaCards() const { return _backupAreaCards; }
    void setBackupAreaCards(const std::vector<CardItem>& cards) { _backupAreaCards = cards; }

    // 删除拷贝构造函数和赋值操作符，确保单例不可复制
    LevelConfig(const LevelConfig&) = delete;
    LevelConfig& operator=(const LevelConfig&) = delete;

private:
    // 私有构造函数，确保外部无法直接创建实例
    LevelConfig() {}
    ~LevelConfig() = default;

    std::vector<CardItem> _mainAreaCards; // 主要区域卡片配置
    std::vector<CardItem> _backupAreaCards; // 备用区域卡片配置
};

#endif // STAGE_SETTINGS_H
