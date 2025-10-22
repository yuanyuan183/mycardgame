#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "cocos2d.h"
#include "../models/GameModel.h"
#include "../views/GameView.h"
#include "../managers/UndoManager.h"
#include "../configs/loaders/LevelConfigLoader.h"
#include "../services/GameModelFromLevelGenerator.h"
#include "../utils/GameUtils.h"

// Forward declarations
class LevelConfigLoader;
class GameModelFromLevelGenerator;

/**
 * Game controller
 * Manages the game flow and coordinates interactions between modules
 */
class GameController
{
public:
    GameController()
        : _currentGameModel(nullptr), _currentGameView(nullptr), _historyManager(nullptr), _levelGenerator(nullptr)
    {
        _historyManager = new UndoManager();
        _levelGenerator = new GameModelFromLevelGenerator();
    }

    ~GameController()
    {
        delete _currentGameModel;
        delete _currentGameView;
        delete _historyManager;
        delete _levelGenerator;
    }

    /**
     * Start a new game
     * @param levelId Level ID
     */
    void beginGame(int levelId)
    {
        CCLOG("GameController: Starting game with level %d", levelId);

        // 1. Load level configuration
        auto& levelConfig = LevelConfigLoader::loadLevelConfig(levelId);

        // 2. Generate game model from level configuration
        _currentGameModel = _levelGenerator->createGameModelFromLevel(levelConfig);

        if (!_currentGameModel) {
            CCLOG("GameController: Failed to generate game model");
            return;
        }

        // 3. Create game view
        _currentGameView = GameView::create();
        if (!_currentGameView) {
            CCLOG("GameController: Failed to create game view");
            return;
        }

        // 4. Set up callbacks
        initializeViewCallbacks();

        // 5. Update the game view
        _currentGameView->updateDisplay(_currentGameModel);

        // 6. Initialize history manager
        _historyManager->setup();

        CCLOG("GameController: Game started successfully");
    }

    /**
     * Handle card click event
     * @param cardId Card ID
     * @return True if handled successfully
     */
    bool handleCardSelection(int cardId)
    {
        if (!_currentGameModel || !_currentGameView) {
            return false;
        }

        CCLOG("GameController: Card clicked: %d", cardId);

        // Get the clicked card and the current bottom card
        CardModel* selectedCard = _currentGameModel->getCardById(cardId);
        CardModel* currentBottomCard = _currentGameModel->getActiveCard();

        if (!selectedCard || !currentBottomCard) {
            CCLOG("GameController: Invalid card or bottom card");
            return false;
        }

        // Check if the cards match
        if (areCardsMatching(selectedCard, currentBottomCard)) {
            CCLOG("GameController: Cards match! %d and %d", selectedCard->getCardValue(), currentBottomCard->getCardValue());

            // Execute match handling
            processCardMatch(selectedCard, currentBottomCard);
            return true;
        }
        else {
            CCLOG("GameController: Cards don't match. %d and %d", selectedCard->getCardValue(), currentBottomCard->getCardValue());
        }

        return false;
    }

    /**
     * Handle draw card event
     */
    void handleCardDraw()
    {
        if (!_currentGameModel || !_currentGameView) {
            return;
        }

        // Check if there are cards left in the stack
        if (_currentGameModel->getReserveCards().empty()) {
            CCLOG("GameController: No cards left in stack");
            return;
        }

        CCLOG("GameController: Drawing card from stack");

        // Draw a card from the stack
        drawCardFromDeck();
    }

    /**
     * Handle undo operation
     */
    void handleUndo()
    {
        if (!_currentGameModel || !_currentGameView || !_historyManager->hasUndoableActions()) {
            CCLOG("GameController: Cannot undo - gameModel=%p, gameView=%p, canUndo=%d",
                _currentGameModel, _currentGameView, _historyManager ? _historyManager->hasUndoableActions() : false);
            return;
        }

        UndoModel* undoModel = _historyManager->removeUndoRecord();
        if (!undoModel) {
            CCLOG("GameController: No undo record found");
            return;
        }

        CCLOG("GameController: Undoing operation type: %d", (int)undoModel->getOperationType());

        // Perform undo based on operation type
        undoPreviousAction(undoModel);

        delete undoModel;
    }

    /**
     * Get the game view (to add to the scene)
     */
    cocos2d::Node* getGameView() const { return _currentGameView; }

    /**
     * Get the current game model (for debugging)
     */
    GameModel* getGameModel() const { return _currentGameModel; }

private:
    /**
     * Set up view callbacks
     */
    void initializeViewCallbacks()
    {
        _currentGameView->setCardClickCallback([this](int cardId) {
            this->handleCardSelection(cardId);
            });

        _currentGameView->setReserveClickCallback([this]() {
            this->handleCardDraw();
            });
    }

    /**
     * Check if two cards match
     * @param card1 Card 1
     * @param card2 Card 2
     * @return True if they match
     */
    bool areCardsMatching(const CardModel* card1, const CardModel* card2) const
    {
        if (!card1 || !card2) {
            return false;
        }

        int value1 = card1->getCardValue();
        int value2 = card2->getCardValue();

        return GameUtils::areCardsAdjacent(value1, value2);
    }

    /**
     * Process card match
     * @param selectedCard Clicked card
     * @param currentBottomCard Current bottom card
     */
    void processCardMatch(CardModel* selectedCard, CardModel* currentBottomCard)
    {
        // Record previous positions
        cocos2d::Vec2 selectedCardPosition = selectedCard->getLocation();

        // Create undo record
        UndoModel* undoModel = new UndoModel();
        undoModel->setOperationType(ActionType::MATCH_CARD);
        undoModel->setMatchedCardId(selectedCard->getItemId());
        undoModel->setPreviousBottomCardId(currentBottomCard->getItemId());
        undoModel->setMatchedCardPosition(selectedCardPosition);
        undoModel->setPreviousBottomPosition(cocos2d::Vec2::ZERO); // Bottom card is at (0,0)

        _historyManager->addUndoRecord(undoModel);

        // Immediately remove card from play field
        _currentGameModel->setCardAction(new RemoveCardAction(selectedCard->getItemId()));
        _currentGameModel->performCardAction();

        // Play card move animation to the bottom position
        animateCardMovement(selectedCard, currentBottomCard);
    }

    /**
     * Play the card move animation
     * @param selectedCard Clicked card
     * @param currentBottomCard Current bottom card
     */
    void animateCardMovement(CardModel* selectedCard, CardModel* currentBottomCard)
    {
        // Calculate target position
        auto bottomNodeWorldPos = _currentGameView->getFooterNode()->convertToWorldSpace(cocos2d::Vec2::ZERO);
        auto playFieldNode = _currentGameView->getMainAreaNode();
        auto targetInPlayField = playFieldNode->convertToNodeSpace(bottomNodeWorldPos);

        // Play animation
        _currentGameView->animateCardMovement(
            selectedCard->getItemId(),
            targetInPlayField,
            0.5f,
            [this, selectedCard, currentBottomCard]() {
                // After animation, update the model
                // Place the previous bottom card back into the stack
                if (currentBottomCard) {
                    std::vector<CardModel*> stackCards = _currentGameModel->getReserveCards();
                    stackCards.insert(stackCards.begin(), currentBottomCard);
                    _currentGameModel->setReserveCards(stackCards);
                }

                // Set new bottom card and update location
                selectedCard->setLocation(cocos2d::Vec2::ZERO); // Position at the bottom card node
                _currentGameModel->setActiveCard(selectedCard);

                // Update view
                this->_currentGameView->updateDisplay(this->_currentGameModel);
                CCLOG("GameController: Card moved to bottom successfully");
            }
        );
    }

    /**
     * Draw a card from the stack and update the view
     */
    void drawCardFromDeck()
    {
        // Record previous bottom card
        CardModel* previousBottomCard = _currentGameModel->getActiveCard();

        // Draw a card from the reserve deck
        _currentGameModel->setCardAction(new DrawCardAction());
        CardModel* drawnCard = _currentGameModel->performDrawCardAction();
        if (drawnCard) {
            // Create undo record
            UndoModel* undoModel = new UndoModel();
            undoModel->setOperationType(ActionType::DRAW_CARD);
            if (previousBottomCard) {
                undoModel->setPreviousBottomCardId(previousBottomCard->getItemId());
            }
            undoModel->setNewBottomCardId(drawnCard->getItemId());

            _historyManager->addUndoRecord(undoModel);

            // Play animation
            animateCardMovement(drawnCard, previousBottomCard);
        }
    }

    /**
     * Undo the operation
     * @param undoModel Undo record
     */
    void undoPreviousAction(UndoModel* undoModel)
    {
        switch (undoModel->getOperationType()) {
        case ActionType::MATCH_CARD:
            processCardUndo(undoModel);
            break;
        case ActionType::DRAW_CARD:
            processCardDrawUndo(undoModel);
            break;
        default:
            CCLOG("GameController: Unknown operation type for undo");
            break;
        }
    }

    /**
     * Undo card match
     * @param undoModel Undo record
     */
    void processCardUndo(UndoModel* undoModel)
    {
        CardModel* matchedCard = _currentGameModel->getCardById(undoModel->getMatchedCardId());
        CardModel* previousBottomCard = _currentGameModel->getCardById(undoModel->getPreviousBottomCardId());

        if (matchedCard && previousBottomCard) {
            // Restore the matched card to the play field
            matchedCard->setLocation(undoModel->getMatchedCardPosition());
            std::vector<CardModel*> playFieldCards = _currentGameModel->getFieldCards();
            playFieldCards.push_back(matchedCard);
            _currentGameModel->setFieldCards(playFieldCards);

            // Restore the previous bottom card
            previousBottomCard->setLocation(cocos2d::Vec2::ZERO); // At bottom card node center
            _currentGameModel->setActiveCard(previousBottomCard);

            // Update the view
            _currentGameView->updateDisplay(_currentGameModel);
        }
    }

    /**
     * Undo card draw
     * @param undoModel Undo record
     */
    void processCardDrawUndo(UndoModel* undoModel)
    {
        CardModel* newBottomCard = _currentGameModel->getCardById(undoModel->getNewBottomCardId());
        CardModel* previousBottomCard = _currentGameModel->getCardById(undoModel->getPreviousBottomCardId());

        if (newBottomCard && previousBottomCard) {
            // Return drawn card to the stack and restore previous bottom card
            newBottomCard->setReversed(false);
            std::vector<CardModel*> stackCards = _currentGameModel->getReserveCards();
            stackCards.push_back(newBottomCard);
            _currentGameModel->setReserveCards(stackCards);

            previousBottomCard->setLocation(undoModel->getPreviousBottomPosition());
            previousBottomCard->setReversed(true);
            _currentGameModel->setActiveCard(previousBottomCard);

            // Update the view
            _currentGameView->updateDisplay(_currentGameModel);
        }
    }

private:
    GameModel* _currentGameModel;
    GameView* _currentGameView;
    UndoManager* _historyManager;
    GameModelFromLevelGenerator* _levelGenerator;
};

#endif // GAME_CONTROLLER_H
