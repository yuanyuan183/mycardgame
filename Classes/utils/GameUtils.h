#pragma once
#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include "cocos2d.h"

USING_NS_CC;

static int g_cardIdentifier = 1;

/**
 * 游戏工具类
 * 提供通用的工具函数
 */
class GameUtils {
public:
    /**
     * 生成一个独特的卡片标识符
     */
    static int createUniqueCardId()
    {
        return g_cardIdentifier++;
    }

    /**
     * 判断两张卡片的点数是否相差1
     */
    static bool areCardsAdjacent(int firstCardValue, int secondCardValue)
    {
        return abs(firstCardValue - secondCardValue) == 1;
    }

    /**
     * 生成一个在给定范围内的随机整数
     */
    static int generateRandomInRange(int lowerBound, int upperBound)
    {
        return random(lowerBound, upperBound);
    }
};

#endif // GAME_UTILS_H
