#pragma once
#ifndef DECK_ITEM_H
#define DECK_ITEM_H

#include "cocos2d.h"

// ��ɫ����
enum CardSuitType
{
    CST_NONE = -1,
    CST_CLUBS,      // ÷��
    CST_DIAMONDS,   // ����
    CST_HEARTS,     // ����
    CST_SPADES,     // ����
    CST_NUM_CARD_SUIT_TYPES
};

// ��������
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

// ��Ϸ�������
enum class AreaType
{
    MAIN_FIELD, // ����Ϸ����
    BOTTOM,     // �ײ�����
    PILE        // �ѵ�����
};

// �������
enum class ActionType
{
    MATCH_CARD,     // ����ƥ��
    DRAW_CARD,      // �鿨
    ADD_NEW_CARD,   // ��������
    SPECIAL_MOVE    // �����ƶ�
};

// ����״̬�ӿ�
class ICardStatus
{
public:
    virtual ~ICardStatus() = default;

    // ��ת״̬
    virtual bool isReversed() const = 0;
    virtual void toggle() = 0;

    // ����λ��
    virtual cocos2d::Vec2 getLoc() const = 0;
    virtual void setLoc(const cocos2d::Vec2& location) = 0;
};

// ����Ŀ��Ʒ�ת״̬
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

// ����Ŀ���δ��ת״̬
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

// ����������
class CardModel
{
public:
    CardModel()
        : _suit(CardSuitType::CST_NONE), _face(CardFaceType::CFT_NONE), _itemId(0), _status(new UnreversedStatus())
    {
    }

    ~CardModel() { delete _status; }

    // ��ȡ�����ÿ��ƻ�ɫ
    CardSuitType getSuitType() const { return _suit; }
    void setSuitType(CardSuitType suit) { _suit = suit; }

    // ��ȡ�����ÿ�����ֵ
    CardFaceType getFaceType() const { return _face; }
    void setFaceType(CardFaceType face) { _face = face; }

    // ��ȡ�����ÿ���Ψһ��ʶ
    int getItemId() const { return _itemId; }
    void setItemId(int itemId) { _itemId = itemId; }

    // ��ȡ���Ƶ�����A=1, J=11, Q=12, K=13��
    int getCardValue() const
    {
        return static_cast<int>(_face) + 1; // ACE=0+1=1, TWO=1+1=2, ..., KING=12+1=13
    }

    // ��ת����
    void setReversed(bool reversed)
    {
        if (_status->isReversed() != reversed)
            _status->toggle();
    }

    // ��ȡ���Ʒ�ת״̬
    bool isReversed() const { return _status->isReversed(); }

    // ��ȡ�����ÿ���λ��
    cocos2d::Vec2 getLocation() const { return _status->getLoc(); }
    void setLocation(const cocos2d::Vec2& location) { _status->setLoc(location); }

private:
    CardSuitType _suit;        // ��ɫ
    CardFaceType _face;        // ��ֵ
    int _itemId;           // ����ΨһID
    ICardStatus* _status;  // ��ǰ����״̬����ת��δ��ת��
};

#endif // DECK_ITEM_H
