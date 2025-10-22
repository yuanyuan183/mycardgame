#pragma once
#ifndef CARD_VIEW_H
#define CARD_VIEW_H

#include "cocos2d.h"
#include "../models/CardModel.h"

USING_NS_CC;

/**
 * @brief 卡牌视图
 * 负责单张卡牌的显示和触摸事件处理
 */
class CardView : public cocos2d::Node
{
public:
    // 使用标准的cocos2d-x创建模式
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
     * 更新视图显示
     * @param cardModel 卡牌数据模型
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

        // 设置位置
        setPosition(cardModel->getLocation());

        // 移除旧的显示内容
        removeAllChildren();

        if (_isFaceUp) {
            displayCardFront(cardModel);  // 加载卡牌正面
        }
        else {
            displayCardBack();           // 加载卡牌背面
        }

        configureTouchHandling();
    }

    /**
     * 设置卡牌点击回调
     * @param callback 回调函数，参数为卡牌ID
     */
    void setTouchHandler(const std::function<void(int)>& callback) { _touchHandler = callback; }

    /**
     * 播放卡牌移动动画
     * @param targetPosition 目标位置
     * @param duration 动画时长
     * @param callback 动画完成后的回调
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
     * 播放卡牌匹配动画
     * @param callback 动画完成后的回调
     */
    void playMatchEffect(const std::function<void()>& callback = nullptr)
    {
        // 简单的缩放动画表示匹配
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
     * 设置卡牌是否可点击
     * @param enabled 是否可点击
     */
    void setTouchable(bool enabled)
    {
        _eventDispatcher->pauseEventListenersForTarget(this, !enabled);
        CCLOG("CardView: Touch %s for card %d", enabled ? "enabled" : "disabled", _id);
    }

    /**
     * 设置卡牌是否翻开
     * @param flipped 是否翻开
     */
    void setFaceUp(bool flipped) { _isFaceUp = flipped; }

private:
    // 加载卡牌背面
    void displayCardBack()
    {
        _sprite = Sprite::create("res/card_general.png");
        if (!_sprite) {
            _sprite = createFallbackCardSprite();
        }
        addChild(_sprite);
    }

    // 加载卡牌正面
    void displayCardFront(const CardModel* cardModel)
    {
        _sprite = Sprite::create("res/card_general.png");
        if (!_sprite) {
            _sprite = createFallbackCardSprite();
        }
        addChild(_sprite);

        // 加载数字和花色
        displayFaceAndSuit(cardModel);
    }

    // 加载卡牌数字和花色
    void displayFaceAndSuit(const CardModel* cardModel)
    {
        int value = cardModel->getCardValue();
        CardSuitType suit = cardModel->getSuitType();

        // 加载大数字
        displayBigNumber(value, suit);
        // 加载小数字
        displaySmallNumber(value, suit);
        // 加载花色
        displaySuitIcon(suit);
    }

    // 加载大数字
    void displayBigNumber(int value, CardSuitType suit)
    {
        std::string filename = getFaceFilename(value, suit, "big");
        _frontSprite = Sprite::create(filename);
        if (_frontSprite) {
            _frontSprite->setPosition(Vec2(_sprite->getContentSize().width / 2, _sprite->getContentSize().height / 2));
            _sprite->addChild(_frontSprite);
        }
    }

    // 加载小数字
    void displaySmallNumber(int value, CardSuitType suit)
    {
        std::string filename = getFaceFilename(value, suit, "small");
        auto smallSprite = Sprite::create(filename);
        if (smallSprite) {
            smallSprite->setPosition(Vec2(25, _sprite->getContentSize().height - 25));
            _sprite->addChild(smallSprite);
        }
    }

    // 加载花色图标
    void displaySuitIcon(CardSuitType suit)
    {
        std::string filename = getSuitFilename(suit);
        _iconSprite = Sprite::create(filename);
        if (_iconSprite) {
            _iconSprite->setPosition(Vec2(_sprite->getContentSize().width - 25, _sprite->getContentSize().height - 25));
            _sprite->addChild(_iconSprite);
        }
    }

    // 获取数字文件路径
    std::string getFaceFilename(int value, CardSuitType suit, const std::string& size)
    {
        std::string color = (suit == CardSuitType::CST_HEARTS || suit == CardSuitType::CST_DIAMONDS) ? "red" : "black";
        std::string name = (value == 1) ? "A" :
            (value == 11) ? "J" :
            (value == 12) ? "Q" :
            (value == 13) ? "K" : std::to_string(value);

        return "res/number/" + size + "_" + color + "_" + name + ".png";
    }

    // 获取花色图标文件路径
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

    // 创建默认卡牌精灵（用于缺失资源时的替代）
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

    // 触摸处理
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

    cocos2d::Sprite* _sprite; // 卡牌精灵
    cocos2d::Sprite* _frontSprite; // 数字精灵
    cocos2d::Sprite* _iconSprite; // 花色精灵
    std::function<void(int)> _touchHandler; // 点击回调
    int _id; // 卡牌ID
    bool _isFaceUp; // 是否翻开
};

#endif // CARD_VIEW_H
