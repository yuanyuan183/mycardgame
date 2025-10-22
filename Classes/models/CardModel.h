#pragma once
#ifndef DECK_ITEM_H
#define DECK_ITEM_H

#include "cocos2d.h"

// 花色类型
enum CardSuitType
{
    CST_NONE = -1,
    CST_CLUBS,      // 梅花
    CST_DIAMONDS,   // 方块
    CST_HEARTS,     // 红桃
    CST_SPADES,     // 黑桃
    CST_NUM_CARD_SUIT_TYPES
};

// 正面类型
enum CardFaceType
{
    CFT_NONE = -1,
    CFT_ACE,
    CFT_TWO,
    CFT_THREE,
    CFT_FOUR,
    CFT_FIVE,
    CFT_SIX,
    CFT_SEVEN,
    CFT_EIGHT,
    CFT_NINE,
    CFT_TEN,
    CFT_JACK,
    CFT_QUEEN,
    CFT_KING,
    CFT_NUM_CARD_FACE_TYPES
};

// 游戏区域分类
enum class AreaType
{
    MAIN_FIELD, // 主游戏区域
    BOTTOM,     // 底部区域
    PILE        // 堆叠区域
};

// 操作类别
enum class ActionType
{
    MATCH_CARD,     // 卡牌匹配
    DRAW_CARD,      // 抽卡
    ADD_NEW_CARD,   // 新增卡牌
    SPECIAL_MOVE    // 特殊移动
};

// 卡牌状态接口
class ICardStatus
{
public:
    virtual ~ICardStatus() = default;

    // 翻转状态
    virtual bool isReversed() const = 0;
    virtual void toggle() = 0;

    // 卡牌位置
    virtual cocos2d::Vec2 getLoc() const = 0;
    virtual void setLoc(const cocos2d::Vec2& location) = 0;
};

// 具体的卡牌翻转状态
class ReversedStatus : public ICardStatus
{
public:
    ReversedStatus() : _reversed(true), _location(cocos2d::Vec2::ZERO) {}

    bool isReversed() const override { return _reversed; }
    void toggle() override { _reversed = !_reversed; }
    cocos2d::Vec2 getLoc() const override { return _location; }
    void setLoc(const cocos2d::Vec2& location) override { _location = location; }

private:
    bool _reversed;
    cocos2d::Vec2 _location;
};

// 具体的卡牌未翻转状态
class UnreversedStatus : public ICardStatus
{
public:
    UnreversedStatus() : _reversed(false), _location(cocos2d::Vec2::ZERO) {}

    bool isReversed() const override { return _reversed; }
    void toggle() override { _reversed = !_reversed; }
    cocos2d::Vec2 getLoc() const override { return _location; }
    void setLoc(const cocos2d::Vec2& location) override { _location = location; }

private:
    bool _reversed;
    cocos2d::Vec2 _location;
};

// 卡牌数据类
class CardModel
{
public:
    CardModel()
        : _suit(CardSuitType::CST_NONE), _face(CardFaceType::CFT_NONE), _itemId(0), _status(new UnreversedStatus())
    {
    }

    ~CardModel() { delete _status; }

    // 获取和设置卡牌花色
    CardSuitType getSuitType() const { return _suit; }
    void setSuitType(CardSuitType suit) { _suit = suit; }

    // 获取和设置卡牌面值
    CardFaceType getFaceType() const { return _face; }
    void setFaceType(CardFaceType face) { _face = face; }

    // 获取和设置卡牌唯一标识
    int getItemId() const { return _itemId; }
    void setItemId(int itemId) { _itemId = itemId; }

    // 获取卡牌点数（A=1, J=11, Q=12, K=13）
    int getCardValue() const
    {
        return static_cast<int>(_face) + 1; // ACE=0+1=1, TWO=1+1=2, ..., KING=12+1=13
    }

    // 翻转卡牌
    void setReversed(bool reversed)
    {
        if (_status->isReversed() != reversed)
            _status->toggle();
    }

    // 获取卡牌翻转状态
    bool isReversed() const { return _status->isReversed(); }

    // 获取和设置卡牌位置
    cocos2d::Vec2 getLocation() const { return _status->getLoc(); }
    void setLocation(const cocos2d::Vec2& location) { _status->setLoc(location); }

private:
    CardSuitType _suit;        // 花色
    CardFaceType _face;        // 面值
    int _itemId;           // 卡牌唯一ID
    ICardStatus* _status;  // 当前卡牌状态（翻转或未翻转）
};

#endif // DECK_ITEM_H
