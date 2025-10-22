#pragma once
#ifndef CARD_VIEW_H
#define CARD_VIEW_H

#include "cocos2d.h"
#include "../models/CardModel.h"

USING_NS_CC;

/**
 * @brief ������ͼ
 * �����ſ��Ƶ���ʾ�ʹ����¼�����
 */
class CardView : public cocos2d::Node
{
public:
    // ʹ�ñ�׼��cocos2d-x����ģʽ
    static CardView* create()
    {
        CardView* pRet = new CardView();
        if (pRet && pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

    virtual bool init() override
    {
        if (!Node::init()) {
            return false;
        }
        _sprite = nullptr;
        _frontSprite = nullptr;
        _iconSprite = nullptr;
        _id = 0;
        _isFaceUp = false;

        return true;
    }

    /**
     * ������ͼ��ʾ
     * @param cardModel ��������ģ��
     */
    void update(const CardModel* cardModel)
    {
        if (!cardModel) {
            return;
        }

        CCLOG("CardView: Updating card %d with face %d and suit %d",
            cardModel->getItemId(),
            (int)cardModel->getFaceType(),
            (int)cardModel->getSuitType());

        _id = cardModel->getItemId();
        _isFaceUp = cardModel->isReversed();

        // ����λ��
        setPosition(cardModel->getLocation());

        // �Ƴ��ɵ���ʾ����
        removeAllChildren();

        if (_isFaceUp) {
            displayCardFront(cardModel);  // ���ؿ�������
        }
        else {
            displayCardBack();           // ���ؿ��Ʊ���
        }

        configureTouchHandling();
    }

    /**
     * ���ÿ��Ƶ���ص�
     * @param callback �ص�����������Ϊ����ID
     */
    void setTouchHandler(const std::function<void(int)>& callback) { _touchHandler = callback; }

    /**
     * ���ſ����ƶ�����
     * @param targetPosition Ŀ��λ��
     * @param duration ����ʱ��
     * @param callback ������ɺ�Ļص�
     */
    void playMovementEffect(const cocos2d::Vec2& targetPosition, float duration, const std::function<void()>& callback = nullptr)
    {
        auto move = MoveTo::create(duration, targetPosition);
        if (callback) {
            auto sequence = Sequence::create(move, CallFunc::create(callback), nullptr);
            runAction(sequence);
        }
        else {
            runAction(move);
        }
    }

    /**
     * ���ſ���ƥ�䶯��
     * @param callback ������ɺ�Ļص�
     */
    void playMatchEffect(const std::function<void()>& callback = nullptr)
    {
        // �򵥵����Ŷ�����ʾƥ��
        auto enlarge = ScaleTo::create(0.1f, 1.2f);
        auto shrink = ScaleTo::create(0.1f, 1.0f);
        if (callback) {
            auto sequence = Sequence::create(enlarge, shrink, CallFunc::create(callback), nullptr);
            runAction(sequence);
        }
        else {
            auto sequence = Sequence::create(enlarge, shrink, nullptr);
            runAction(sequence);
        }
    }

    /**
     * ���ÿ����Ƿ�ɵ��
     * @param enabled �Ƿ�ɵ��
     */
    void setTouchable(bool enabled)
    {
        _eventDispatcher->pauseEventListenersForTarget(this, !enabled);
        CCLOG("CardView: Touch %s for card %d", enabled ? "enabled" : "disabled", _id);
    }

    /**
     * ���ÿ����Ƿ񷭿�
     * @param flipped �Ƿ񷭿�
     */
    void setFaceUp(bool flipped) { _isFaceUp = flipped; }

private:
    // ���ؿ��Ʊ���
    void displayCardBack()
    {
        _sprite = Sprite::create("res/card_general.png");
        if (!_sprite) {
            _sprite = createFallbackCardSprite();
        }
        addChild(_sprite);
    }

    // ���ؿ�������
    void displayCardFront(const CardModel* cardModel)
    {
        _sprite = Sprite::create("res/card_general.png");
        if (!_sprite) {
            _sprite = createFallbackCardSprite();
        }
        addChild(_sprite);

        // �������ֺͻ�ɫ
        displayFaceAndSuit(cardModel);
    }

    // ���ؿ������ֺͻ�ɫ
    void displayFaceAndSuit(const CardModel* cardModel)
    {
        int value = cardModel->getCardValue();
        CardSuitType suit = cardModel->getSuitType();

        // ���ش�����
        displayBigNumber(value, suit);
        // ����С����
        displaySmallNumber(value, suit);
        // ���ػ�ɫ
        displaySuitIcon(suit);
    }

    // ���ش�����
    void displayBigNumber(int value, CardSuitType suit)
    {
        std::string filename = getFaceFilename(value, suit, "big");
        _frontSprite = Sprite::create(filename);
        if (_frontSprite) {
            _frontSprite->setPosition(Vec2(_sprite->getContentSize().width / 2, _sprite->getContentSize().height / 2));
            _sprite->addChild(_frontSprite);
        }
    }

    // ����С����
    void displaySmallNumber(int value, CardSuitType suit)
    {
        std::string filename = getFaceFilename(value, suit, "small");
        auto smallSprite = Sprite::create(filename);
        if (smallSprite) {
            smallSprite->setPosition(Vec2(25, _sprite->getContentSize().height - 25));
            _sprite->addChild(smallSprite);
        }
    }

    // ���ػ�ɫͼ��
    void displaySuitIcon(CardSuitType suit)
    {
        std::string filename = getSuitFilename(suit);
        _iconSprite = Sprite::create(filename);
        if (_iconSprite) {
            _iconSprite->setPosition(Vec2(_sprite->getContentSize().width - 25, _sprite->getContentSize().height - 25));
            _sprite->addChild(_iconSprite);
        }
    }

    // ��ȡ�����ļ�·��
    std::string getFaceFilename(int value, CardSuitType suit, const std::string& size)
    {
        std::string color = (suit == CardSuitType::CST_HEARTS || suit == CardSuitType::CST_DIAMONDS) ? "red" : "black";
        std::string name = (value == 1) ? "A" :
            (value == 11) ? "J" :
            (value == 12) ? "Q" :
            (value == 13) ? "K" : std::to_string(value);

        return "res/number/" + size + "_" + color + "_" + name + ".png";
    }

    // ��ȡ��ɫͼ���ļ�·��
    std::string getSuitFilename(CardSuitType suit)
    {
        switch (suit) {
        case CardSuitType::CST_CLUBS: return "res/suits/club.png";
        case CardSuitType::CST_DIAMONDS: return "res/suits/diamond.png";
        case CardSuitType::CST_HEARTS: return "res/suits/heart.png";
        case CardSuitType::CST_SPADES: return "res/suits/spade.png";
        default: return "";
        }
    }

    // ����Ĭ�Ͽ��ƾ��飨����ȱʧ��Դʱ�������
    cocos2d::Sprite* createFallbackCardSprite()
    {
        auto drawNode = DrawNode::create();
        Vec2 rectangle[4] = { Vec2(0, 0), Vec2(100, 0), Vec2(100, 150), Vec2(0, 150) };
        drawNode->drawPolygon(rectangle, 4, Color4F(0.3f, 0.3f, 0.5f, 1.0f), 1, Color4F(0.0f, 0.0f, 0.0f, 1.0f));
        auto sprite = Sprite::create();
        sprite->addChild(drawNode);
        sprite->setContentSize(Size(100, 150));
        return sprite;
    }

    // ��������
    void configureTouchHandling()
    {
        _eventDispatcher->removeEventListenersForTarget(this);

        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);

        listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
            if (!_sprite || !isVisible()) return false;
            Vec2 locationInNode = convertToNodeSpace(touch->getLocation());
            Size s = _sprite->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);
            if (rect.containsPoint(locationInNode)) {
                _sprite->setColor(Color3B(200, 200, 255));
                return true;
            }
            return false;
            };

        listener->onTouchEnded = [this](Touch* touch, Event* event) {
            _sprite->setColor(Color3B::WHITE);
            if (_touchHandler) {
                _touchHandler(_id);
            }
            };

        listener->onTouchCancelled = [this](Touch* touch, Event* event) {
            _sprite->setColor(Color3B::WHITE);
            };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }

    cocos2d::Sprite* _sprite; // ���ƾ���
    cocos2d::Sprite* _frontSprite; // ���־���
    cocos2d::Sprite* _iconSprite; // ��ɫ����
    std::function<void(int)> _touchHandler; // ����ص�
    int _id; // ����ID
    bool _isFaceUp; // �Ƿ񷭿�
};

#endif // CARD_VIEW_H
