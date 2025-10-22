#pragma once
#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include "cocos2d.h"

USING_NS_CC;

static int g_cardIdentifier = 1;

/**
 * ��Ϸ������
 * �ṩͨ�õĹ��ߺ���
 */
class GameUtils {
public:
    /**
     * ����һ�����صĿ�Ƭ��ʶ��
     */
    static int createUniqueCardId()
    {
        return g_cardIdentifier++;
    }

    /**
     * �ж����ſ�Ƭ�ĵ����Ƿ����1
     */
    static bool areCardsAdjacent(int firstCardValue, int secondCardValue)
    {
        return abs(firstCardValue - secondCardValue) == 1;
    }

    /**
     * ����һ���ڸ�����Χ�ڵ��������
     */
    static int generateRandomInRange(int lowerBound, int upperBound)
    {
        return random(lowerBound, upperBound);
    }
};

#endif // GAME_UTILS_H
