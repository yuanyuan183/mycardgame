#pragma once
#ifndef LEVEL_TO_GAME_MODEL_BUILDER_H
#define LEVEL_TO_GAME_MODEL_BUILDER_H

#include "../models/CardModel.h"
#include "../configs/models/LevelConfig.h"
#include "../utils/GameUtils.h"

/**
 * �������ɸ�����
 * ������ʵ�����Ĳ���
 */
class CardAssembler
{
public:
    /**
     * ���ɿ���ʵ��
     * @param cardConfig ��������
     * @param cardId ����ID
     * @return �µĿ���ʵ��
     */
    CardModel* assembleCard(const LevelConfig::CardItem& cardConfig, int cardId)
    {
        CardModel* card = new CardModel();
        card->setItemId(cardId);
        card->setFaceType(cardConfig.cardValue);
        card->setSuitType(cardConfig.cardSuit);
        card->setLocation(cardConfig.cardPosition);
        card->setReversed(true); // Ĭ�Ϸ���״̬

        return card;
    }
};

/**
 * ��Ϸģ�͹�������
 * ���ؿ�����ת��Ϊ��̬��Ϸģ������
 */
class GameModelFromLevelGenerator
{
public:
    GameModelFromLevelGenerator() : _cardAssembler(new CardAssembler()) {}
    ~GameModelFromLevelGenerator() { delete _cardAssembler; }

    /**
     * ���ݹؿ�����������Ϸģ��
     * @param levelConfig �ؿ�����
     * @return ��Ϸģ��ʵ��
     */
    GameModel* createGameModelFromLevel(LevelConfig& levelConfig)
    {
        GameModel* newGameModel = new GameModel();

        // �����������Ŀ���
        std::vector<CardModel*> playAreaCards;
        const std::vector<LevelConfig::CardItem>& playAreaConfigurations = levelConfig.getMainAreaCards();
        for (const auto& cardConfig : playAreaConfigurations) {
            int cardId = GameUtils::createUniqueCardId();
            CardModel* card = _cardAssembler->assembleCard(cardConfig, cardId);
            if (card) {
                playAreaCards.push_back(card);
            }
        }
        newGameModel->setFieldCards(playAreaCards);

        // ���ɱ��������Ŀ���
        std::vector<CardModel*> reserveCards;
        const std::vector<LevelConfig::CardItem>& reserveConfigurations = levelConfig.getBackupAreaCards();
        for (const auto& cardConfig : reserveConfigurations) {
            int cardId = GameUtils::createUniqueCardId();
            CardModel* card = _cardAssembler->assembleCard(cardConfig, cardId);
            if (card) {
                reserveCards.push_back(card);
            }
        }
        newGameModel->setReserveCards(reserveCards);

        // ���ó�ʼ���ƣ��ӱ����ƶѳ�ȡ��һ�ţ�
        if (!reserveCards.empty()) {
            std::vector<CardModel*> remainingReserveCards = reserveCards;
            CardModel* firstCard = remainingReserveCards.back();
            remainingReserveCards.pop_back();

            newGameModel->setActiveCard(firstCard);
            newGameModel->setReserveCards(remainingReserveCards);
        }

        return newGameModel;
    }

private:
    // �������ɸ�������ʵ��
    CardAssembler* _cardAssembler;
};

#endif // LEVEL_TO_GAME_MODEL_BUILDER_H
