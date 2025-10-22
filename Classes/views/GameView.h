#pragma once
#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "cocos2d.h"
#include "CardView.h"

// Forward declaration
class GameModel;

/**
 * Game UI view
 * Responsible for rendering the entire UI and handling user interactions
 */
class GameView : public cocos2d::Node
{
public:
    // Standard cocos2d-x object creation pattern
    static GameView* create()
    {
        GameView* instance = new GameView();
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
        if (!Node::init()) {
            return false;
        }

        initializeUI();
        return true;
    }

    /**
     * Updates the UI based on the current game model
     * @param model Game data model
     */
    void updateDisplay(GameModel* model)
    {
        if (!model) return;

        CCLOG("GameView: Updating display");

        // Gather nodes to remove (removing card views)
        std::vector<Node*> nodesToDelete;

        for (auto child : _mainAreaNode->getChildren()) {
            if (dynamic_cast<CardView*>(child)) {
                nodesToDelete.push_back(child);
            }
        }

        for (auto child : _footerNode->getChildren()) {
            if (dynamic_cast<CardView*>(child)) {
                nodesToDelete.push_back(child);
            }
        }

        for (auto child : _reserveNode->getChildren()) {
            if (dynamic_cast<CardView*>(child)) {
                nodesToDelete.push_back(child);
            }
        }

        // Remove all collected card views
        for (auto node : nodesToDelete) {
            node->removeFromParent();
        }

        // Clear current card view mapping
        _cardViewMap.clear();

        // Create cards for the main play field
        const auto& fieldCards = model->getFieldCards();
        for (auto card : fieldCards) {
            if (card) {
                CardView* cardDisplay = CardView::create();
                if (cardDisplay) {
                    cardDisplay->update(card);
                    cardDisplay->setTouchHandler(_cardTouchCallback);
                    _cardViewMap[card->getItemId()] = cardDisplay;
                    _mainAreaNode->addChild(cardDisplay);
                }
            }
        }

        // Create the active card at the bottom of the screen
        CardModel* activeCard = model->getActiveCard();
        if (activeCard) {
            CardView* cardDisplay = CardView::create();
            if (cardDisplay) {
                activeCard->setReversed(true);
                cardDisplay->update(activeCard);
                cardDisplay->setScale(1.05f);
                _cardViewMap[activeCard->getItemId()] = cardDisplay;
                _footerNode->addChild(cardDisplay);
            }
        }

        // Display the top card of the reserve stack
        const auto& reserveCards = model->getReserveCards();
        if (!reserveCards.empty()) {
            CardModel* topReserveCard = reserveCards.back();
            if (topReserveCard) {
                CardView* cardDisplay = CardView::create();
                if (cardDisplay) {
                    topReserveCard->setReversed(false);
                    cardDisplay->update(topReserveCard);
                    _cardViewMap[topReserveCard->getItemId()] = cardDisplay;
                    _reserveNode->addChild(cardDisplay);
                }
            }
        }
    }

    /**
     * Set the card click callback
     * @param callback Function to handle card clicks, accepts card ID as an argument
     */
    void setCardClickCallback(const std::function<void(int)>& callback) {
        _cardTouchCallback = callback;
    }

    /**
     * Set the reserve stack click callback
     * @param callback Function to handle stack clicks
     */
    void setReserveClickCallback(const std::function<void()>& callback) {
        _reserveClickCallback = callback;
    }

    /**
     * Play the animation for moving a card to a target position
     * @param cardId ID of the card to move
     * @param targetPosition Target position for the card
     * @param duration Duration of the animation
     * @param callback Callback to call after the animation finishes
     */
    void animateCardMovement(int cardId, const cocos2d::Vec2& targetPosition,
        float duration, const std::function<void()>& callback = nullptr)
    {
        CardView* cardDisplay = getCardView(cardId);
        if (cardDisplay) {
            cardDisplay->playMovementEffect(targetPosition, duration, callback);
        }
        else if (callback) {
            callback();
        }
    }

    /**
     * Retrieve the card view for a given card ID
     * @param cardId ID of the card
     * @return CardView pointer corresponding to the card ID
     */
    CardView* getCardView(int cardId) const
    {
        auto it = _cardViewMap.find(cardId);
        return (it != _cardViewMap.end()) ? it->second : nullptr;
    }

    /**
     * Play the match effect animation for a card
     * @param cardId ID of the card to animate
     * @param callback Callback to call after the animation finishes
     */
    void triggerMatchAnimation(int cardId, const std::function<void()>& callback = nullptr)
    {
        CardView* cardDisplay = getCardView(cardId);
        if (cardDisplay) {
            cardDisplay->playMatchEffect(callback);
        }
        else if (callback) {
            callback();
        }
    }

    /**
     * Get the position of the footer node (used for undo functionality)
     * @return Position of the footer node
     */
    cocos2d::Vec2 getFooterNodePosition() const { return _footerNode->getPosition(); }

    /**
     * Get the position of the reserve node (used for undo functionality)
     * @return Position of the reserve node
     */
    cocos2d::Vec2 getReserveNodePosition() const { return _reserveNode->getPosition(); }

    // Getters for nodes
    cocos2d::Node* getMainAreaNode() const { return _mainAreaNode; }
    cocos2d::Node* getFooterNode() const { return _footerNode; }
    cocos2d::Node* getReserveNode() const { return _reserveNode; }

private:
    void initializeUI()
    {
        // Initialize main area, footer, and reserve nodes
        _mainAreaNode = Node::create();
        _footerNode = Node::create();
        _reserveNode = Node::create();

        // Set up backgrounds for the nodes
        auto mainAreaBackground = LayerColor::create(Color4B(204, 153, 51, 255), 1080, 1500);
        _mainAreaNode->addChild(mainAreaBackground, -1);

        auto footerBackground = LayerColor::create(Color4B(128, 0, 128, 255), 1080, 580);
        footerBackground->setPosition(0, 0);
        addChild(footerBackground, -1);

        addChild(_mainAreaNode);
        addChild(_footerNode);
        addChild(_reserveNode);

        // Set node positions
        _mainAreaNode->setPosition(0, 580);        // Main game area at the top
        _footerNode->setPosition(540, 290);        // Footer centered
        _reserveNode->setPosition(740, 290);       // Reserve node to the right of the footer

        // Set up background for footer and reserve
        auto footerArea = Sprite::create("res/card_general.png");
        if (footerArea) {
            footerArea->setColor(Color3B(180, 180, 220));
            footerArea->setOpacity(120);
            footerArea->setScale(1.2f);
            _footerNode->addChild(footerArea, -1);
        }

        auto reserveArea = Sprite::create("res/card_general.png");
        if (reserveArea) {
            reserveArea->setColor(Color3B(220, 180, 180));
            reserveArea->setOpacity(120);
            _reserveNode->addChild(reserveArea, -1);
        }

        // Set up touch handling for reserve area
        setupReserveTouch();
    }

    void setupReserveTouch()
    {
        // Remove existing touch listener
        if (_reserveTouchListener) {
            _eventDispatcher->removeEventListener(_reserveTouchListener);
            _reserveTouchListener = nullptr;
        }

        // Create new touch listener
        _reserveTouchListener = EventListenerTouchOneByOne::create();
        _reserveTouchListener->setSwallowTouches(true);

        _reserveTouchListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
            if (!_reserveNode || !_reserveNode->isVisible()) {
                return false;
            }

            // Convert touch location to reserve node's coordinate system
            Vec2 locationInNode = _reserveNode->convertToNodeSpace(touch->getLocation());
            CCLOG("GameView: Reserve touch at (%.1f, %.1f) in node space", locationInNode.x, locationInNode.y);

            // Enlarge touch area to ensure detection
            Size areaSize(200, 200); // Expanded touch area
            Rect touchArea(-areaSize.width / 2, -areaSize.height / 2, areaSize.width, areaSize.height);

            if (touchArea.containsPoint(locationInNode)) {
                CCLOG("GameView: Reserve touch began - area hit");
                return true;
            }
            return false;
            };

        _reserveTouchListener->onTouchEnded = [this](Touch* touch, Event* event) {
            CCLOG("GameView: Reserve touch ended - calling callback");
            if (_reserveClickCallback) {
                _reserveClickCallback();
            }
            };

        // Add listener to event dispatcher with scene graph priority
        _eventDispatcher->addEventListenerWithSceneGraphPriority(_reserveTouchListener, _reserveNode);
    }

    std::unordered_map<int, CardView*> _cardViewMap; // Map of card ID to card view
    std::function<void(int)> _cardTouchCallback; // Callback for card clicks
    std::function<void()> _reserveClickCallback; // Callback for reserve clicks

    cocos2d::Node* _mainAreaNode; // Main game area node
    cocos2d::Node* _footerNode;   // Footer node
    cocos2d::Node* _reserveNode;  // Reserve node

    cocos2d::Sprite* _reserveBackground; // Reserve background
    cocos2d::EventListenerTouchOneByOne* _reserveTouchListener; // Reserve touch listener
};

#endif // GAME_VIEW_H
