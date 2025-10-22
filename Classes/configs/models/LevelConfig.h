#pragma once
#ifndef STAGE_SETTINGS_H
#define STAGE_SETTINGS_H

#include "cocos2d.h"
#include "../../models/CardModel.h"
#include <vector>
#include <mutex>

/**
 * �׶���������ģ��
 * �洢�׶εľ�̬��������
 */
class LevelConfig
{
public:
    // ��Ƭ������
    struct CardItem
    {
        CardFaceType cardValue;    // ��Ƭ����ֵ
        CardSuitType cardSuit;     // ��Ƭ�Ļ�ɫ
        cocos2d::Vec2 cardPosition; // ��Ƭ��λ��
    };

    // ��ȡ LevelConfig �ĵ���ʵ��
    static LevelConfig& getInstance()
    {
        static LevelConfig instance; // �ӳٳ�ʼ��
        return instance;
    }

    // ��ȡ��������Ҫ����Ŀ�Ƭ����
    const std::vector<CardItem>& getMainAreaCards() const { return _mainAreaCards; }
    void setMainAreaCards(const std::vector<CardItem>& cards) { _mainAreaCards = cards; }

    // ��ȡ�����ñ�������Ŀ�Ƭ����
    const std::vector<CardItem>& getBackupAreaCards() const { return _backupAreaCards; }
    void setBackupAreaCards(const std::vector<CardItem>& cards) { _backupAreaCards = cards; }

    // ɾ���������캯���͸�ֵ��������ȷ���������ɸ���
    LevelConfig(const LevelConfig&) = delete;
    LevelConfig& operator=(const LevelConfig&) = delete;

private:
    // ˽�й��캯����ȷ���ⲿ�޷�ֱ�Ӵ���ʵ��
    LevelConfig() {}
    ~LevelConfig() = default;

    std::vector<CardItem> _mainAreaCards; // ��Ҫ����Ƭ����
    std::vector<CardItem> _backupAreaCards; // ��������Ƭ����
};

#endif // STAGE_SETTINGS_H
