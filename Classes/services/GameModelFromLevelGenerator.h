#pragma once
#ifndef LEVEL_TO_GAME_MODEL_BUILDER_H
#define LEVEL_TO_GAME_MODEL_BUILDER_H

#include "../models/CardModel.h"
#include "../configs/models/LevelConfig.h"
#include "../utils/GameUtils.h"

/**
 * 卡牌生成辅助类
 * 负责卡牌实例化的操作
 */
class CardAssembler
{
public:
    /**
     * 生成卡牌实例
     * @param cardConfig 卡牌配置
     * @param cardId 卡牌ID
     * @return 新的卡牌实例
     */
    CardModel* assembleCard(const LevelConfig::CardItem& cardConfig, int cardId)
    {
        CardModel* card = new CardModel();
        card->setItemId(cardId);
        card->setFaceType(cardConfig.cardValue);
        card->setSuitType(cardConfig.cardSuit);
        card->setLocation(cardConfig.cardPosition);
        card->setReversed(true); // 默认翻开状态

        return card;
    }
};

/**
 * 游戏模型构建服务
 * 将关卡配置转换为动态游戏模型数据
 */
class GameModelFromLevelGenerator
{
public:
    GameModelFromLevelGenerator() : _cardAssembler(new CardAssembler()) {}
    ~GameModelFromLevelGenerator() { delete _cardAssembler; }

    /**
     * 根据关卡配置生成游戏模型
     * @param levelConfig 关卡配置
     * @return 游戏模型实例
     */
    GameModel* createGameModelFromLevel(LevelConfig& levelConfig)
    {
        GameModel* newGameModel = new GameModel();

        // 生成主牌区的卡牌
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

        // 生成备用牌区的卡牌
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

        // 设置初始底牌（从备用牌堆抽取第一张）
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
    // 卡牌生成辅助工具实例
    CardAssembler* _cardAssembler;
};

#endif // LEVEL_TO_GAME_MODEL_BUILDER_H
