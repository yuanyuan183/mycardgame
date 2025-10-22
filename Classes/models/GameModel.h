#pragma once
#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "cocos2d.h"
#include "CardModel.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

// Forward declaration
class CardModel;

// Card Action Interface
class CardAction {
public:
    virtual ~CardAction() {}
    virtual void performAction(std::vector<CardModel*>& fieldCards, std::vector<CardModel*>& reserveCards, CardModel* activeCard, std::unordered_map<int, CardModel*>& cardDictionary) = 0;
};

// Action Class: Remove Card
class RemoveCardAction : public CardAction {
public:
    RemoveCardAction(int cardId) : _cardId(cardId) {}

    void performAction(std::vector<CardModel*>& fieldCards, std::vector<CardModel*>& reserveCards, CardModel* activeCard, std::unordered_map<int, CardModel*>& cardDictionary) override {
        for (auto it = fieldCards.begin(); it != fieldCards.end(); ++it) {
            if ((*it)->getItemId() == _cardId) {
                fieldCards.erase(it);
                cardDictionary.erase(_cardId);
                break;
            }
        }
    }

private:
    int _cardId;
};

// Action Class: Draw Card
class DrawCardAction : public CardAction {
public:
    DrawCardAction() {}

    void performAction(std::vector<CardModel*>& fieldCards, std::vector<CardModel*>& reserveCards, CardModel* activeCard, std::unordered_map<int, CardModel*>& cardDictionary) override {
        if (reserveCards.empty()) {
            CCLOG("GameModel: No more cards in the reserve stack");
            return;
        }
        CardModel* drawnCard = reserveCards.back();
        reserveCards.pop_back();
        cardDictionary.erase(drawnCard->getItemId());

        // Store the drawn card for future reference
        _drawnCard = drawnCard;
    }

    // Retrieve the drawn card
    CardModel* getDrawnCard() const { return _drawnCard; }

private:
    CardModel* _drawnCard = nullptr;  // Store the drawn card
};

// Action Class: Add Card to Active Position
class AddCardToActivePositionAction : public CardAction {
public:
    AddCardToActivePositionAction(CardModel* card) : _card(card) {}

    void performAction(std::vector<CardModel*>& fieldCards, std::vector<CardModel*>& reserveCards, CardModel* activeCard, std::unordered_map<int, CardModel*>& cardDictionary) override {
        if (activeCard) {
            reserveCards.insert(reserveCards.begin(), activeCard);
        }
        activeCard = _card;
        if (_card) {
            cardDictionary[_card->getItemId()] = _card;
        }
    }

private:
    CardModel* _card;
};

/**
 * Game State Model
 * Responsible for storing and managing the game's current state and data
 */
class GameModel
{
public:
    GameModel() : _activeCard(nullptr), _currentAction(nullptr) {}
    ~GameModel()
    {
        // Cleanup all card objects
        for (auto card : _fieldCards) {
            delete card;
        }
        for (auto card : _reserveCards) {
            delete card;
        }
        if (_activeCard) {
            delete _activeCard;
        }
    }

    // Accessor and Mutator for the field cards
    const std::vector<CardModel*>& getFieldCards() const { return _fieldCards; }
    void setFieldCards(const std::vector<CardModel*>& cards)
    {
        _fieldCards = cards;
        updateCardDictionary();
    }

    // Accessor and Mutator for the active card (the currently displayed card)
    CardModel* getActiveCard() const { return _activeCard; }
    void setActiveCard(CardModel* card)
    {
        _activeCard = card;
        if (card) {
            _cardDictionary[card->getItemId()] = card;
        }
    }

    // Accessor and Mutator for the reserve cards
    const std::vector<CardModel*>& getReserveCards() const { return _reserveCards; }
    void setReserveCards(const std::vector<CardModel*>& cards)
    {
        _reserveCards = cards;
        updateCardDictionary();
    }

    // Retrieve card by ID
    CardModel* getCardById(int cardId) const
    {
        auto it = _cardDictionary.find(cardId);
        if (it != _cardDictionary.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Set the current card action
    void setCardAction(CardAction* action) {
        _currentAction = action;
    }

    // Perform the current card action
    void performCardAction() {
        if (_currentAction) {
            _currentAction->performAction(_fieldCards, _reserveCards, _activeCard, _cardDictionary);
        }
    }

    // Perform draw card action and return the drawn card
    CardModel* performDrawCardAction() {
        if (_currentAction) {
            _currentAction->performAction(_fieldCards, _reserveCards, _activeCard, _cardDictionary);
            if (auto drawAction = dynamic_cast<DrawCardAction*>(_currentAction)) {
                return drawAction->getDrawnCard(); // Return the drawn card
            }
        }
        return nullptr;
    }

private:
    std::vector<CardModel*> _fieldCards; // Cards on the field
    CardModel* _activeCard; // The currently active (visible) card
    std::vector<CardModel*> _reserveCards; // The reserve stack of cards
    std::unordered_map<int, CardModel*> _cardDictionary; // A map for quick card lookup by ID
    CardAction* _currentAction; // The current card action being executed

    // Update the card dictionary with the current cards
    void updateCardDictionary()
    {
        _cardDictionary.clear();
        for (auto card : _fieldCards) {
            _cardDictionary[card->getItemId()] = card;
        }
        for (auto card : _reserveCards) {
            _cardDictionary[card->getItemId()] = card;
        }
        if (_activeCard) {
            _cardDictionary[_activeCard->getItemId()] = _activeCard;
        }
    }
};

#endif // GAME_MODEL_H
