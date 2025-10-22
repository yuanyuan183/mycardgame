#pragma once
#ifndef UNDO_MODEL_H
#define UNDO_MODEL_H

#include "cocos2d.h"
#include "CardModel.h"

/**
 * ������������ģ��
 * ��¼һ�β��������ڻ��˹���
 */
class UndoModel
{
    int _specialCardId;
    cocos2d::Vec2 _specialPosition;
public:
    UndoModel()
        : _operationType(ActionType::MATCH_CARD)
        , _matchedCardId(-1)
        , _previousBottomCardId(-1)
        , _newBottomCardId(-1)
        , _previousBottomPosition(cocos2d::Vec2::ZERO)
        , _matchedCardPosition(cocos2d::Vec2::ZERO)
    {
    }
    ~UndoModel() = default;

    // ��ȡ�����ò�������
    ActionType getOperationType() const { return _operationType; }
    void setOperationType(ActionType type) { _operationType = type; }

    // ���ڿ���ƥ���������¼��ƥ��������ƺ�֮ǰ�ĵ���
    int getMatchedCardId() const { return _matchedCardId; }
    void setMatchedCardId(int cardId) { _matchedCardId = cardId; }

    int getPreviousBottomCardId() const { return _previousBottomCardId; }
    void setPreviousBottomCardId(int cardId) { _previousBottomCardId = cardId; }

    // ���ڳ��Ʋ�������¼֮ǰ�ĵ���
    int getNewBottomCardId() const { return _newBottomCardId; }
    void setNewBottomCardId(int cardId) { _newBottomCardId = cardId; }

    // ��ȡ�����ò���ǰ�ĵ���λ��
    const cocos2d::Vec2& getPreviousBottomPosition() const { return _previousBottomPosition; }
    void setPreviousBottomPosition(const cocos2d::Vec2& position) { _previousBottomPosition = position; }

    // ��ȡ�����ò���ǰ��ƥ����λ��
    const cocos2d::Vec2& getMatchedCardPosition() const { return _matchedCardPosition; }
    void setMatchedCardPosition(const cocos2d::Vec2& position) { _matchedCardPosition = position; }

    int getSpecialCardId() const { return _specialCardId; }
    void setSpecialCardId(int cardId) { _specialCardId = cardId; }

    const cocos2d::Vec2& getSpecialPosition() const { return _specialPosition; }
    void setSpecialPosition(const cocos2d::Vec2& position) { _specialPosition = position; }

private:
    ActionType _operationType;           // ��������
    int _matchedCardId;                     // ƥ���������ID
    int _previousBottomCardId;              // ֮ǰ�ĵ���ID
    int _newBottomCardId;                   // �µĵ���ID����Գ��Ʋ�����
    cocos2d::Vec2 _previousBottomPosition;  // ����ǰ����λ��
    cocos2d::Vec2 _matchedCardPosition;     // ����ǰƥ����λ��
};

#endif // UNDO_MODEL_H