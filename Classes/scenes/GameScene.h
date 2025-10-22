#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "cocos2d.h"
#include "../controllers/GameController.h"
#include "ui/CocosGUI.h"

using namespace ui;

/**
 * Main game scene
 * Contains the game controller and view
 */
class GameScene : public cocos2d::Scene
{
public:
    static GameScene* create()
    {
        GameScene* instance = new GameScene();
        if (instance && instance->init()) {
            instance->autorelease();
            return instance;
        }
        else {
            delete instance;
            instance = nullptr;
            return nullptr;
        }
    }

    virtual bool init() override
    {
        if (!Scene::init()) {
            return false;
        }

        setupSceneBackground();
        initializeController();
        createResetButton();

        return true;
    }

private:
    void setupSceneBackground()
    {
        auto sceneBg = LayerColor::create(Color4B(0, 100, 0, 255)); // Set background color
        addChild(sceneBg, -1);
    }

    void initializeController()
    {
        _controller = new GameController();
        _controller->beginGame(1);

        if (_controller->getGameView()) {
            addChild(_controller->getGameView());
        }
    }

    void createResetButton()
    {
        auto resetBtn = Button::create("");
        resetBtn->setTitleText("back");
        resetBtn->setTitleFontSize(60);
        resetBtn->setTitleColor(Color3B::WHITE);
        resetBtn->setColor(Color3B(255, 200, 0));
        resetBtn->setContentSize(Size(220, 90));
        resetBtn->setOpacity(230);
        resetBtn->setPosition(Vec2(260, 300));

        resetBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
            handleResetButtonEvent(sender, type);
            });
        addChild(resetBtn);
    }

    void handleResetButtonEvent(Ref* sender, ui::Widget::TouchEventType type)
    {
        auto button = static_cast<ui::Button*>(sender);
        switch (type) {
        case ui::Widget::TouchEventType::BEGAN:
            button->setScale(0.95f);
            button->setColor(Color3B(255, 180, 0));
            break;
        case ui::Widget::TouchEventType::ENDED:
            button->setScale(1.0f);
            button->setColor(Color3B(255, 200, 0));
            this->onResetButtonClicked();
            break;
        case ui::Widget::TouchEventType::CANCELED:
            button->setScale(1.0f);
            button->setColor(Color3B(255, 200, 0));
            break;
        default:
            break;
        }
    }

    void onResetButtonClicked()
    {
        if (_controller) {
            _controller->handleUndo();
        }
    }

    GameController* _controller; // Game controller
};

#endif // GAME_SCENE_H
